// DataStore.cpp
// 知识点：
//   QSqlDatabase::addDatabase("QSQLITE") 注册默认连接；SQLite 单文件，
//     无需服务器。写入用 QSqlQuery::prepare + 占位符(:xxx) 防注入、便于
//     复用语句。
//   表结构：实时采样(模拟量)、设备启停事件、报警事件、
//     参数配方四类，Navicat 里各作为一张表直接查询。
#include "datastore.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>

DataStore::DataStore(QObject *parent)
    : QObject(parent)
{
}

bool DataStore::open()
{
    // 知识点：数据库文件固定在项目目录下（D:/Qt/qtpro/bigProject1），
    // Navicat 直接按该路径定位即可打开，不依赖系统默认的用户目录。
    //若是将QDir().mkpath(dir);放到后面，就会出现创建一个db文件，
    //而后续的操作是需要在db文件里面写数据库的内容
    const QString dir = QStringLiteral("D:/Qt/qtpro/bigProject1");
    //创建文件夹(mkpath如果没有文件就创建，有就跳过)
    QDir().mkpath(dir);
    m_dbPath = dir + QStringLiteral("/pumpstation.db");

    //QSqlDatabase 创建的连接不是线程安全的
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        qWarning() << "[DataStore] 数据库打开失败:" << db.lastError().text();
        return false;
    }

    // 建表（IF NOT EXISTS 幂等，重复启动不会报错）
    exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS realtime_samples("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " ts TEXT NOT NULL,"
        " supply_temp REAL, supply_pressure REAL, conductivity REAL,"
        " return_temp REAL, tower_outlet REAL, supply_flow REAL,"
        " tank_level REAL, cumulative_flow REAL, cop REAL, daily_power REAL)"));

    exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS device_events("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " ts TEXT NOT NULL, device TEXT, event TEXT)"));

    exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS alarm_events("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " ts TEXT NOT NULL, level INTEGER, type TEXT, state TEXT)"));

    exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS param_recipes("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " ts TEXT NOT NULL, name TEXT, value REAL)"));

    qDebug() << "[DataStore] 数据库就绪:" << m_dbPath;
    return true;
}

void DataStore::exec(const QString &sql)
{
    QSqlQuery q;
    if (!q.exec(sql))
        qWarning() << "[DataStore] SQL 执行失败:" << q.lastError().text() << "|" << sql;
}

void DataStore::logRealtime(double supplyTemp, double supplyPressure, double conductivity,
                            double returnTemp, double towerOutlet, double supplyFlow,
                            double tankLevel, double cumulativeFlow, double cop, double dailyPower)
{
    QSqlQuery q;
    q.prepare(QStringLiteral(
        "INSERT INTO realtime_samples(ts, supply_temp, supply_pressure, conductivity,"
        " return_temp, tower_outlet, supply_flow, tank_level, cumulative_flow, cop, daily_power)"
        " VALUES(:ts,:st,:sp,:cond,:rt,:to,:sf,:tl,:cf,:cop,:dp)"));
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    q.bindValue(QStringLiteral(":st"), supplyTemp);
    q.bindValue(QStringLiteral(":sp"), supplyPressure);
    q.bindValue(QStringLiteral(":cond"), conductivity);
    q.bindValue(QStringLiteral(":rt"), returnTemp);
    q.bindValue(QStringLiteral(":to"), towerOutlet);
    q.bindValue(QStringLiteral(":sf"), supplyFlow);
    q.bindValue(QStringLiteral(":tl"), tankLevel);
    q.bindValue(QStringLiteral(":cf"), cumulativeFlow);
    q.bindValue(QStringLiteral(":cop"), cop);
    q.bindValue(QStringLiteral(":dp"), dailyPower);
    if (!q.exec())
        qWarning() << "[DataStore] 插入实时数据失败:" << q.lastError().text();
}

void DataStore::logDeviceEvent(const QString &key, const QString &event)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT INTO device_events(ts, device, event) VALUES(:ts,:d,:e)"));
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    q.bindValue(QStringLiteral(":d"), key);
    q.bindValue(QStringLiteral(":e"), event);

    //数据库链接只能在子线程(一直运行)或线程池(一次性任务)操作，如果在主线程容易出现卡死当前页面的情况，什么都做不了
    //使用 ? 作为占位符,比上面方式更方便
    // q.prepare("INSERT INTO device_events(ts, device, event) VALUES(?, ?, ?)");

    // // 按顺序绑定，不需要写名字
    // q.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    // q.addBindValue(key);
    // q.addBindValue(event);
    if (!q.exec())
        qWarning() << "[DataStore] 插入设备事件失败:" << q.lastError().text();
}

void DataStore::logAlarm(int level, const QString &type, const QString &state)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT INTO alarm_events(ts, level, type, state) VALUES(:ts,:lvl,:tp,:st)"));
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    q.bindValue(QStringLiteral(":lvl"), level);
    q.bindValue(QStringLiteral(":tp"), type);
    q.bindValue(QStringLiteral(":st"), state);
    if (!q.exec())
        qWarning() << "[DataStore] 插入报警事件失败:" << q.lastError().text();
}

void DataStore::saveRecipe(const QStringList &names, const QVector<double> &values)
{
    const QString ts = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    QSqlQuery q;
    const int n = qMin(names.size(), values.size());
    for (int i = 0; i < n; ++i) {
        q.prepare(QStringLiteral("INSERT INTO param_recipes(ts, name, value) VALUES(:ts,:n,:v)"));
        q.bindValue(QStringLiteral(":ts"), ts);
        q.bindValue(QStringLiteral(":n"), names.at(i));
        q.bindValue(QStringLiteral(":v"), values.at(i));
        if (!q.exec())
            qWarning() << "[DataStore] 保存配方失败:" << q.lastError().text();
    }
}