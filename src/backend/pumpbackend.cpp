// PumpBackend.cpp
// 知识点：
//   UPDATE 宏：把计算抖动值 → 变化才赋值 → 发 NOTIFY三步收敛成
//     一行，避免 19 个模拟量写 19 遍重复逻辑（宏仅存在于本 .cpp，不影响接口）。
//   qFuzzyCompare：浮点判等不能直接用 ==，用容差比较安全。
//   connect 新语法：信号带参数连到无参槽是允许的（多余参数会被丢弃）。
#include "pumpbackend.h"
#include "devicemodel.h"
#include "reportmodel.h"
#include "alarmmodel.h"
#include "datastore.h"

#include <QRandomGenerator>
#include <QtNumeric>   // qFuzzyCompare 浮点容差比较

// Modbus TCP（QModbusTcpClient 作为 主站/客户端 读取从站寄存器）
#include <QModbusTcpClient>
#include <QModbusReply>
#include <QModbusDataUnit>
#include <QModbusDevice>
#include <QVariant>

// 更新一个模拟量：base 基准值，amp 抖动幅度
#define UPDATE(name, base, amp) do { \
    const double _v = jitter(base, amp); \
    if (!qFuzzyCompare(_v, m_##name)) { m_##name = _v; emit name##Changed(); } \
} while (false)

// 从 DeviceModel 按键同步一个运行态布尔量
#define SYNC_BOOL(name, key) do { \
    const bool _v = m_devices->runningByKey(key); \
    if (_v != m_##name) { m_##name = _v; emit name##Changed(); } \
} while (false)

// 把 Modbus 读到的原始寄存器值(raw，quint16)按缩放因子(scale)换算后写入遥测属性
#define APPLY(name, raw, scale) do { \
    const double _v = double(raw) / (scale); \
    if (!qFuzzyCompare(_v, m_##name)) { m_##name = _v; emit name##Changed(); } \
} while (false)

// Modbus 保持寄存器映射表（HMI 读 Modbus 从站寄存器刷新界面）
// 地址 0 起 = Modbus Slave 软件里的 40001；一次读 28 个寄存器。
//   前 18 个(0~17)  = 模拟量（缩放：温度/流量/电导率/电流/耗电×10，压力/液位×1000）
//   后 10 个(18~27) = 设备开关量（0=停/关，非 0=运行/开）
static const int MODBUS_REG_COUNT = 28;
// 断链自动重连间隔（秒）：从站长时间不可达时不高频打点，按此间隔周期性重试连接
static const int MODBUS_RECONNECT_INTERVAL = 5;

PumpBackend::PumpBackend(QObject *parent)
    : QObject(parent)
{
    // 子模型用 this 作父对象，生命周期随 Backend 自动释放（Qt 父子关系）
    m_devices = new DeviceModel(this);
    m_reports = new ReportModel(this);
    m_alarms  = new AlarmModel(this);

    // SQLite 数据存储（小型系统 SQLite 即可，Navicat 直接打开 .db）
    m_store = new DataStore(this);
    m_store->open();

    // 报警状态流转（新增/确认/消除）实时写库（报警事件记录）
    connect(m_alarms, &AlarmModel::stateChanged, this,
            [this](int level, const QString &type, const QString &state) {
                if (m_store)
                    m_store->logAlarm(level, type, state);
            });

    // 设备模型数据一变，同步运行态到本类布尔属性（供工艺流程页状态灯）
    connect(m_devices, &DeviceModel::dataChanged, this, &PumpBackend::syncDeviceStates);

    //  Modbus TCP 客户端（HMI 为客户端/主站，读 Modbus Slave 软件） 
    // 知识点：连接参数用 NetworkAddress/NetworkPort 两个键配置 IP 与端口；
    // setTimeout/retries 控制超时与重试，保证从站没启动时快速失败、回退模拟器。
    m_modbus = new QModbusTcpClient(this);
    m_modbus->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_modbusHost);
    m_modbus->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_modbusPort);
    m_modbus->setTimeout(1000);
    m_modbus->setNumberOfRetries(1);
    connect(m_modbus, &QModbusTcpClient::stateChanged, this, &PumpBackend::onModbusStateChanged);

    // 模拟器心跳
    connect(&m_timer, &QTimer::timeout, this, &PumpBackend::onTick);
    m_timer.setInterval(1000);

    // 初始同步一次，保证页面打开即有正确状态灯
    syncDeviceStates();
}

