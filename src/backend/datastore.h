// DataStore.h —— SQLite 数据存储（单泵站使用 SQLite）
// 知识点 / 编写原因：
//   - 小型系统（单泵站）使用 SQLite 或 MySQL 即可，
//     SQLite 是嵌入式文件数据库、零部署，Navicat 打开 .db 文件即可查询，
//     满足导出的内容用数据库存储、能在 Navicat 里查的要求。
//   - 把写库逻辑从 PumpBackend 剥离成独立类：后端只负责定时/事件调用
//     logXxx()，SQL 细节集中在 DataStore，职责清晰、便于替换 MySQL。
#ifndef DATASTORE_H
#define DATASTORE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

class DataStore : public QObject
{
    Q_OBJECT
public:
    explicit DataStore(QObject *parent = nullptr);

    // 打开（或创建）数据库与四张表；失败返回 false（调用方打日志）
    bool open();

    // 数据库文件绝对路径（供 Navicat 打开 / UI 提示）
    QString dbPath() const { return m_dbPath; }

    // 写入一条实时采样（每秒由遥测模拟器调用）
    void logRealtime(double supplyTemp, double supplyPressure, double conductivity,
                     double returnTemp, double towerOutlet, double supplyFlow,
                     double tankLevel, double cumulativeFlow, double cop, double dailyPower);

    // 写入一条设备启停事件
    void logDeviceEvent(const QString &key, const QString &event);

    // 写入一条报警事件（新增/已确认/已消除）
    void logAlarm(int level, const QString &type, const QString &state);

    // 保存参数配方（每次保存参数写一组 name=value 记录）
    void saveRecipe(const QStringList &names, const QVector<double> &values);

private:
    void exec(const QString &sql);   // 执行无参数 SQL（建表等）
    QString m_dbPath;
};

#endif // DATASTORE_H