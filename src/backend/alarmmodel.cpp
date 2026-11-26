// AlarmModel.cpp
// 知识点：
//   新增报警用 beginInsertRows/endInsertRows 通知 QML哪几行插入，
//     而非简单 append，这样 ListView 才能正确地做插入动画/滚动定位。
#include "alarmmodel.h"
#include <QDateTime>

AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_alarms = {
        //列表初始化，硬编码
        //QString()运行时动态分配，QStringLiteral编译时静态分配，开销比较小
        { 1, QStringLiteral("10:09:58"), QStringLiteral("供水压力过低(<0.1MPa)"), QStringLiteral("已确认") },
        { 2, QStringLiteral("09:42:15"), QStringLiteral("电导率超限(>800μS/cm)"),   QStringLiteral("未确认") },
        { 2, QStringLiteral("08:30:02"), QStringLiteral("冷却塔风机 #2 故障"),      QStringLiteral("已消除") },
        { 3, QStringLiteral("07:55:40"), QStringLiteral("膨胀罐液位偏低"),          QStringLiteral("已确认") },
        { 3, QStringLiteral("06:12:33"), QStringLiteral("补水频率异常"),            QStringLiteral("已确认") },
    };
}

int AlarmModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_alarms.size();
}

QVariant AlarmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_alarms.size())
        return {};
    const Alarm &a = m_alarms.at(index.row());
    switch (role) {
    case LevelRole:
        return a.level;
    case TimeRole:
        return a.time;
    case TypeRole:
        return a.type;
    case StateRole:
        return a.state;
    default:
        return {};
    }
}

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    return {
        { LevelRole, "level" },
        { TimeRole,  "time"  },
        { TypeRole,  "type"  },
        // 知识点：角色名不能用 “state”，它会和 QML 每个 Item 自带的 state
        // 属性冲突（内置属性优先、角色值取不到），故改名 alarmState。
        { StateRole, "alarmState" },
    };
}

void AlarmModel::acknowledge(int row)
{
    if (row < 0 || row >= m_alarms.size())
        return;
    if (m_alarms[row].state == QStringLiteral("未确认")) {
        m_alarms[row].state = QStringLiteral("已确认");
        emit dataChanged(index(row, 0), index(row, 0), { StateRole });
        emit unackedChanged();
        emit stateChanged(m_alarms[row].level, m_alarms[row].type, m_alarms[row].state);
    }
}

void AlarmModel::reset(int row)
{
    if (row < 0 || row >= m_alarms.size())
        return;
    if (m_alarms[row].state == QStringLiteral("已确认")) {
        m_alarms[row].state = QStringLiteral("已消除");
        emit dataChanged(index(row, 0), index(row, 0), { StateRole });
        emit stateChanged(m_alarms[row].level, m_alarms[row].type, m_alarms[row].state);
    }
}

int AlarmModel::unackedCount() const
{
    int n = 0;
    for (const Alarm &a : m_alarms)
        if (a.state == QStringLiteral("未确认"))
            ++n;
    return n;
}

void AlarmModel::addAlarm(int level, const QString &type)
{
    // 先声明将在第 0 行插入 1 行，再真正写入，再结束插入
    beginInsertRows(QModelIndex(), 0, 0);
    m_alarms.prepend({ level,
                       QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss")),
                       type,
                       QStringLiteral("未确认") });
    endInsertRows();
    emit unackedChanged();
    emit stateChanged(level, type, QStringLiteral("未确认"));
}