QAbstractItemModel* PumpBackend::devices() const { return m_devices; }
QAbstractItemModel* PumpBackend::reports() const { return m_reports; }
QAbstractItemModel* PumpBackend::alarms()  const { return m_alarms; }

QString PumpBackend::dbPath() const { return m_store ? m_store->dbPath() : QString(); }

void PumpBackend::start()
{
    if (!m_timer.isActive())
        m_timer.start();
    // 尝试连接 Modbus Slave：失败不影响启动，跑模拟器兜底。
    if (m_modbusEnabled && m_modbus)
        m_modbus->connectDevice();
}

double PumpBackend::jitter(double base, double amp) const
{
    // 知识点：QRandomGenerator::global()->generateDouble() 返回 [0,1)，
    // 减 0.5 后得到 [-0.5,0.5)，再乘幅度实现基准值附近抖动。
    if (amp <= 0.0)
        return base;
    return base + (QRandomGenerator::global()->generateDouble() - 0.5) * amp;
}

void PumpBackend::onTick()
{
    // 知识点：数据源三态——
    //   ① 已连接：读外部寄存器驱动（异步，结果在 onModbusReadFinished 续跑控制逻辑）；
    //   ② 已启用但未连接：按固定间隔自动重连（断链/重连无需重开页面）；
    //   ③ 未连接/未启用：内部模拟器兜底，保证页面始终有数据。
    if (m_modbusEnabled && m_modbusConnected) {
        pollModbus();
        return;
    }

    if (m_modbusEnabled && m_modbus) {
        const QModbusDevice::State st = m_modbus->state();
        if (st == QModbusDevice::UnconnectedState) {
            // 断链自动重连：每 MODBUS_RECONNECT_INTERVAL 秒重试一次，
            // 避免从站长时间不可达时高频发起无谓的连接请求。
            if (++m_reconnectTick >= MODBUS_RECONNECT_INTERVAL) {
                m_reconnectTick = 0;
                m_modbus->connectDevice();
            }
        } else if (st == QModbusDevice::ConnectingState) {
            // 知识点：connectDevice() 对不可达主机（SYN 无应答）可能长时间卡在
            // ConnectingState 不返回，若不主动断开将永远无法重连；
            // 这里超时后强制 disconnectDevice()，让其回到 UnconnectedState，
            // 下一轮重新发起连接，保证从站后上线也能及时对接。
            if (++m_reconnectTick >= MODBUS_RECONNECT_INTERVAL * 2) {
                m_reconnectTick = 0;
                m_modbus->disconnectDevice();
            }
        } else {
            m_reconnectTick = 0;   // ClosingState 等过渡态，暂停计数
        }
    }

    updateTelemetrySimulated();
    runControlLogic();
}

void PumpBackend::updateTelemetrySimulated()
{
    //  温度（℃） 
    UPDATE(supplyTemp,        32.07, 0.8);
    UPDATE(returnTemp,        47.20, 0.8);
    UPDATE(coolingTowerOutlet, 34.70, 0.5);
    UPDATE(exchangerIn,       31.68, 0.4);
    UPDATE(exchangerOut,      31.61, 0.4);

    //  压力（MPa） 
    UPDATE(supplyPressure, 0.539, 0.02);
    UPDATE(returnPressure, 0.412, 0.02);
    UPDATE(makeupPressure, 0.200, 0.01);

    //  流量/电导率/液位 
    UPDATE(supplyFlow, 69.2, 2.0);
    m_cumulativeFlow += 0.02;                 // 累计量单调递增更真实
    emit cumulativeFlowChanged();
    UPDATE(conductivity,  596.2, 3.0);
    UPDATE(tankLevel,     2.573, 0.02);
    UPDATE(expansionLevel, 0.62,  0.02);

    //  泵电流（A）：运行态才有电流，停止为 0 
    double c = m_cyclePump1Running ? jitter(40.4, 1.0) : 0.0;
    if (!qFuzzyCompare(c, m_cyclePump1Current)) { m_cyclePump1Current = c; emit cyclePump1CurrentChanged(); }
    c = m_cyclePump2Running ? jitter(15.0, 0.5) : 0.0;
    if (!qFuzzyCompare(c, m_cyclePump2Current)) { m_cyclePump2Current = c; emit cyclePump2CurrentChanged(); }
    c = m_makeupPump1Running ? jitter(1.6, 0.1) : 0.0;
    if (!qFuzzyCompare(c, m_makeupPump1Current)) { m_makeupPump1Current = c; emit makeupPump1CurrentChanged(); }
    c = m_makeupPump2Running ? jitter(1.4, 0.1) : 0.0;
    if (!qFuzzyCompare(c, m_makeupPump2Current)) { m_makeupPump2Current = c; emit makeupPump2CurrentChanged(); }

    //  能效 / 能耗 KPI（能效优化、累计耗电/运行时间、COP） 
    UPDATE(cop, 6.2, 0.15);          // 系统 COP 在基准值附近小幅波动
    UPDATE(wetBulbTemp, 26.4, 0.5);  // 环境湿球温度（影响冷却塔出水设定）
    m_dailyPower += 0.015;           // 当日耗电单调累加（约合 1300 kWh/天，更真实）
    emit dailyPowerChanged();
    m_pumpRunHours += 1.0 / 3600.0;  // 每秒累计 1 秒（换算成小时）
    emit pumpRunHoursChanged();
}

