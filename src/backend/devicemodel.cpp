// DeviceModel.cpp
// 知识点：
//   构造函数里预置多台设备（循环水泵/
//     冷却塔风机/喷淋泵/补水泵/电磁阀），并按 group 分组，供泵组切换。
//   分组过滤实现：m_devices 始终存全部设备，m_visible 存当前
//     过滤条件下可见设备的下标；rowCount/data 都基于 m_visible。
//     切换 group 用 begin/endResetModel 通知视图整体重建。
//   emit dataChanged(index, index, roles) 用固定行区间通知更新，
//     roles 精确到哪些角色变了，避免无谓的全量刷新。
#include "devicemodel.h"

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // 预置设备：循环水泵组 3 台 N+1 备用 / 冷却塔设备 / 补水泵组 / 阀门
    m_devices = {
        { QStringLiteral("循环水泵 #1"), QStringLiteral("cyclePump1"), QStringLiteral("循环水泵组"), true,  true  },
        { QStringLiteral("循环水泵 #2"), QStringLiteral("cyclePump2"), QStringLiteral("循环水泵组"), false, false },
        { QStringLiteral("循环水泵 #3"), QStringLiteral("cyclePump3"), QStringLiteral("循环水泵组"), false, false },
        { QStringLiteral("冷却塔风机 #1"), QStringLiteral("coolingFan1"), QStringLiteral("冷却塔设备"), true,  false },
        { QStringLiteral("冷却塔风机 #2"), QStringLiteral("coolingFan2"), QStringLiteral("冷却塔设备"), false, false },
        { QStringLiteral("喷淋泵 #1"),   QStringLiteral("sprayPump1"),   QStringLiteral("冷却塔设备"), false, false },
        { QStringLiteral("补水泵 #1"),   QStringLiteral("makeupPump1"), QStringLiteral("补水泵组"), true,  true  },
        { QStringLiteral("补水泵 #2"),   QStringLiteral("makeupPump2"), QStringLiteral("补水泵组"), false, true  },
        { QStringLiteral("补水电磁阀"),  QStringLiteral("makeupValve"), QStringLiteral("阀门"), true,  false },
        { QStringLiteral("排污电磁阀"),  QStringLiteral("drainValve"),  QStringLiteral("阀门"), false, false },
    };
    rebuildIndex();   // 初始全部过滤
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_visible.size();   // 只算过滤后可见设备
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_visible.size())
        return {};

    const Device &d = m_devices.at(m_visible.at(index.row()));
    switch (role) {
    case NameRole:
        return d.name;
    case KeyRole:
        return d.key;
    case GroupRole:
        return d.group;
    case RunningRole:
        return d.running;
    case ManualRole:
        return d.manual;
    default:
        return {};
    }
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return {
        { NameRole,    "name"    },
        { KeyRole,     "key"     },
        { GroupRole,   "group"   },
        { RunningRole, "running" },
        { ManualRole,  "manual"  },
    };
}

int DeviceModel::indexOfKey(const QString &key) const
{
    for (int i = 0; i < m_devices.size(); ++i)
        if (m_devices.at(i).key == key)
            return i;
    return -1;
}

int DeviceModel::toActual(int row) const
{
    return (row >= 0 && row < m_visible.size()) ? m_visible.at(row) : -1;
}

void DeviceModel::rebuildIndex()
{
    m_visible.clear();
    for (int i = 0; i < m_devices.size(); ++i)
        if (m_filter.isEmpty() || m_devices.at(i).group == m_filter)
            m_visible.append(i);
}

QStringList DeviceModel::groups() const
{
    // 按插入顺序去重，得到各泵组名称（供 QML 生成过滤按钮）
    QStringList g;
    for (const Device &d : m_devices)
        if (!g.contains(d.group))
            g << d.group;
    return g;
}

QString DeviceModel::filter() const { return m_filter; }

void DeviceModel::setFilter(const QString &group)
{
    if (m_filter == group)
        return;
    m_filter = group;
    // 知识点：过滤导致整张表变化，用 resetModel 通知视图重建
    beginResetModel();
    rebuildIndex();
    endResetModel();
    emit filterChanged();
    emit countChanged();
}

int DeviceModel::count() const { return m_visible.size(); }

void DeviceModel::toggle(int row)
{
    const int a = toActual(row);
    if (a < 0)
        return;
    m_devices[a].running = !m_devices[a].running;
    emit dataChanged(index(row, 0), index(row, 0), { RunningRole });
}

void DeviceModel::start(int row)
{
    const int a = toActual(row);
    if (a < 0 || m_devices[a].running)
        return;
    m_devices[a].running = true;
    emit dataChanged(index(row, 0), index(row, 0), { RunningRole });
}

void DeviceModel::stop(int row)
{
    const int a = toActual(row);
    if (a < 0 || !m_devices[a].running)
        return;
    m_devices[a].running = false;
    emit dataChanged(index(row, 0), index(row, 0), { RunningRole });
}

void DeviceModel::setManual(int row, bool on)
{
    const int a = toActual(row);
    if (a < 0)
        return;
    m_devices[a].manual = on;
    emit dataChanged(index(row, 0), index(row, 0), { ManualRole });
}

bool DeviceModel::runningAt(int row) const
{
    const int a = toActual(row);
    return a >= 0 && m_devices.at(a).running;
}

bool DeviceModel::runningByKey(const QString &key) const
{
    const int i = indexOfKey(key);
    return i >= 0 && m_devices.at(i).running;
}

bool DeviceModel::manualByKey(const QString &key) const
{
    const int i = indexOfKey(key);
    return i >= 0 && m_devices.at(i).manual;
}

void DeviceModel::startByKey(const QString &key)
{
    const int a = indexOfKey(key);
    if (a < 0 || m_devices[a].running)
        return;
    m_devices[a].running = true;
    // 若该设备当前可见，通知对应可见行的数据变化
    const int v = m_visible.indexOf(a);
    if (v >= 0)
        emit dataChanged(index(v, 0), index(v, 0), { RunningRole });
}

void DeviceModel::stopByKey(const QString &key)
{
    const int a = indexOfKey(key);
    if (a < 0 || !m_devices[a].running)
        return;
    m_devices[a].running = false;
    const int v = m_visible.indexOf(a);
    if (v >= 0)
        emit dataChanged(index(v, 0), index(v, 0), { RunningRole });
}