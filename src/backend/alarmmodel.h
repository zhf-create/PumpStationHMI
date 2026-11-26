// AlarmModel.h —— 报警列表模型（对应报警信息查询⑥）
// 知识点：
//   - 报警有确认/消除等状态流转，Q_INVOKABLE acknowledge() 就是
//     状态机里的一个动作，触发 dataChanged 让 QML 即时变色。
//   - 级别(level)是 int，方便 QML 按 1/2/3 映射颜色（红/橙/黄）。
#ifndef ALARMMODEL_H
#define ALARMMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QHash>

class AlarmModel : public QAbstractListModel
{
    Q_OBJECT

    // 未确认报警条数（供报警页顶部提示还剩 N 条未处理）
    Q_PROPERTY(int unackedCount READ unackedCount NOTIFY unackedChanged)

public:
    enum Role {
        LevelRole = Qt::UserRole + 1,  // 级别：1 一级 / 2 二级 / 3 三级
        TimeRole,                      // 发生时间
        TypeRole,                      // 报警类型
        StateRole                      // 状态：已确认/未确认/已消除
    };

    explicit AlarmModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    //QVariant这个函数需要返回数据，但返回的数据类型不是固定的，或者是多种多样的。
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    //Q_INVOKABLE是允许一个普通的 C++ 成员函数，能够被 QML 或元对象系统（如 QMetaObject::invokeMethod）识别并调用
    Q_INVOKABLE void acknowledge(int row);     // 确认某条报警（未确认→已确认）
    Q_INVOKABLE void reset(int row);           // 复位某条报警（已确认→已消除）
    Q_INVOKABLE void addAlarm(int level, const QString &type); // 新增一条（供遥测触发）

    int unackedCount() const;

signals:
    void unackedChanged();
    // 报警状态流转（新增/确认/消除），供后端写库（报警事件记录）
    void stateChanged(int level, const QString &type, const QString &state);

private:
    struct Alarm {
        int level = 3;
        QString time;
        QString type;
        QString state;
    };
    QVector<Alarm> m_alarms;
};

#endif // ALARMMODEL_H