void PumpBackend::runControlLogic()
{
    // 控制回路（各核心模块；真实为 PLC 做 PID 运算，此处用
    // 基准 + 误差×比例系数模拟趋势，qBound 夹在物理上下限内）
    // 知识点：qBound(min,v,max) 等价 min(max(v,min),max)，Qt 全局辅助函数。

    // 1) 冷却塔风机变频调速（图4风机变频调速）：出水温度高于设定 → 升频散热
    m_coolingFanFreq = qBound(20.0, 30.0 + (m_coolingTowerOutlet - m_coolingSetpoint) * 5.0, 50.0);
    emit coolingFanFreqChanged();

    // 2) 供水泵变频恒压（图4变频恒压供水）：压力低于设定 → 升频增压
    m_supplyPumpFreq = qBound(20.0, 42.0 + (m_pressSetpoint - m_supplyPressure) * 60.0, 50.0);
    emit supplyPumpFreqChanged();

    // 3) 板换一次侧电动阀开度（5.2一次侧流量调节）：二次侧出口温度高于设定 → 开大阀门增换热
    m_valvePos1 = qBound(0.0, 40.0 + (m_exchangerOut - m_tempSetpoint) * 3.0, 100.0);
    emit valvePos1Changed();

    // 4) 板换二次侧旁通阀（图4二次侧旁通）：实际流量高于目标 → 开大旁通阀旁泄，维持流量恒定
    m_valvePos2 = qBound(0.0, (m_supplyFlow - 65.0) * 2.0, 100.0);
    emit valvePos2Changed();

    // 5) 循环泵组等时轮换（图4多泵轮换）：每 120 秒在 #1/#2 间轮换主泵，#3 恒为 N+1 备用
    //    知识点：真实系统按累计运行时间（数千小时）等时轮换，此处缩短周期便于观察。
    //    Modbus 模式下泵状态由外部寄存器输入，跳过本地轮换，避免覆盖外部指令。
    if (!m_modbusConnected && ++m_rotationTick >= 120) {
        m_rotationTick = 0;
        // 手动模式（manual=true）的泵由用户在控制页手动启停，不参与自动轮换，
        // 否则点“停止”后会在下一个轮换周期被自动“启动”而回弹。
        if (!m_devices->manualByKey(QStringLiteral("cyclePump1"))
                && !m_devices->manualByKey(QStringLiteral("cyclePump2"))) {
            m_activePumpIndex = (m_activePumpIndex + 1) % 2;
            if (m_activePumpIndex == 0) {
                m_devices->startByKey(QStringLiteral("cyclePump1"));
                m_devices->stopByKey(QStringLiteral("cyclePump2"));
            } else {
                m_devices->startByKey(QStringLiteral("cyclePump2"));
                m_devices->stopByKey(QStringLiteral("cyclePump1"));
            }
        }
    }

    //  水质管理：排污由电导率驱动，补水由电导率排污 或 液位低驱动 
    // 知识点：排污只在电导率超标时开；补水有两条触发源——①电导率超标需稀释补软水、
    // ②补水箱液位低需定压补水（5.7 滞回控制：低于下限置位，恢复至上限才复位，避免频繁开关）。
    const bool dirty = m_conductivity > m_conductivityLimit;   // 电导率超标(>500μS/cm)
    if (dirty != m_autoDrain) {
        m_autoDrain = dirty;
        // 手动模式不自动排污：避免用户手动关闭排污阀后被“电导率超标”逻辑重新打开而回弹。
        if (!m_devices->manualByKey(QStringLiteral("drainValve"))) {
            if (dirty) m_devices->startByKey(QStringLiteral("drainValve"));
            else       m_devices->stopByKey(QStringLiteral("drainValve"));
        }
    }
    if (m_tankLevel < m_tankLevelLow)             // 液位低于下限 → 请求补水
        m_levelMakeup = true;
    else if (m_tankLevel > m_tankLevelHigh)       // 液位恢复至高限以上 → 取消请求
        m_levelMakeup = false;
    const bool needMakeup = dirty || m_levelMakeup;   // 补水阀 = 排污稀释 或 液位补水
    if (needMakeup != m_makeupActive) {
        m_makeupActive = needMakeup;
        // 手动模式不自动补水：避免用户手动关闭补水阀后被补水逻辑重新打开而回弹。
        if (!m_devices->manualByKey(QStringLiteral("makeupValve"))) {
            if (needMakeup) m_devices->startByKey(QStringLiteral("makeupValve"));
            else            m_devices->stopByKey(QStringLiteral("makeupValve"));
        }
    }

    //  电导率报警：>800μS/cm 且持续 30 分钟才触发二级报警 
    // 知识点：连续超限计时，一旦回落就清零重计；累计满 1800s(30min) 只报一次，
    // 避免电导率在阈值附近抖动时误报（对比之前上升沿即时触发的做法）。
    const bool over = m_conductivity > m_conductivityAlarmLimit;
    if (over) {
        if (++m_conductivityOverSeconds >= 1800 && !m_conductivityAlarm) {
            m_conductivityAlarm = true;
            emit conductivityAlarmChanged();
            m_alarms->addAlarm(2, QStringLiteral("电导率持续超标(>800μS/cm 已超30分钟)"));
        }
    } else {
        m_conductivityOverSeconds = 0;
        if (m_conductivityAlarm) {
            m_conductivityAlarm = false;
            emit conductivityAlarmChanged();
        }
    }

    //  报警联动：温度/压力越限 → 报警 + 设备保护动作 
    // 知识点：用上升沿标记，越限只在第一次出现时报一次，避免每秒刷屏；
    // 参数回到阈值内则清零，下次再越限可重新触发（模拟真实消警/复位）。
    const bool tempOverL1 = m_supplyTemp > m_tempAlarmL1;
    if (tempOverL1 && !m_tempL1Active) {
        m_tempL1Active = true;
        m_alarms->addAlarm(1, QStringLiteral("供水温度一级报警(>") + QString::number(m_tempAlarmL1, 'f', 0) + QStringLiteral("℃)"));
    }
    if (!tempOverL1) m_tempL1Active = false;

    const bool tempOverL2 = m_supplyTemp > m_tempAlarmL2;
    if (tempOverL2 && !m_tempL2Active) {
        m_tempL2Active = true;
        m_alarms->addAlarm(1, QStringLiteral("供水温度二级停泵保护(>") + QString::number(m_tempAlarmL2, 'f', 0) + QStringLiteral("℃)"));
        // 联动：紧急停泵
        m_devices->stopByKey(QStringLiteral("cyclePump1"));
        m_devices->stopByKey(QStringLiteral("cyclePump2"));
        m_devices->stopByKey(QStringLiteral("cyclePump3"));
    }
    if (!tempOverL2) m_tempL2Active = false;

    const bool pressLow = m_supplyPressure < m_pressLowAlarm;
    if (pressLow && !m_pressLowActive) {
        m_pressLowActive = true;
        m_alarms->addAlarm(1, QStringLiteral("供水压力过低(<") + QString::number(m_pressLowAlarm, 'f', 2) + QStringLiteral("MPa)"));
        // 联动：自动启动备用泵尝试恢复压力
        m_devices->startByKey(QStringLiteral("cyclePump2"));
    }
    if (!pressLow) m_pressLowActive = false;

    //  膨胀罐液位异常报警：低→可能进气/气蚀，高→可能超压/气囊破裂 
    // 知识点：三级（一般）报警用黄色提示，24h 内处理；
    // 同样用上升沿标记，避免越限期间每秒重复刷屏。
    const bool expansionLow = m_expansionLevel < m_expansionLow;
    if (expansionLow && !m_expansionLowActive) {
        m_expansionLowActive = true;
        m_alarms->addAlarm(3, QStringLiteral("膨胀罐液位过低(<")
                                 + QString::number(m_expansionLow, 'f', 2) + QStringLiteral("m, 可能进气/气蚀)"));
    }
    if (!expansionLow) m_expansionLowActive = false;

    const bool expansionHigh = m_expansionLevel > m_expansionHigh;
    if (expansionHigh && !m_expansionHighActive) {
        m_expansionHighActive = true;
        m_alarms->addAlarm(3, QStringLiteral("膨胀罐液位过高(>")
                                 + QString::number(m_expansionHigh, 'f', 2) + QStringLiteral("m, 可能超压/气囊破裂)"));
    }
    if (!expansionHigh) m_expansionHighActive = false;

    //  数据记录：每秒落一条实时采样到 SQLite 
    if (m_store)
        m_store->logRealtime(m_supplyTemp, m_supplyPressure, m_conductivity,
                             m_returnTemp, m_coolingTowerOutlet, m_supplyFlow,
                             m_tankLevel, m_cumulativeFlow, m_cop, m_dailyPower);
}

