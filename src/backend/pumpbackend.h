// PumpBackend.h —— 上位机后端总控（单例，QML 中以 Backend 引用）
// 设计思想（知识点）：
//   1) 前端只做展示、后端只做数据：QML 页面通过属性绑定读取
//      Backend.supplyTemp、Backend.devices 等，不再写死数值。
//   2) 每个 Q_PROPERTY 都带 NOTIFY 信号，QML 绑定会值一变就自动刷新，
//      这就是 Qt 元对象系统实现响应式 UI 的关键。
//   3) 遥测数据由内部 QTimer 用抖动(jitter)模拟，真实项目把这部分
//      换成 Modbus TCP 轮询 PLC 寄存器即可（接口不变）。
#ifndef PUMPBACKEND_H
#define PUMPBACKEND_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QAbstractItemModel>
#include <QHash>
#include <QVector>

class DeviceModel;
class ReportModel;
class AlarmModel;
class DataStore;
class QModbusTcpClient;
class QModbusReply;

class PumpBackend : public QObject
{
    Q_OBJECT

    //  遥测模拟量（只读，供 ① ② 页展示） 
    Q_PROPERTY(double supplyTemp        READ supplyTemp        NOTIFY supplyTempChanged)
    Q_PROPERTY(double returnTemp        READ returnTemp        NOTIFY returnTempChanged)
    Q_PROPERTY(double coolingTowerOutlet READ coolingTowerOutlet NOTIFY coolingTowerOutletChanged)
    Q_PROPERTY(double exchangerIn       READ exchangerIn       NOTIFY exchangerInChanged)
    Q_PROPERTY(double exchangerOut      READ exchangerOut      NOTIFY exchangerOutChanged)
    Q_PROPERTY(double supplyPressure    READ supplyPressure    NOTIFY supplyPressureChanged)
    Q_PROPERTY(double returnPressure    READ returnPressure    NOTIFY returnPressureChanged)
    Q_PROPERTY(double makeupPressure    READ makeupPressure    NOTIFY makeupPressureChanged)
    Q_PROPERTY(double supplyFlow        READ supplyFlow        NOTIFY supplyFlowChanged)
    Q_PROPERTY(double cumulativeFlow    READ cumulativeFlow    NOTIFY cumulativeFlowChanged)
    Q_PROPERTY(double conductivity      READ conductivity      NOTIFY conductivityChanged)
    Q_PROPERTY(double tankLevel         READ tankLevel         NOTIFY tankLevelChanged)
    Q_PROPERTY(double expansionLevel    READ expansionLevel    NOTIFY expansionLevelChanged)
    Q_PROPERTY(double valvePos1         READ valvePos1         NOTIFY valvePos1Changed)
    Q_PROPERTY(double valvePos2         READ valvePos2         NOTIFY valvePos2Changed)
    // 变频调速结果（冷却塔风机变频调速 / 供水泵变频恒压）：比例调节后的频率
    Q_PROPERTY(double supplyPumpFreq   READ supplyPumpFreq   NOTIFY supplyPumpFreqChanged)
    Q_PROPERTY(double coolingFanFreq   READ coolingFanFreq   NOTIFY coolingFanFreqChanged)
    Q_PROPERTY(double cyclePump1Current READ cyclePump1Current NOTIFY cyclePump1CurrentChanged)
    Q_PROPERTY(double cyclePump2Current READ cyclePump2Current NOTIFY cyclePump2CurrentChanged)
    Q_PROPERTY(double makeupPump1Current READ makeupPump1Current NOTIFY makeupPump1CurrentChanged)
    Q_PROPERTY(double makeupPump2Current READ makeupPump2Current NOTIFY makeupPump2CurrentChanged)
    Q_PROPERTY(bool   conductivityAlarm READ conductivityAlarm NOTIFY conductivityAlarmChanged)
    // 能效 / 能耗 KPI（能效优化、累计耗电/运行时间、COP）
    Q_PROPERTY(double cop          READ cop          NOTIFY copChanged)          // 系统性能系数 COP
    Q_PROPERTY(double wetBulbTemp  READ wetBulbTemp  NOTIFY wetBulbTempChanged)  // 环境湿球温度（℃）
    Q_PROPERTY(double dailyPower   READ dailyPower   NOTIFY dailyPowerChanged)   // 当日累计耗电量（kWh）
    Q_PROPERTY(double pumpRunHours READ pumpRunHours NOTIFY pumpRunHoursChanged) // 水泵累计运行时间（h）

