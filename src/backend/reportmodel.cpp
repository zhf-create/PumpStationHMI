// ReportModel.cpp
// 知识点：
//   四类报表的数据都在构造函数里预置成静态示例（真实项目换成查
//     SQLite/CSV，接口 selectType/headers/data 不变）。
//   data() 里把一行的 QStringList 作为 QVariant 返回，QML 侧就能
//     把这个角色当成 JS 数组喂给 Repeater 生成等宽单元格。
#include "reportmodel.h"

#include <QFile>
#include <QDateTime>
#include <QDir>

ReportModel::ReportModel(QObject *parent)
    : QAbstractListModel(parent)
{
    //  日报（0） 
    {
        Table t;
        t.title = QStringLiteral("运行日报（温度/压力最值均值 + 报警汇总）");
        t.tag = QStringLiteral("日报");
        t.trendCol = 3;                              // 均温列
        t.trendAxis = QStringLiteral("均温(℃)");
        t.headers = { QStringLiteral("日期"), QStringLiteral("最高温(℃)"), QStringLiteral("最低温(℃)"),
                      QStringLiteral("均温(℃)"), QStringLiteral("均压(MPa)"), QStringLiteral("耗电量(kWh)"),
                      QStringLiteral("补水量(m³)"), QStringLiteral("报警数") };
        t.rows = {
            { QStringLiteral("2026-05-13"), QStringLiteral("33.5"), QStringLiteral("31.0"), QStringLiteral("32.1"), QStringLiteral("0.54"), QStringLiteral("1180"), QStringLiteral("8.6"), QStringLiteral("2") },
            { QStringLiteral("2026-05-12"), QStringLiteral("33.8"), QStringLiteral("31.2"), QStringLiteral("32.4"), QStringLiteral("0.53"), QStringLiteral("1215"), QStringLiteral("9.1"), QStringLiteral("0") },
            { QStringLiteral("2026-05-11"), QStringLiteral("33.1"), QStringLiteral("30.6"), QStringLiteral("31.8"), QStringLiteral("0.55"), QStringLiteral("1102"), QStringLiteral("7.9"), QStringLiteral("1") },
            { QStringLiteral("2026-05-10"), QStringLiteral("33.4"), QStringLiteral("30.9"), QStringLiteral("32.0"), QStringLiteral("0.54"), QStringLiteral("1146"), QStringLiteral("8.3"), QStringLiteral("0") },
            { QStringLiteral("2026-05-09"), QStringLiteral("33.7"), QStringLiteral("31.1"), QStringLiteral("32.3"), QStringLiteral("0.52"), QStringLiteral("1238"), QStringLiteral("9.5"), QStringLiteral("3") },
            { QStringLiteral("2026-05-08"), QStringLiteral("32.9"), QStringLiteral("30.4"), QStringLiteral("31.6"), QStringLiteral("0.56"), QStringLiteral("1085"), QStringLiteral("7.4"), QStringLiteral("0") },
            { QStringLiteral("2026-05-07"), QStringLiteral("33.2"), QStringLiteral("30.8"), QStringLiteral("31.9"), QStringLiteral("0.53"), QStringLiteral("1120"), QStringLiteral("8.0"), QStringLiteral("1") },
        };
        m_tables << t;
    }

    //  周报（1） 
    {
        Table t;
        t.title = QStringLiteral("运行周报（运行时长 / 能耗 / 报警 / 设备运行率）");
        t.tag = QStringLiteral("周报");
        t.trendCol = 2;                              // 均温列
        t.trendAxis = QStringLiteral("均温(℃)");
        t.headers = { QStringLiteral("周次"), QStringLiteral("运行时长(h)"), QStringLiteral("均温(℃)"),
                      QStringLiteral("均压(MPa)"), QStringLiteral("耗电量(kWh)"), QStringLiteral("补水量(m³)"),
                      QStringLiteral("报警次数"), QStringLiteral("运行率(%)") };
        t.rows = {
            { QStringLiteral("第19周"), QStringLiteral("168.0"), QStringLiteral("32.2"), QStringLiteral("0.54"), QStringLiteral("8120"), QStringLiteral("52.4"), QStringLiteral("4"), QStringLiteral("99.2") },
            { QStringLiteral("第18周"), QStringLiteral("167.5"), QStringLiteral("32.5"), QStringLiteral("0.53"), QStringLiteral("8260"), QStringLiteral("55.1"), QStringLiteral("2"), QStringLiteral("99.5") },
            { QStringLiteral("第17周"), QStringLiteral("168.0"), QStringLiteral("31.9"), QStringLiteral("0.55"), QStringLiteral("7980"), QStringLiteral("49.8"), QStringLiteral("6"), QStringLiteral("98.7") },
            { QStringLiteral("第16周"), QStringLiteral("167.8"), QStringLiteral("32.1"), QStringLiteral("0.54"), QStringLiteral("8050"), QStringLiteral("51.2"), QStringLiteral("3"), QStringLiteral("99.0") },
            { QStringLiteral("第15周"), QStringLiteral("168.0"), QStringLiteral("32.4"), QStringLiteral("0.52"), QStringLiteral("8190"), QStringLiteral("53.6"), QStringLiteral("5"), QStringLiteral("99.3") },
        };
        m_tables << t;
    }

    //  月报（2） 
    {
        Table t;
        t.title = QStringLiteral("运行月报（能效趋势 / 设备运行率 / 维护提醒）");
        t.tag = QStringLiteral("月报");
        t.trendCol = 6;                              // COP 列
        t.trendAxis = QStringLiteral("COP");
        t.headers = { QStringLiteral("月份"), QStringLiteral("运行率(%)"), QStringLiteral("均温(℃)"),
                      QStringLiteral("耗电量(kWh)"), QStringLiteral("补水量(m³)"), QStringLiteral("报警次数"),
                      QStringLiteral("COP"), QStringLiteral("维护提醒") };
        t.rows = {
            { QStringLiteral("2026-05"), QStringLiteral("99.0"), QStringLiteral("32.3"), QStringLiteral("124500"), QStringLiteral("203.4"), QStringLiteral("9"),  QStringLiteral("6.2"), QStringLiteral("无") },
            { QStringLiteral("2026-04"), QStringLiteral("98.7"), QStringLiteral("32.6"), QStringLiteral("129800"), QStringLiteral("210.2"), QStringLiteral("12"), QStringLiteral("6.0"), QStringLiteral("无") },
            { QStringLiteral("2026-03"), QStringLiteral("99.2"), QStringLiteral("31.8"), QStringLiteral("118600"), QStringLiteral("188.7"), QStringLiteral("7"),  QStringLiteral("6.4"), QStringLiteral("板换例行清洗") },
            { QStringLiteral("2026-02"), QStringLiteral("98.9"), QStringLiteral("31.5"), QStringLiteral("112400"), QStringLiteral("176.3"), QStringLiteral("5"),  QStringLiteral("6.6"), QStringLiteral("无") },
            { QStringLiteral("2026-01"), QStringLiteral("99.1"), QStringLiteral("31.7"), QStringLiteral("115200"), QStringLiteral("180.9"), QStringLiteral("6"),  QStringLiteral("6.5"), QStringLiteral("无") },
            { QStringLiteral("2025-12"), QStringLiteral("98.8"), QStringLiteral("32.1"), QStringLiteral("120300"), QStringLiteral("195.1"), QStringLiteral("10"), QStringLiteral("6.1"), QStringLiteral("无") },
        };
        m_tables << t;
    }

    //  年报（3） 
    {
        Table t;
        t.title = QStringLiteral("年度报表（能耗分析 / 设备寿命评估 / 运维计划）");
        t.tag = QStringLiteral("年报");
        t.trendCol = 4;                              // 平均COP 列
        t.trendAxis = QStringLiteral("平均COP");
        t.headers = { QStringLiteral("年度"), QStringLiteral("总耗电量(kWh)"), QStringLiteral("补水总量(m³)"),
                      QStringLiteral("报警总数"), QStringLiteral("平均COP"), QStringLiteral("运行率(%)"),
                      QStringLiteral("下年度运维计划") };
        t.rows = {
            { QStringLiteral("2026"), QStringLiteral("1420000"), QStringLiteral("2210"), QStringLiteral("108"), QStringLiteral("6.2"), QStringLiteral("99.1"), QStringLiteral("年度大修（春秋季）") },
            { QStringLiteral("2025"), QStringLiteral("1518000"), QStringLiteral("2380"), QStringLiteral("126"), QStringLiteral("5.9"), QStringLiteral("98.8"), QStringLiteral("板换拆洗+填料更换") },
            { QStringLiteral("2024"), QStringLiteral("1476000"), QStringLiteral("2315"), QStringLiteral("118"), QStringLiteral("6.0"), QStringLiteral("98.9"), QStringLiteral("水泵机械密封更换") },
            { QStringLiteral("2023"), QStringLiteral("1532000"), QStringLiteral("2400"), QStringLiteral("135"), QStringLiteral("5.8"), QStringLiteral("98.5"), QStringLiteral("常规年度维护") },
        };
        m_tables << t;
    }
}

int ReportModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : current().rows.size();
}

QVariant ReportModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= current().rows.size())
        return {};
    if (role == CellsRole)
        return current().rows.at(index.row());   // QStringList -> QVariant，QML 当作数组
    return {};
}

QHash<int, QByteArray> ReportModel::roleNames() const
{
    return { { CellsRole, "cells" } };
}

QStringList ReportModel::headers() const { return current().headers; }
int ReportModel::columnCount() const { return current().headers.size(); }
QString ReportModel::title() const { return current().title; }
int ReportModel::count() const { return current().rows.size(); }

void ReportModel::selectType(int type)
{
    if (type < 0 || type >= m_tables.size() || type == m_current)
        return;
    // 知识点：换数据源要 begin/endResetModel 包裹，通知视图整体重绘。
    beginResetModel();
    m_current = type;
    endResetModel();
    emit headerChanged();
}

QString ReportModel::toCsv() const
{
    // 知识点：首行表头 + 逐行逗号拼接，与表格列一一对应（CSV 导出）。
    QString csv = current().headers.join(QLatin1Char(',')) + QLatin1Char('\n');
    for (const QStringList &row : current().rows)
        csv += row.join(QLatin1Char(',')) + QLatin1Char('\n');
    return csv;
}

QString ReportModel::trendAxis() const { return current().trendAxis; }