void PumpBackend::syncDeviceStates()
{
    // 知识点：流程页状态灯与设备控制面共用同一份真实状态，
    // 避免两处维护不一致。key 与 DeviceModel 构造函数里一一对应。
    SYNC_BOOL(cyclePump1Running, "cyclePump1");
    SYNC_BOOL(cyclePump2Running, "cyclePump2");
    SYNC_BOOL(cyclePump3Running, "cyclePump3");
    SYNC_BOOL(coolingFan1Running, "coolingFan1");
    SYNC_BOOL(coolingFan2Running, "coolingFan2");
    SYNC_BOOL(makeupPump1Running, "makeupPump1");
    SYNC_BOOL(makeupPump2Running, "makeupPump2");
    SYNC_BOOL(sprayPump1Running,  "sprayPump1");
    SYNC_BOOL(makeupValveOpen,    "makeupValve");
    SYNC_BOOL(drainValveOpen,     "drainValve");

    // 设备启停变化写库（设备启停事件记录）
    logDeviceTransitions();
}

void PumpBackend::logDeviceTransitions()
{
    // 知识点：只记录状态发生翻转的瞬间（上升/下降沿），避免每秒重复写。
    // 首次运行时用当前态初始化上次快照，不产生假事件。
    struct Dev { const char *key; bool running; const char *on; const char *off; };
    const Dev list[] = {
        { "cyclePump1", m_cyclePump1Running, "启动", "停止" },
        { "cyclePump2", m_cyclePump2Running, "启动", "停止" },
        { "cyclePump3", m_cyclePump3Running, "启动", "停止" },
        { "coolingFan1", m_coolingFan1Running, "启动", "停止" },
        { "coolingFan2", m_coolingFan2Running, "启动", "停止" },
        { "makeupPump1", m_makeupPump1Running, "启动", "停止" },
        { "makeupPump2", m_makeupPump2Running, "启动", "停止" },
        { "sprayPump1",  m_sprayPump1Running,  "启动", "停止" },
        { "makeupValve", m_makeupValveOpen,    "打开", "关闭" },
        { "drainValve",  m_drainValveOpen,     "打开", "关闭" },
    };
    for (const Dev &d : list) {
        const QString key = QString::fromLatin1(d.key);
        if (!m_prevRunning.contains(key)) {
            m_prevRunning[key] = d.running;   // 首次：只记录快照
            continue;
        }
        if (m_prevRunning.value(key) != d.running) {
            // 知识点：d.on/d.off 是源文件 UTF-8 编码的中文字面量，
            // 必须用 fromUtf8 按 UTF-8 解码，误用 fromLatin1 会把
            // UTF-8 字节当 Latin-1 逐字节转，写入数据库就成了乱码。
            m_store->logDeviceEvent(key, d.running ? QString::fromUtf8(d.on)
                                                   : QString::fromUtf8(d.off));
            m_prevRunning[key] = d.running;
        }
    }
}