    //  设备运行态（由 DeviceModel 同步，供 ① 页状态灯） 
    Q_PROPERTY(bool cyclePump1Running READ cyclePump1Running NOTIFY cyclePump1RunningChanged)
    Q_PROPERTY(bool cyclePump2Running READ cyclePump2Running NOTIFY cyclePump2RunningChanged)
    Q_PROPERTY(bool cyclePump3Running READ cyclePump3Running NOTIFY cyclePump3RunningChanged)
    Q_PROPERTY(bool coolingFan1Running READ coolingFan1Running NOTIFY coolingFan1RunningChanged)
    Q_PROPERTY(bool coolingFan2Running READ coolingFan2Running NOTIFY coolingFan2RunningChanged)
    Q_PROPERTY(bool makeupPump1Running READ makeupPump1Running NOTIFY makeupPump1RunningChanged)
    Q_PROPERTY(bool makeupPump2Running READ makeupPump2Running NOTIFY makeupPump2RunningChanged)
    Q_PROPERTY(bool sprayPump1Running  READ sprayPump1Running  NOTIFY sprayPump1RunningChanged)
    Q_PROPERTY(bool makeupValveOpen    READ makeupValveOpen    NOTIFY makeupValveOpenChanged)
    Q_PROPERTY(bool drainValveOpen     READ drainValveOpen     NOTIFY drainValveOpenChanged)

    //  自控参数（可读写，供 ④ 页设定） 
    Q_PROPERTY(double tempSetpoint   READ tempSetpoint   WRITE setTempSetpoint   NOTIFY tempSetpointChanged)
    Q_PROPERTY(double pressSetpoint  READ pressSetpoint  WRITE setPressSetpoint  NOTIFY pressSetpointChanged)
    Q_PROPERTY(double coolingSetpoint READ coolingSetpoint WRITE setCoolingSetpoint NOTIFY coolingSetpointChanged)
    Q_PROPERTY(double tempProportional  READ tempProportional  WRITE setTempProportional  NOTIFY tempProportionalChanged)
    Q_PROPERTY(double tempIntegral      READ tempIntegral      WRITE setTempIntegral      NOTIFY tempIntegralChanged)
    Q_PROPERTY(double pressProportional READ pressProportional WRITE setPressProportional NOTIFY pressProportionalChanged)
    Q_PROPERTY(double pressIntegral     READ pressIntegral     WRITE setPressIntegral     NOTIFY pressIntegralChanged)
    Q_PROPERTY(double tempAlarmL1    READ tempAlarmL1    WRITE setTempAlarmL1    NOTIFY tempAlarmL1Changed)
    Q_PROPERTY(double tempAlarmL2    READ tempAlarmL2    WRITE setTempAlarmL2    NOTIFY tempAlarmL2Changed)
    Q_PROPERTY(double pressLowAlarm  READ pressLowAlarm  WRITE setPressLowAlarm  NOTIFY pressLowAlarmChanged)
    Q_PROPERTY(double conductivityLimit READ conductivityLimit WRITE setConductivityLimit NOTIFY conductivityLimitChanged)