QStringList ReportModel::trendLabels() const
{
    // 横轴 = 每行第一列（日期/周次/月份/年度）
    QStringList labels;
    for (const QStringList &row : current().rows)
        labels << row.value(0);
    return labels;
}

QVariantList ReportModel::trendValues() const
{
    // 纵轴 = 该报表指定的趋势列数值，转 double 供 QML Canvas 画折线
    QVariantList vals;
    for (const QStringList &row : current().rows)
        vals << row.value(current().trendCol).toDouble();
    return vals;
}

QString ReportModel::exportCsv() const
{
    // 把 CSV 文本写入磁盘文件并返回路径。导出目录固定为 D:\Qt\qtpro\baobiao，不存在则先创建。
    const QString dir = QStringLiteral("D:/Qt/qtpro/baobiao");
    QDir().mkpath(dir);
    const QString path = dir + QStringLiteral("/泵站") + current().tag + QStringLiteral("_")
                         + QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"))
                         + QStringLiteral(".csv");

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return QString();
    // 知识点：写 UTF-8 BOM（\xEF\xBB\xBF），让 Excel 双击打开 CSV 时正确识别
    // 中文；否则无 BOM 的 UTF-8 中文在 Excel 里会显示为乱码。
    f.write("\xEF\xBB\xBF");
    f.write(toCsv().toUtf8());
    f.close();
    return path;
}