void PumpBackend::saveParams()
{
    // 知识点：真实系统还应 Hard 写入 PLC 寄存器 + 落盘配方文件；
    // 这里把 11 项自控参数写入数据库配方表 param_recipes，
    // 便于 Navicat 按 name/value 查询每次保存的历史配方。
    if (m_store) {
        const QStringList names = {
            QStringLiteral("供水温度设定值"), QStringLiteral("供水压力设定值"),
            QStringLiteral("冷却塔出水设定"), QStringLiteral("温度PB"),
            QStringLiteral("温度Ti"), QStringLiteral("压力PB"),
            QStringLiteral("压力Ti"), QStringLiteral("温度一级报警"),
            QStringLiteral("温度二级停泵"), QStringLiteral("压力低压报警"),
            QStringLiteral("电导率上限")
        };
        const QVector<double> values = {
            m_tempSetpoint, m_pressSetpoint, m_coolingSetpoint,
            m_tempProportional, m_tempIntegral, m_pressProportional,
            m_pressIntegral, m_tempAlarmL1, m_tempAlarmL2,
            m_pressLowAlarm, m_conductivityLimit
        };
        m_store->saveRecipe(names, values);
    }
    m_saveMessage = QStringLiteral("参数已保存（已写入数据库配方表）");
    emit saveMessageChanged();
}