    //  子模型（供 ③ ⑤ ⑥ 页 ListView 作 model） 
    // 知识点：用基类指针 QAbstractItemModel* 暴露，QML 无需注册具体子类型，
    // 直接 `model: Backend.devices` 即可，且避免指针型 Q_PROPERTY 的元类型问题。
    Q_PROPERTY(QAbstractItemModel* devices READ devices CONSTANT)
    Q_PROPERTY(QAbstractItemModel* reports READ reports CONSTANT)
    Q_PROPERTY(QAbstractItemModel* alarms  READ alarms  CONSTANT)

    // 保存结果反馈（供 ④ 页提示）
    Q_PROPERTY(QString saveMessage READ saveMessage NOTIFY saveMessageChanged)

    //  登录鉴权（参数修改 / 设备操作需登录，管理员才可改参数） 
    // 知识点：loggedIn/currentUser 只读，登录状态集中在后端管理；
    // QML 只能调用 login()/logout()，写操作入口按 loggedIn 决定是否放行。
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString currentUser READ currentUser NOTIFY currentUserChanged)
    Q_PROPERTY(QString loginMessage READ loginMessage NOTIFY loginMessageChanged)

    // 数据库文件绝对路径（供 Navicat 打开 / 界面提示）
    Q_PROPERTY(QString dbPath READ dbPath CONSTANT)

    //  Modbus TCP 数据源（HMI 作为客户端读取 Modbus 从站/PLC 寄存器） 
    // 知识点：modbusEnabled 默认 true；连接失败时自动回退内部模拟器，保证无 Modbus 环境也能演示。
    Q_PROPERTY(bool   modbusEnabled   READ modbusEnabled   WRITE setModbusEnabled   NOTIFY modbusEnabledChanged)
    Q_PROPERTY(bool   modbusConnected READ modbusConnected NOTIFY modbusConnectedChanged)
    Q_PROPERTY(QString modbusStatus   READ modbusStatus    NOTIFY modbusStatusChanged)
    Q_PROPERTY(QString modbusHost     READ modbusHost      WRITE setModbusHost      NOTIFY modbusHostChanged)
    Q_PROPERTY(int    modbusPort      READ modbusPort       WRITE setModbusPort       NOTIFY modbusPortChanged)

