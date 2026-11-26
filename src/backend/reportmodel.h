// ReportModel.h —— 报表数据模型（对应数据报表查询⑤）
// 知识点 / 编写原因：
//   - 日报 / 月报 / 年报三种报表，且各自字段不同
//     （日报=温度压力最值均值+报警汇总；月报=运行统计+能效趋势+运行率；
//      年报=能耗分析+寿命评估+运维计划）。本类额外补了周报，共四类。
//   - 四类数据字段数不同、语义不同，所以改成泛化表格模型。
//   - trendLabels / trendValues / trendAxis 给趋势曲线提供数据：
//     每张表指定一个趋势列（如均温、COP），Canvas 据此画折线。
//   - exportCsv() 把当前报表真正写入文件（CSV 导出）。
#ifndef REPORTMODEL_H
#define REPORTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QStringList>
#include <QVariantList>

class ReportModel : public QAbstractListModel
{
    Q_OBJECT

    // 列标题列表（随报表类型变化）
    Q_PROPERTY(QStringList headers READ headers NOTIFY headerChanged)
    // 列数 = headers.size()，供 QML 均分列宽
    Q_PROPERTY(int columnCount READ columnCount NOTIFY headerChanged)
    // 当前报表标题
    Q_PROPERTY(QString title READ title NOTIFY headerChanged)
    // 当前报表行数，供 QML 显示"共 N 条"
    Q_PROPERTY(int count READ count NOTIFY headerChanged)
    //  趋势曲线数据（随报表类型变化） 
    Q_PROPERTY(QString trendAxis READ trendAxis NOTIFY headerChanged)          // 纵轴名称
    Q_PROPERTY(QStringList trendLabels READ trendLabels NOTIFY headerChanged)  // 横轴标签（第一列）
    Q_PROPERTY(QVariantList trendValues READ trendValues NOTIFY headerChanged) // 趋势数值

public:
    enum Role {
        CellsRole = Qt::UserRole + 1   // 每行各列文本（QVariant/QStringList）
    };

    explicit ReportModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 切换报表类型：0=日报 1=周报 2=月报 3=年报（补充周报）
    Q_INVOKABLE void selectType(int type);

    // 导出当前报表为 CSV 文件（支持 CSV 导出），返回保存路径
    Q_INVOKABLE QString exportCsv() const;

    QStringList headers() const;
    int columnCount() const;
    QString title() const;
    int count() const;
    QString trendAxis() const;
    QStringList trendLabels() const;
    QVariantList trendValues() const;

signals:
    void headerChanged();

private:
    struct Table {
        QString title;
        QString tag;           // 短名（报/周报/月报/年报），用于导出文件名
        QStringList headers;
        QVector<QStringList> rows;
        int trendCol = 0;      // 趋势数值所在的列下标
        QString trendAxis;     // 趋势纵轴名称
    };
    QVector<Table> m_tables;   // 下标即报表类型
    int m_current = 0;

    // 把当前报表拼成 CSV 文本（供 exportCsv 写文件）
    QString toCsv() const;

    const Table &current() const { return m_tables.at(m_current); }
};

#endif // REPORTMODEL_H