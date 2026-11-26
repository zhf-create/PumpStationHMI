// ============================================================
// OperationDataPage.qml —— 运行数据画面
// 控制与监测层采集参数、能效优化、
//          数据记录内容：耗电/运行时间/流量/电导率。
//
// 知识点：
//   - Flickable：参数多，超出可视区时上下滚动查看。
//   - GridLayout 等宽网格：比 Flow 更「工业化」——每张参数卡
//     同宽同高、行列对齐，不再出现 Flow 换行后右端参差不齐、
//     内容挤向一侧的情况（Layout.fillWidth 让卡片平分每列）。
//   - 所有数值绑定 Backend 实时属性，后端一更新 UI 即刷新。
//   - 能效 KPI（COP/湿球温度/当日耗电/累计运行时间）来自
//     PumpBackend 新增的 NOTIFY 属性，与报表里的能效数据呼应。
// ============================================================
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

Item {
    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: contentCol.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: contentCol
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 8
            spacing: 12

            // ---------------- 模拟量参数 ----------------
            SectionTitle { text: "模拟量参数（实时值）" }
            GridLayout {
                width: parent.width
                columns: 5
                columnSpacing: 8
                rowSpacing: 8

                ParamTile { label: "供水温度";     value: Backend.supplyTemp.toFixed(2);         unit: "℃";    Layout.fillWidth: true }
                ParamTile { label: "回水温度";     value: Backend.returnTemp.toFixed(2);         unit: "℃";    Layout.fillWidth: true }
                ParamTile { label: "冷却塔出水";   value: Backend.coolingTowerOutlet.toFixed(2); unit: "℃";    Layout.fillWidth: true }
                ParamTile { label: "板换进口";     value: Backend.exchangerIn.toFixed(2);        unit: "℃";    Layout.fillWidth: true }
                ParamTile { label: "板换出口";     value: Backend.exchangerOut.toFixed(2);       unit: "℃";    Layout.fillWidth: true }

                ParamTile { label: "供水压力";     value: Backend.supplyPressure.toFixed(3);     unit: "MPa";  Layout.fillWidth: true }
                ParamTile { label: "回水压力";     value: Backend.returnPressure.toFixed(3);     unit: "MPa";  Layout.fillWidth: true }
                ParamTile { label: "补水压力";     value: Backend.makeupPressure.toFixed(3);     unit: "MPa";  Layout.fillWidth: true }
                ParamTile { label: "供水流量";     value: Backend.supplyFlow.toFixed(1);         unit: "m³/h"; Layout.fillWidth: true }
                ParamTile { label: "补水累计流量"; value: Backend.cumulativeFlow.toFixed(1);     unit: "m³";   Layout.fillWidth: true }

                ParamTile { label: "电导率";       value: Backend.conductivity.toFixed(1);       unit: "μS/cm"; alarm: Backend.conductivityAlarm; Layout.fillWidth: true }
                ParamTile { label: "水箱液位";     value: Backend.tankLevel.toFixed(3);          unit: "m";     Layout.fillWidth: true }
                ParamTile { label: "膨胀罐液位";   value: Backend.expansionLevel.toFixed(2);     unit: "m";     Layout.fillWidth: true }
                ParamTile { label: "板换#1阀位";   value: Backend.valvePos1.toFixed(1);          unit: "%";     Layout.fillWidth: true }
                ParamTile { label: "板换#2阀位";   value: Backend.valvePos2.toFixed(1);          unit: "%";     Layout.fillWidth: true }
                // 变频调速结果（风机变频调速 / 供水泵变频恒压）
                ParamTile { label: "供水泵频率";   value: Backend.supplyPumpFreq.toFixed(1);     unit: "Hz";    Layout.fillWidth: true }
                ParamTile { label: "冷却塔风机频率"; value: Backend.coolingFanFreq.toFixed(1);   unit: "Hz";    Layout.fillWidth: true }
            }

            // ---------------- 实时趋势曲线 ----------------
            // 知识点：「趋势曲线」指历史趋势（见报表页）；此处按
            // 刷新策略（数值/动画/趋势绑定 Tag、温度属中速组 1-2s），
            // 用 Timer 每秒采样 Backend 实时值，滚动显示近 60 秒供水/回水温度。
            SectionTitle { text: "实时趋势曲线（近 60 秒 · 供水 / 回水温度）" }
            Rectangle {
                id: livePanel
                width: parent.width
                height: 200
                radius: Theme.panelRadius
                color: Theme.panel
                border.color: Theme.panelBorder

                // 两组滚动采样序列（JS 数组，Timer 每秒 push/shift）
                property var tSeries: []
                property var rSeries: []
                property int maxPts: 60

                Canvas {
                    id: rtChart
                    anchors.fill: parent
                    anchors.margins: 8
                    onPaint: {
                        var ctx = getContext("2d");
                        var w = width, h = height;
                        ctx.clearRect(0, 0, w, h);

                        var n = livePanel.tSeries.length;
                        if (n === 0) return;

                        var padL = 48, padR = 16, padT = 14, padB = 26;
                        var plotW = w - padL - padR;
                        var plotH = h - padT - padB;

                        // 合并两组数据求 min/max（两序列同长、同步采样）
                        var min = 1e9, max = -1e9;
                        for (var i = 0; i < n; i++) {
                            var a = livePanel.tSeries[i], b = livePanel.rSeries[i];
                            if (a < min) min = a; if (a > max) max = a;
                            if (b < min) min = b; if (b > max) max = b;
                        }
                        var range = max - min; if (range <= 0.0001) range = 1;
                        var pad = range * 0.1; if (pad <= 0.0001) pad = 1;
                        var yMin = min - pad, yMax = max + pad, yRange = yMax - yMin;

                        // 最新样本靠右，形成向左滚动的走势
                        function px(k) { return padL + (n === 1 ? plotW : k * plotW / (n - 1)); }
                        function py(v) { return padT + plotH - (v - yMin) / yRange * plotH; }

                        // 网格线 + 纵轴刻度
                        ctx.strokeStyle = "#2E6DA6"; ctx.lineWidth = 1;
                        for (var g = 0; g <= 4; g++) {
                            var gy = padT + plotH * g / 4;
                            ctx.beginPath();
                            ctx.moveTo(padL, gy); ctx.lineTo(w - padR, gy);
                            ctx.stroke();
                        }
                        ctx.fillStyle = "#AFC9E8"; ctx.font = "10px sans-serif"; ctx.textAlign = "right";
                        ctx.fillText(yMax.toFixed(1), padL - 6, padT + 4);
                        ctx.fillText(yMin.toFixed(1), padL - 6, padT + plotH);

                        // 折线 + 数据点
                        function plot(arr, color) {
                            ctx.strokeStyle = color; ctx.lineWidth = 2;
                            ctx.beginPath();
                            for (var k = 0; k < arr.length; k++) {
                                var x = px(k), y = py(arr[k]);
                                if (k === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
                            }
                            ctx.stroke();
                            ctx.fillStyle = color;
                            for (var q = 0; q < arr.length; q++) {
                                ctx.beginPath();
                                ctx.arc(px(q), py(arr[q]), 2.5, 0, 2 * Math.PI);
                                ctx.fill();
                            }
                        }
                        plot(livePanel.tSeries, "#19C8E6");   // 供水温度（青）
                        plot(livePanel.rSeries, "#FFB74D");   // 回水温度（橙）

                        // 时间轴标签
                        ctx.fillStyle = "#AFC9E8";
                        ctx.textAlign = "center";
                        ctx.fillText("60s前", padL, h - 6);
                        ctx.fillText("现在", w - padR, h - 6);
                    }
                }

                // 图例
                RowLayout {
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: 8
                    spacing: 6
                    Rectangle { width: 10; height: 10; radius: 2; color: "#19C8E6"; Layout.alignment: Qt.AlignVCenter }
                    Text { text: "供水温度"; color: Theme.textValue; font.pixelSize: Theme.fontSmall; Layout.alignment: Qt.AlignVCenter }
                    Rectangle { width: 10; height: 10; radius: 2; color: "#FFB74D"; Layout.alignment: Qt.AlignVCenter }
                    Text { text: "回水温度"; color: Theme.textValue; font.pixelSize: Theme.fontSmall; Layout.alignment: Qt.AlignVCenter }
                }

                // 每秒采样一次（温度属中速组 1-2s，取 1s 贴合刷新策略）
                Timer {
                    interval: 1000
                    running: true
                    repeat: true
                    onTriggered: {
                        livePanel.tSeries.push(Backend.supplyTemp);
                        livePanel.rSeries.push(Backend.returnTemp);
                        if (livePanel.tSeries.length > livePanel.maxPts) {
                            livePanel.tSeries.shift();
                            livePanel.rSeries.shift();
                        }
                        rtChart.requestPaint();
                    }
                }
            }

            // ---------------- 能效 / 能耗 KPI ----------------
            SectionTitle { text: "能效与能耗指标（文档 5.6 / 5.8 / 11.3）" }
            GridLayout {
                width: parent.width
                columns: 4
                columnSpacing: 8
                rowSpacing: 8

                ParamTile { label: "系统性能系数 COP"; value: Backend.cop.toFixed(2);          unit: "";     Layout.fillWidth: true }
                ParamTile { label: "环境湿球温度";       value: Backend.wetBulbTemp.toFixed(1);  unit: "℃";   Layout.fillWidth: true }
                ParamTile { label: "当日累计耗电";       value: Backend.dailyPower.toFixed(1);   unit: "kWh"; Layout.fillWidth: true }
                ParamTile { label: "水泵累计运行时间";   value: Backend.pumpRunHours.toFixed(0);  unit: "h";   Layout.fillWidth: true }
            }

            // ---------------- 设备运行状态 ----------------
            SectionTitle { text: "设备运行状态" }
            GridLayout {
                width: parent.width
                columns: 5
                columnSpacing: 8
                rowSpacing: 8

                DeviceCard { name: "循环水泵 #1";   running: Backend.cyclePump1Running;  onText: "运行"; offText: "停止"; detail: Backend.cyclePump1Current.toFixed(1) + " A" }
                DeviceCard { name: "循环水泵 #2";   running: Backend.cyclePump2Running;  onText: "运行"; offText: "停止"; detail: Backend.cyclePump2Current.toFixed(1) + " A" }
                DeviceCard { name: "循环水泵 #3";   running: Backend.cyclePump3Running;  onText: "运行"; offText: "停止"; detail: "备用（N+1）" }
                DeviceCard { name: "冷却塔风机 #1"; running: Backend.coolingFan1Running; onText: "运行"; offText: "停止" }
                DeviceCard { name: "冷却塔风机 #2"; running: Backend.coolingFan2Running; onText: "运行"; offText: "停止" }

                DeviceCard { name: "喷淋泵 #1";     running: Backend.sprayPump1Running;  onText: "运行"; offText: "停止" }
                DeviceCard { name: "补水泵 #1";     running: Backend.makeupPump1Running; onText: "运行"; offText: "停止"; detail: Backend.makeupPump1Current.toFixed(1) + " A" }
                DeviceCard { name: "补水泵 #2";     running: Backend.makeupPump2Running; onText: "运行"; offText: "停止"; detail: Backend.makeupPump2Current.toFixed(1) + " A" }
                DeviceCard { name: "补水电磁阀";    running: Backend.makeupValveOpen;    onText: "打开"; offText: "关闭" }
                DeviceCard { name: "排污电磁阀";    running: Backend.drainValveOpen;     onText: "打开"; offText: "关闭" }
            }
        }
    }

    // ---------- 内联组件：分区标题条 ----------
    component SectionTitle: Rectangle {
        required property string text

        width: parent.width
        height: 30
        radius: 4
        color: "#0F3560"
        border.color: Theme.panelBorder

        // 左侧青色强调条，工业化标题样式
        Rectangle {
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            width: 4; height: 16; radius: 2
            color: Theme.navActive
        }
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 18
            anchors.verticalCenter: parent.verticalCenter
            text: parent.text
            color: Theme.textTitle
            font.pixelSize: Theme.fontMedium
            font.bold: true
        }
    }

    // ---------- 内联组件：设备运行状态卡 ----------
    component DeviceCard: Rectangle {
        required property string name    // 设备名
        required property bool running   // 运行/打开 状态
        required property string onText  // 运行态文字（运行/打开）
        required property string offText // 停止态文字（停止/关闭）
        property string detail: ""       // 附加参数（如电流），为空则不显示

        Layout.fillWidth: true
        Layout.preferredHeight: 54
        radius: Theme.panelRadius
        color: Theme.panel
        border.color: Theme.panelBorder

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            Text {
                Layout.preferredWidth: 96
                text: name
                color: Theme.textTitle
                font.pixelSize: Theme.fontNav
                font.bold: true
            }
            StatusPill { status: running ? "run" : "stop"; text: running ? onText : offText }
            // 弹性占位，把 detail 推到卡片右侧
            Item { Layout.fillWidth: true }
            Text {
                text: detail
                color: Theme.textValue
                font.pixelSize: Theme.fontNav
                visible: detail !== ""
            }
        }
    }
}