bool PumpBackend::login(const QString &user, const QString &password)
{
    // 知识点：HMI 本地账号校验。管理员有改参数 / 操作设备权限，需登录后才能
    // 操作。密码明文仅为演示，真实项目应存哈希（PBKDF2/BCrypt）或对接企业账号体系。
    if (user.trimmed() == QStringLiteral("admin") && password == QStringLiteral("123456")) {
        m_loggedIn = true;
        m_currentUser = QStringLiteral("admin（管理员）");
        m_loginMessage = QStringLiteral("登录成功");
        emit loggedInChanged();
        emit currentUserChanged();
        emit loginMessageChanged();
        return true;
    }
    m_loggedIn = false;
    m_currentUser.clear();
    m_loginMessage = QStringLiteral("用户名或密码错误");
    emit loggedInChanged();
    emit currentUserChanged();
    emit loginMessageChanged();
    return false;
}

void PumpBackend::logout()
{
    m_loggedIn = false;
    m_currentUser.clear();
    m_loginMessage = QStringLiteral("已退出登录");
    emit loggedInChanged();
    emit currentUserChanged();
    emit loginMessageChanged();
}

void PumpBackend::simulatePumpFault(const QString &key)
{
    // 知识点：模拟水泵过载/缺相/变频器故障——停故障泵 + 切换备用泵 + 推报警。
    // 这是可交互的演示入口：界面触发后把对应泵状态置停、启动备用泵补位，
    // 同时经 AlarmModel 生成一条一级报警（备选泵自动投入保证供水不中断）。
    m_devices->stopByKey(key);
    m_alarms->addAlarm(1, key + QStringLiteral(" 故障（过载/缺相/变频器）"));

    // 循环泵 N+1 互备：故障一台自动补位另一台
    if (key == QStringLiteral("cyclePump1"))
        m_devices->startByKey(QStringLiteral("cyclePump2"));
    else if (key == QStringLiteral("cyclePump2"))
        m_devices->startByKey(QStringLiteral("cyclePump1"));
    else if (key == QStringLiteral("cyclePump3"))
        m_devices->startByKey(QStringLiteral("cyclePump1"));
}