public:
    explicit PumpBackend(QObject *parent = nullptr);

    // 子模型访问器
    QAbstractItemModel* devices() const;
    QAbstractItemModel* reports() const;
    QAbstractItemModel* alarms()  const;

    // 慢启动：在对象创建后由 main() 调用，开始模拟数据
    Q_INVOKABLE void start();

    // 保存参数（保存为配方）
    Q_INVOKABLE void saveParams();

    // 模拟水泵故障（过载/缺相/变频器故障→自动停机并切换备用泵），供界面演示触发
    Q_INVOKABLE void simulatePumpFault(const QString &key);

    // 登录 / 退出（账号校验在后端，QML 只传用户名密码）
    Q_INVOKABLE bool login(const QString &user, const QString &password);
    Q_INVOKABLE void logout();

    //  遥测 getter 
    double supplyTemp() const {
        return m_supplyTemp;
    }
    double returnTemp() const {
        return m_returnTemp;
    }
    double coolingTowerOutlet() const {
        return m_coolingTowerOutlet;
    }
    double exchangerIn() const {
        return m_exchangerIn;
    }
    double exchangerOut() const {
        return m_exchangerOut;
    }
    double supplyPressure() const {
        return m_supplyPressure;
    }
    double returnPressure() const {
        return m_returnPressure;
    }
    double makeupPressure() const {
        return m_makeupPressure;
    }
    double supplyFlow() const {
        return m_supplyFlow;
    }
    double cumulativeFlow() const {
        return m_cumulativeFlow;
    }
    double conductivity() const {
        return m_conductivity;
    }
    double tankLevel() const {
        return m_tankLevel;
    }
    double expansionLevel() const {
        return m_expansionLevel;
    }
    double valvePos1() const {
        return m_valvePos1;
    }
    double valvePos2() const {
        return m_valvePos2;
    }
    double supplyPumpFreq() const {
        return m_supplyPumpFreq;
    }
    double coolingFanFreq() const {
        return m_coolingFanFreq;
    }
    double cyclePump1Current() const {
        return m_cyclePump1Current;
    }
    double cyclePump2Current() const {
        return m_cyclePump2Current;
    }
    double makeupPump1Current() const {
        return m_makeupPump1Current;
    }
    double makeupPump2Current() const {
        return m_makeupPump2Current;
    }
    bool conductivityAlarm() const {
        return m_conductivityAlarm;
    }
    double cop() const {
        return m_cop;
    }
    double wetBulbTemp() const {
        return m_wetBulbTemp;
    }
    double dailyPower() const {
        return m_dailyPower;
    }
    double pumpRunHours() const {
        return m_pumpRunHours;
    }

    //  设备运行态 getter 
    bool cyclePump1Running() const { return m_cyclePump1Running; }
    bool cyclePump2Running() const { return m_cyclePump2Running; }
    bool cyclePump3Running() const { return m_cyclePump3Running; }
    bool coolingFan1Running() const { return m_coolingFan1Running; }
    bool coolingFan2Running() const { return m_coolingFan2Running; }
    bool makeupPump1Running() const { return m_makeupPump1Running; }
    bool makeupPump2Running() const { return m_makeupPump2Running; }
    bool sprayPump1Running() const { return m_sprayPump1Running; }
    bool makeupValveOpen() const { return m_makeupValveOpen; }
    bool drainValveOpen() const { return m_drainValveOpen; }

    //  参数 getter/setter 
    double tempSetpoint() const { return m_tempSetpoint; }
    double pressSetpoint() const { return m_pressSetpoint; }
    double coolingSetpoint() const { return m_coolingSetpoint; }
    double tempProportional() const { return m_tempProportional; }
    double tempIntegral() const { return m_tempIntegral; }
    double pressProportional() const { return m_pressProportional; }
    double pressIntegral() const { return m_pressIntegral; }
    double tempAlarmL1() const { return m_tempAlarmL1; }
    double tempAlarmL2() const { return m_tempAlarmL2; }
    double pressLowAlarm() const { return m_pressLowAlarm; }
    double conductivityLimit() const { return m_conductivityLimit; }

    void setTempSetpoint(double v);
    void setPressSetpoint(double v);
    void setCoolingSetpoint(double v);
    void setTempProportional(double v);
    void setTempIntegral(double v);
    void setPressProportional(double v);
    void setPressIntegral(double v);
    void setTempAlarmL1(double v);
    void setTempAlarmL2(double v);
    void setPressLowAlarm(double v);
    void setConductivityLimit(double v);

    QString saveMessage() const { return m_saveMessage; }
    QString dbPath() const;
    bool loggedIn() const { return m_loggedIn; }
    QString currentUser() const { return m_currentUser; }
    QString loginMessage() const { return m_loginMessage; }

    //  Modbus 访问器 
    bool modbusEnabled() const { return m_modbusEnabled; }
    bool modbusConnected() const { return m_modbusConnected; }
    QString modbusStatus() const { return m_modbusStatus; }
    QString modbusHost() const { return m_modbusHost; }
    int modbusPort() const { return m_modbusPort; }
    void setModbusEnabled(bool v);
    void setModbusHost(const QString &v);
    void setModbusPort(int v);

