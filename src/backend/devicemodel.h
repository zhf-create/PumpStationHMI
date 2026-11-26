// DeviceModel.h —— 设备列表模型（对应设备控制画面③）
// 知识点：
//   - QAbstractListModel：Qt 的模型/视图架构里，模型只负责数据，
//     视图（QML 的 ListView/Repeater）负责展示。二者通过 role 解耦。
//   - roleNames()：把 C++ 索引角色映射成 QML 侧可读的字符串名。
//   - group 分组 + filter 过滤：多泵站/多泵协同与
//     循环水泵组/补水泵组要求能按泵组切换查看，这里用分组过滤
//     实现：QML 点不同泵组按钮 → setFilter → 视图只显示该组设备，
//     实现切换不同水泵交互（知识点：模型侧过滤而非 QML 里硬塞）。
#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QHash>
#include <QStringList>

class DeviceModel : public QAbstractListModel
{
    Q_OBJECT

    // 分组名列表（只读，供 QML 生成泵组切换按钮）
    Q_PROPERTY(QStringList groups READ groups CONSTANT)
    // 当前分组过滤（空 = 全部设备），供 QML 切换泵组
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    // 可见设备数（随 filter 变化，供 QML 显示共 N 台）
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    // 角色枚举：从 UserRole+1 起，避免与 Qt 内置角色冲突，UserRole是256起点数字
    //用枚举是表面写的是变量名，但是后台传输的还是数字，比如说NameRole->>257，因为
    //QT控件只认int
    enum Role {
        NameRole = Qt::UserRole + 1,   // 设备名
        KeyRole,                       // 稳定键（供 C++/QML 按名查找）
        GroupRole,                     // 所属分组（泵组过滤用）
        RunningRole,                   // 是否运行
        ManualRole                     // 是否手动模式
    };

    struct Device {
        QString name;
        QString key;
        QString group;
        bool running = false;
        bool manual = false;
    };

    explicit DeviceModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    //  供 QML 调用的控制接口（Q_INVOKABLE 才能被 QML 直接调用） 
    // 注意：下面的 row 都是过滤后可见行号，内部会映射回真实下标。
    Q_INVOKABLE void toggle(int row);            // 启停切换
    Q_INVOKABLE void start(int row);             // 启动
    Q_INVOKABLE void stop(int row);              // 停止
    Q_INVOKABLE void setManual(int row, bool on); // 手动/自动
    Q_INVOKABLE bool runningAt(int row) const;   // 按可见行号查运行态
    Q_INVOKABLE bool runningByKey(const QString &key) const; // 按键查运行态
    bool manualByKey(const QString &key) const; // 按键查手动模式（供后端自动/Modbus 逻辑判断是否可覆盖）

    // 按稳定键操作真实列表（供后端报警联动调用，不受分组过滤影响）
    void startByKey(const QString &key);
    void stopByKey(const QString &key);

    QStringList groups() const;
    QString filter() const;
    void setFilter(const QString &group);
    int count() const;

signals:
    void filterChanged();
    void countChanged();

private:
    QVector<Device> m_devices;     // 全部设备（真实数据，含所有泵组）
    QVector<int> m_visible;        // 当前过滤下可见设备的真实下标
    QString m_filter;              // 空 = 全部
    int indexOfKey(const QString &key) const;
    int toActual(int row) const;   // 可见行号 -> 真实下标
    void rebuildIndex();           // 依 m_filter 重建可见下标表
};

#endif // DEVICEMODEL_H