//  参数 setter（变化才发信号，避免无谓刷新） 
void PumpBackend::setTempSetpoint(double v)    { if (qFuzzyCompare(v, m_tempSetpoint)) return;    m_tempSetpoint = v;    emit tempSetpointChanged(); }
void PumpBackend::setPressSetpoint(double v)   { if (qFuzzyCompare(v, m_pressSetpoint)) return;   m_pressSetpoint = v;   emit pressSetpointChanged(); }
void PumpBackend::setCoolingSetpoint(double v) { if (qFuzzyCompare(v, m_coolingSetpoint)) return; m_coolingSetpoint = v; emit coolingSetpointChanged(); }
void PumpBackend::setTempProportional(double v)  { if (qFuzzyCompare(v, m_tempProportional)) return;  m_tempProportional = v;  emit tempProportionalChanged(); }
void PumpBackend::setTempIntegral(double v)      { if (qFuzzyCompare(v, m_tempIntegral)) return;      m_tempIntegral = v;      emit tempIntegralChanged(); }
void PumpBackend::setPressProportional(double v) { if (qFuzzyCompare(v, m_pressProportional)) return; m_pressProportional = v; emit pressProportionalChanged(); }
void PumpBackend::setPressIntegral(double v)     { if (qFuzzyCompare(v, m_pressIntegral)) return;     m_pressIntegral = v;     emit pressIntegralChanged(); }
void PumpBackend::setTempAlarmL1(double v)   { if (qFuzzyCompare(v, m_tempAlarmL1)) return;   m_tempAlarmL1 = v;   emit tempAlarmL1Changed(); }
void PumpBackend::setTempAlarmL2(double v)   { if (qFuzzyCompare(v, m_tempAlarmL2)) return;   m_tempAlarmL2 = v;   emit tempAlarmL2Changed(); }
void PumpBackend::setPressLowAlarm(double v) { if (qFuzzyCompare(v, m_pressLowAlarm)) return; m_pressLowAlarm = v; emit pressLowAlarmChanged(); }
void PumpBackend::setConductivityLimit(double v) { if (qFuzzyCompare(v, m_conductivityLimit)) return; m_conductivityLimit = v; emit conductivityLimitChanged(); }

//  Modbus 数据源（HMI 作为主站读取 Modbus Slave 从站寄存器） 

void PumpBackend::pollModbus()
{
    // 知识点：一次读 28 个保持寄存器（0~27），避免多个异步请求间的同步复杂度。
    // sendReadRequest 返回 QModbusReply*（失败返回 nullptr），连接其 finished 信号
    // 到无参槽；读取结果在 onModbusReadFinished 里用 qobject_cast(sender()) 取回。
    if (!m_modbus)
        return;
    const QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0, MODBUS_REG_COUNT);
    if (auto *reply = m_modbus->sendReadRequest(readUnit, 1))      // 从站地址=1（Modbus Slave 软件默认站号）
        connect(reply, &QModbusReply::finished, this, &PumpBackend::onModbusReadFinished);
}

void PumpBackend::onModbusReadFinished()
{
    // 知识点：多个在途请求可能先后返回，必须用 qobject_cast 确认 sender 是本次的 reply，
    // 不能假设一定成功；无错误且寄存器数量完整才应用，否则丢弃由下个周期重试（快速失败、不阻塞界面）。
    auto *reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QVector<quint16> holding = reply->result().values();
        if (holding.size() >= MODBUS_REG_COUNT) {
            applyModbusData(holding);
            runControlLogic();   // 数据刷新后立即跑一遍控制/报警联动
        }
    }
    reply->deleteLater();   // QModbusReply 由信号触发、用 deleteLater 安全回收
}

void PumpBackend::onModbusStateChanged()
{
    // 知识点：QModbusDevice::state() 是连接状态机当前态，只有 ConnectedState 才算真正可读；
    // Connecting/Unconnected/Closing 一律视为未就绪，数据源回退内部模拟器兜底。
    const QModbusDevice::State st = m_modbus ? m_modbus->state() : QModbusDevice::UnconnectedState;
    const bool connected = (st == QModbusDevice::ConnectedState);

    if (connected != m_modbusConnected) {
        m_modbusConnected = connected;
        emit modbusConnectedChanged();
    }
    if (connected)
        m_reconnectTick = 0;   // 连接成功即清零重连计时，断链后再从 0 起算

    QString status;
    switch (st) {
    case QModbusDevice::ConnectedState:   status = QStringLiteral("已连接（Modbus 数据源）"); break;
    case QModbusDevice::ConnectingState:  status = QStringLiteral("连接中…");              break;
    case QModbusDevice::ClosingState:     status = QStringLiteral("断开中…");              break;
    case QModbusDevice::UnconnectedState:
    default:                             status = QStringLiteral("未连接（回退模拟器）");   break;
    }
    if (status != m_modbusStatus) {
        m_modbusStatus = status;
        emit modbusStatusChanged();
    }

    if (!connected)
        qWarning() << "[Modbus] 从站未连接，回退内部模拟器";
}