signals:
    // 知识点：Q_PROPERTY 里的 NOTIFY 必须对应类内真正声明的信号，
    // 否则 moc 编译时报NOTIFY signal does not exist。这里把所有
    // NOTIFY 信号集中声明（无参，QML 绑定值变即刷新，参数由 getter 读取）。

    //  遥测模拟量 
    void supplyTempChanged();
    void returnTempChanged();
    void coolingTowerOutletChanged();
    void exchangerInChanged();
    void exchangerOutChanged();
    void supplyPressureChanged();
    void returnPressureChanged();
    void makeupPressureChanged();
    void supplyFlowChanged();
    void cumulativeFlowChanged();
    void conductivityChanged();
    void tankLevelChanged();
    void expansionLevelChanged();
    void valvePos1Changed();
    void valvePos2Changed();
    void supplyPumpFreqChanged();
    void coolingFanFreqChanged();
    void cyclePump1CurrentChanged();
    void cyclePump2CurrentChanged();
    void makeupPump1CurrentChanged();
    void makeupPump2CurrentChanged();
    void conductivityAlarmChanged();
    void copChanged();
    void wetBulbTempChanged();
    void dailyPowerChanged();
    void pumpRunHoursChanged();

    //  设备运行态 
    void cyclePump1RunningChanged();
    void cyclePump2RunningChanged();
    void cyclePump3RunningChanged();
    void coolingFan1RunningChanged();
    void coolingFan2RunningChanged();
    void makeupPump1RunningChanged();
    void makeupPump2RunningChanged();
    void sprayPump1RunningChanged();
    void makeupValveOpenChanged();
    void drainValveOpenChanged();

    //  自控参数 
    void tempSetpointChanged();
    void pressSetpointChanged();
    void coolingSetpointChanged();
    void tempProportionalChanged();
    void tempIntegralChanged();
    void pressProportionalChanged();
    void pressIntegralChanged();
    void tempAlarmL1Changed();
    void tempAlarmL2Changed();
    void pressLowAlarmChanged();
    void conductivityLimitChanged();

    //  保存结果 
    void saveMessageChanged();

    //  登录鉴权 
    void loggedInChanged();
    void currentUserChanged();
    void loginMessageChanged();

    //  Modbus 
    void modbusEnabledChanged();
    void modbusConnectedChanged();
    void modbusStatusChanged();
    void modbusHostChanged();
    void modbusPortChanged();

private slots:
    void onTick();                          // 数据源心跳（模拟器或 Modbus）
    void syncDeviceStates();                // 从 DeviceModel 同步运行态到布尔属性
    void onModbusStateChanged();            // Modbus 连接状态变化（无参，内部读 m_modbus->state()）
    void onModbusReadFinished();            // Modbus 读取完成（无参，内部 qobject_cast sender()）

