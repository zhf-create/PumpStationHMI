// ============================================================
// ReportPage.qml —— 数据报表查询
// 日报/月报/年报；CSV 导出；趋势曲线。
//
// 知识点 / 编写原因：
//   - 泛化表格：TabBar 4 个按钮调用 Backend.reports.selectType(n)，
//     切到日报/周报/月报/年报四套不同数据；表头与列宽从
//     Backend.reports.headers / columnCount 动态算，等宽分列。
//   - CSV 导出：点击「导出 CSV」调用 C++ exportCsv()，真正写文件并
//     返回保存路径，QML 只负责展示路径提示。
//   - 趋势曲线：Canvas 从 Backend.reports.trendValues/trendLabels/trendAxis
//     取数，用 moveTo/lineTo 画折线 + 坐标轴，切换报表或开关面板时重绘。
// ============================================================
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

Item {
    id: root

    property bool showTrend: false

    // 知识点：切换报表的同时，若趋势面板可见则触发 Canvas 重绘，
    // 让图表跟随新数据刷新（selectType 是同步的，先换数据再重绘即可）。
    function selectReport(n) {
        Backend.reports.selectType(n);
        if (showTrend) trendChart.requestPaint();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // ---------------- 报表类型切换 ----------------
        TabBar {
            id: bar
            Layout.fillWidth: true
            background: Rectangle { color: "#0F3560"; radius: 6 }
            TabButton { text: "日报"; onClicked: root.selectReport(0) }
            TabButton { text: "周报"; onClicked: root.selectReport(1) }
            TabButton { text: "月报"; onClicked: root.selectReport(2) }
            TabButton { text: "年报"; onClicked: root.selectReport(3) }
        }

        // ---------------- 标题 + 记录数 ----------------
        Row {
            Layout.fillWidth: true
            spacing: 12
            Text {
                text: Backend.reports.title
                color: Theme.textTitle
                font.pixelSize: Theme.fontMedium
                font.bold: true
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "（共 " + Backend.reports.count + " 条 · 来自历史数据库，演示数据）"
                color: Theme.textMuted
                font.pixelSize: Theme.fontLabel
            }
        }

        // ---------------- 表格面板 ----------------
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.panelRadius
            color: Theme.panel
            border.color: Theme.panelBorder

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 2

                // 表头（列标题来自 C++ headers）
                Rectangle {
                    id: headerRow
                    Layout.fillWidth: true
                    Layout.preferredHeight: 34
                    radius: 4
                    color: "#1B4E7E"
                    border.color: Theme.panelBorder
                    Row {
                        anchors.fill: parent
                        Repeater {
                            model: Backend.reports.headers
                            delegate: CellLabel {
                                width: headerRow.width / Backend.reports.columnCount
                                height: headerRow.height
                                header: true
                            }
                        }
                    }
                }

                // 数据行（cells 角色 = 每行各列文本数组）
                ListView {
                    id: reportList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 2
                    model: Backend.reports
                    delegate: Row {
                        width: reportList.width
                        height: 30
                        Repeater {
                            model: cells
                            delegate: CellLabel {
                                width: reportList.width / Backend.reports.columnCount
                                height: 30
                            }
                        }
                    }
                }
            }
        }

        // ---------------- 导出 + 趋势按钮 + 提示 ----------------
        Column {
            Layout.fillWidth: true
            spacing: 4

            Row {
                width: parent.width
                spacing: 10
                Button {
                    text: "导出 CSV"
                    onClicked: {
                        var p = Backend.reports.exportCsv();
                        hint.text = p === "" ? "导出失败，请检查目录权限" : "已导出到：" + p;
                    }
                }
                Button {
                    text: showTrend ? "收起趋势曲线" : "查看趋势曲线"
                    onClicked: {
                        showTrend = !showTrend;
                        if (showTrend) trendChart.requestPaint();
                    }
                }
            }
            Text {
                id: hint
                width: parent.width
                text: ""
                color: Theme.textValue
                font.pixelSize: Theme.fontLabel
                elide: Text.ElideMiddle   // 路径过长时中间省略，避免溢出
            }
        }

        // ---------------- 趋势曲线面板 ----------------
        Rectangle {
            id: trendPanel
            Layout.fillWidth: true
            Layout.preferredHeight: showTrend ? 200 : 0
            visible: showTrend
            clip: true
            radius: Theme.panelRadius
            color: Theme.panel
            border.color: Theme.panelBorder

            Column {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 4

                Text {
                    id: trendTitle
                    text: "趋势曲线（纵轴：" + Backend.reports.trendAxis + "）"
                    color: Theme.textTitle
                    font.pixelSize: Theme.fontNav
                    font.bold: true
                }

                Canvas {
                    id: trendChart
                    width: parent.width
                    height: Math.max(0, parent.height - trendTitle.height - 4)
                    onWidthChanged: requestPaint()
                    onHeightChanged: requestPaint()

                    onPaint: {
                        var ctx = getContext("2d");
                        var w = width, h = height;
                        ctx.clearRect(0, 0, w, h);

                        var values = Backend.reports.trendValues;
                        var labels = Backend.reports.trendLabels;
                        if (!values || values.length === 0)
                            return;

                        var n = values.length;
                        var padL = 48, padR = 16, padT = 14, padB = 26;
                        var plotW = w - padL - padR;
                        var plotH = h - padT - padB;

                        // 计算纵轴范围（min/max），避免固定刻度导致折线压在底部
                        var min = values[0], max = values[0];
                        for (var i = 1; i < n; i++) {
                            if (values[i] < min) min = values[i];
                            if (values[i] > max) max = values[i];
                        }
                        var range = (max - min);
                        if (range <= 0.0001) range = 1;   // 避免除 0

                        // 知识点：上下各扩 10% 作为绘图留白。否则 max 正好映射到
                        // 顶边 y=padT、min 映射到底边，3px 数据点会贴边被裁剪、
                        // 极值无法完整显示；平线数据（min==max）也会被压到底边。
                        var pad = range * 0.1; if (pad <= 0.0001) pad = 1;
                        var yMin = min - pad;
                        var yMax = max + pad;
                        var yRange = yMax - yMin;

                        function px(i) { return padL + (n === 1 ? 0 : i * plotW / (n - 1)); }
                        function py(v) { return padT + plotH - (v - yMin) / yRange * plotH; }

                        // 横网格线
                        ctx.strokeStyle = "#2E6DA6";
                        ctx.lineWidth = 1;
                        for (var g = 0; g <= 4; g++) {
                            var gy = padT + plotH * g / 4;
                            ctx.beginPath();
                            ctx.moveTo(padL, gy);
                            ctx.lineTo(w - padR, gy);
                            ctx.stroke();
                        }

                        // 纵轴刻度（min / max）
                        ctx.fillStyle = "#AFC9E8";
                        ctx.font = "10px sans-serif";
                        ctx.textAlign = "right";
                        ctx.fillText(yMax.toFixed(1), padL - 6, padT + 4);
                        ctx.fillText(yMin.toFixed(1), padL - 6, padT + plotH);

                        // 折线
                        ctx.strokeStyle = "#19C8E6";
                        ctx.lineWidth = 2;
                        ctx.beginPath();
                        for (i = 0; i < n; i++) {
                            var x = px(i), y = py(values[i]);
                            if (i === 0) ctx.moveTo(x, y);
                            else ctx.lineTo(x, y);
                        }
                        ctx.stroke();

                        // 数据点
                        ctx.fillStyle = "#FFE082";
                        for (i = 0; i < n; i++) {
                            ctx.beginPath();
                            ctx.arc(px(i), py(values[i]), 3, 0, 2 * Math.PI);
                            ctx.fill();
                        }

                        // 横轴标签（抽样显示，避免标签重叠）
                        ctx.fillStyle = "#AFC9E8";
                        ctx.textAlign = "center";
                        var step = Math.max(1, Math.ceil(n / 6));
                        for (i = 0; i < n; i += step) {
                            ctx.fillText(labels[i], px(i), h - 6);
                        }
                    }
                }
            }
        }
    }

    // ---------- 内联组件：表格单元格 ----------
    component CellLabel: Rectangle {
        // 知识点：Qt6 的 Repeater 里，delegate 若写成内联 component，
        // 当前列表项字符串须显式声明为 required modelData 才能访问；
        // 否则运行时报 "modelData is not defined"。
        required property string modelData
        property bool header: false

        color: header ? "#1B4E7E" : "transparent"
        border.color: Theme.panelBorder
        border.width: 1
        Text {
            anchors.fill: parent
            anchors.leftMargin: 4
            anchors.rightMargin: 4
            text: parent.modelData
            color: parent.header ? Theme.textTitle : Theme.textValue
            font.pixelSize: parent.header ? Theme.fontLabel : Theme.fontNav
            font.bold: parent.header
            elide: Text.ElideRight   // 太窄时省略号，避免字符串溢出单元格
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}