void PumpBackend::applyModbusData(const QVector<quint16> &holding)
{
    //  前 18 个模拟量：寄存器原始值 / 缩放因子 = 工程值（映射表见文件头注释） 
    // 知识点：PLC/Modbus 无浮点寄存器时常用定标整数传输，上位机按缩放因子还原真实量纲。
    APPLY(supplyTemp,         holding[0],  10.0);   // ℃ ×10
    APPLY(returnTemp,         holding[1],  10.0);
    APPLY(coolingTowerOutlet, holding[2],  10.0);
    APPLY(exchangerIn,        holding[3],  10.0);
    APPLY(exchangerOut,       holding[4],  10.0);
    APPLY(supplyPressure,     holding[5],  1000.0); // MPa ×1000
    APPLY(returnPressure,     holding[6],  1000.0);
    APPLY(makeupPressure,     holding[7],  1000.0);
    APPLY(supplyFlow,         holding[8],  10.0);   // m³/h ×10
    APPLY(conductivity,       holding[9],  10.0);   // μS/cm ×10
    APPLY(tankLevel,          holding[10], 1000.0); // m ×1000
    APPLY(expansionLevel,     holding[11], 1000.0);
    APPLY(cyclePump1Current,  holding[12], 10.0);   // A ×10
    APPLY(cyclePump2Current,  holding[13], 10.0);
    APPLY(makeupPump1Current, holding[14], 10.0);
    APPLY(makeupPump2Current, holding[15], 10.0);
    APPLY(wetBulbTemp,        holding[16], 10.0);   // ℃ ×10
    APPLY(dailyPower,         holding[17], 10.0);   // kWh ×10

    //  后 10 个开关量（0=停/关，非 0=运行/开）：同步到 DeviceModel，进而刷新状态灯 
    // 知识点：先读当前态、仅在翻转时调用启停，避免每秒写重复的设备事件表记录。
    auto setSwitch = [this](const char *key, bool on) {
        const QString k = QString::fromLatin1(key);
        // 手动模式（manual=true）：状态由用户在控制页手动启停，Modbus 寄存器不覆盖，
        // 否则点“停止/启动”后下一轮轮询又被寄存器值改回，表现为“点按钮又回弹”。
        if (m_devices->manualByKey(k))
            return;
        const bool cur = m_devices->runningByKey(k);
        if (cur != on) {
            if (on) m_devices->startByKey(k);
            else    m_devices->stopByKey(k);
        }
    };
    setSwitch("cyclePump1", holding[18] != 0);
    setSwitch("cyclePump2", holding[19] != 0);
    setSwitch("cyclePump3", holding[20] != 0);
    setSwitch("coolingFan1", holding[21] != 0);
    setSwitch("coolingFan2", holding[22] != 0);
    setSwitch("makeupPump1", holding[23] != 0);
    setSwitch("makeupPump2", holding[24] != 0);
    setSwitch("sprayPump1",  holding[25] != 0);
    setSwitch("makeupValve", holding[26] != 0);
    setSwitch("drainValve",  holding[27] != 0);
}

void PumpBackend::setModbusEnabled(bool v)
{
    // 知识点：运行时切换数据源——启用则发起连接，禁用则断开并清连接态，
    // 数据源立即回退内部模拟器，界面无需重启。
    if (v == m_modbusEnabled)
        return;
    m_modbusEnabled = v;
    emit modbusEnabledChanged();

    if (m_modbusEnabled) {
        if (m_modbus && m_modbus->state() != QModbusDevice::ConnectedState)
            m_modbus->connectDevice();
    } else if (m_modbus) {
        m_modbus->disconnectDevice();
    }
}

void PumpBackend::setModbusHost(const QString &v)
{
    // 知识点：实参 setter 只改成员并发 NOTIFY；IP/端口在下次 connectDevice 时生效
    // （连接参数在构造函数与这里统一写入客户端，运行中改主机/端口需重连才接管）。
    if (v == m_modbusHost)
        return;
    m_modbusHost = v;
    if (m_modbus)
        m_modbus->setConnectionParameter(QModbusDevice::NetworkAddressParameter, v);
    emit modbusHostChanged();
}

void PumpBackend::setModbusPort(int v)
{
    if (v == m_modbusPort)
        return;
    m_modbusPort = v;
    if (m_modbus)
        m_modbus->setConnectionParameter(QModbusDevice::NetworkPortParameter, v);
    emit modbusPortChanged();
}

#undef APPLY
#undef UPDATE
#undef SYNC_BOOL