private:
    double jitter(double base, double amp) const;  // 在基准值附近抖动
    void logDeviceTransitions();                   // 设备启停变化 → 写入 DB

    //  Modbus 数据源 
    void pollModbus();                                  // 发起一次保持寄存器读取
    void applyModbusData(const QVector<quint16> &holding); // 把寄存器值映射到遥测/设备状态
    void updateTelemetrySimulated();                    // 模拟器模式：更新遥测模拟量
    void runControlLogic();                             // 共享控制逻辑（两种数据源都执行）

    //  遥测值 
    double m_supplyTemp = 32.07, m_returnTemp = 47.20, m_coolingTowerOutlet = 34.70;
    double m_exchangerIn = 31.68, m_exchangerOut = 31.61;
    double m_supplyPressure = 0.539, m_returnPressure = 0.412, m_makeupPressure = 0.200;
    double m_supplyFlow = 69.2, m_cumulativeFlow = 128.4;
    double m_conductivity = 596.2, m_tankLevel = 2.573, m_expansionLevel = 0.62;
    double m_valvePos1 = 40.9, m_valvePos2 = 0.0;
    double m_supplyPumpFreq = 45.0;   // 供水泵变频频率（Hz，图4 恒压供水 PID 输出）
    double m_coolingFanFreq = 38.0;   // 冷却塔风机频率（Hz，图4 风机变频 PID 输出）
    double m_cyclePump1Current = 40.4, m_cyclePump2Current = 0.0;
    double m_makeupPump1Current = 1.6, m_makeupPump2Current = 0.0;
    bool m_conductivityAlarm = false;
    double m_cop = 6.2;
    double m_wetBulbTemp = 26.4;
    double m_dailyPower = 860.5;
    double m_pumpRunHours = 12480.0;

    //  设备运行态（镜像 DeviceModel） 
    bool m_cyclePump1Running = true, m_cyclePump2Running = false, m_cyclePump3Running = false;
    bool m_coolingFan1Running = true, m_coolingFan2Running = false;
    bool m_makeupPump1Running = true, m_makeupPump2Running = false, m_sprayPump1Running = false;
    bool m_makeupValveOpen = true, m_drainValveOpen = false;

    // 报警联动上升沿标记（越限只报一次，恢复后清零可再次触发）
    bool m_tempL1Active = false, m_tempL2Active = false, m_pressLowActive = false;
    // 自动排污补水状态（电导率超标自动开排污阀+补水阀）
    bool m_autoDrain = false;

    //  电导率持续超限计时（>800μS/cm 持续 30 分钟才报警） 
    int m_conductivityOverSeconds = 0;   // 连续超限累计秒数

    //  膨胀罐/补水箱液位补水（滞回控制） 
    bool m_levelMakeup = false;          // 液位补水请求（低于下限置位，高于上限复位）
    bool m_makeupActive = false;         // 补水阀当前是否需打开
    double m_tankLevelLow = 2.00;        // 补水箱液位下限（m）
    double m_tankLevelHigh = 2.50;       // 补水箱液位上限（m）

    //  膨胀罐液位报警阈值（低=进气/气蚀，高=超压/气囊破裂） 
    double m_expansionLow = 0.40;        // 液位低报警（m）
    double m_expansionHigh = 0.80;       // 液位高报警（m）
    bool m_expansionLowActive = false;   // 液位低报警上升沿
    bool m_expansionHighActive = false;  // 液位高报警上升沿

    //  循环泵组轮换与故障模拟（多泵轮换、水泵故障切备用泵） 
    int m_rotationTick = 0;              // 轮换计时（秒）
    int m_activePumpIndex = 0;           // 当前主泵：0=#1 1=#2（#3 恒为 N+1 备用）
    int m_faultTick = 0;                 // 水泵故障演示计时（秒）

    //  自控参数 
    double m_tempSetpoint = 40.0, m_pressSetpoint = 0.40, m_coolingSetpoint = 34.5;
    double m_tempProportional = 60, m_tempIntegral = 120;
    double m_pressProportional = 80, m_pressIntegral = 10;
    double m_tempAlarmL1 = 45, m_tempAlarmL2 = 50;
    double m_pressLowAlarm = 0.10, m_conductivityLimit = 500;
    // 电导率报警上限（>800μS/cm 触发二级报警；500 仅为排污补水阈值）
    double m_conductivityAlarmLimit = 800.0;

    QString m_saveMessage;

    //  登录状态（默认未登录；提供本地管理员账号，见 login() 实现） 
    bool m_loggedIn = false;
    QString m_currentUser;
    QString m_loginMessage;

    //  数据源与模拟器 
    DeviceModel *m_devices = nullptr;
    ReportModel *m_reports = nullptr;
    AlarmModel  *m_alarms = nullptr;
    DataStore   *m_store = nullptr;   // SQLite 数据存储
    QTimer m_timer;

    //  Modbus TCP 客户端 
    QModbusTcpClient *m_modbus = nullptr;
    bool m_modbusEnabled = true;    // 是否启用 Modbus 数据源（默认启用，失败回退模拟器）
    bool m_modbusConnected = false; // 是否已连接从站
    QString m_modbusStatus = QStringLiteral("未连接");
    QString m_modbusHost = QStringLiteral("127.0.0.1"); // Modbus Slave 软件默认本机
    int m_modbusPort = 502;         // Modbus TCP 默认端口
    int m_reconnectTick = 0;        // 断链自动重连计时（秒），累计到 MODBUS_RECONNECT_INTERVAL 触发重连

    // 设备上次运行态（用于启停变化时写入事件）
    QHash<QString, bool> m_prevRunning;
};

#endif // PUMPBACKEND_H