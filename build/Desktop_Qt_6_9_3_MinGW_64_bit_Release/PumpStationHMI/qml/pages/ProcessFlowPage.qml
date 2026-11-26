// ============================================================
// ProcessFlowPage.qml —— 工艺流程画面（主界面）
//
// 布局思路（知识点 / 编写原因）：
//   1) 顶部「参数带」：10 个核心参数单行平铺，数值绑定 Backend，竖直空间让给流程图。
//   2) 本轮改为「纵向流程图」（自上而下的立式管道图画法）：
//      - 顶部：冷却塔 ⇄ 板式换热器（一次侧冷却回路，横向短连接）；
//      - 中部：板换底部出水 → 吸入口母管（左侧竖管）下沉 → 分三根短桩供入
//        3 台循环泵（竖列）→ 泵右侧短桩汇入出口母管（上升竖管）；
//      - 口径母管上到顶部 → 水平供水总管 → 模具回路供给竖管（右侧竖列）下沉
//        分四根短桩供入 4 个模具回路 → 回路右侧短桩汇入回水竖管；
//      - 底部：回水沿左下折返，回到板换左下角，形成完整二次侧循环；
//      - 左下角：补水泵 → 补水阀 → 膨胀罐/水箱。
//   3) 纵向好处：设备按「热源(板换)→动力(泵)→负载(模具)」自上而下排放，
//      流向以「下→上」折返表达，比横排更接近立式 P&ID 习惯，泵列/回路列一目了然。
//   4) 管端全连接：所有折线端点都落在设备边框或另一根管的共享拐点，无悬空端。
// ============================================================
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes
import Backend 1.0

Item {
    id: page

    // ---- 点击设备节点后显示的参数浮窗状态 ----
    property string selName: ""
    property string selDetail: ""
    property bool selVisible: false
    // 知识点：点击设备节点 → 显示该设备关键参数；再点同一设备则关闭浮窗。
    function showNode(name, detail) {
        if (selVisible && selName === name) { selVisible = false; return; }
        selName = name; selDetail = detail; selVisible = true;
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // ---------------- 顶部参数带 ----------------
        GridLayout {
            Layout.fillWidth: true
            columns: 10
            columnSpacing: 8
            rowSpacing: 8

            ParamTile { Layout.fillWidth: true; label: "供水温度";   value: Backend.supplyTemp.toFixed(2);         unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "回水温度";   value: Backend.returnTemp.toFixed(2);         unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "冷却塔出水"; value: Backend.coolingTowerOutlet.toFixed(2); unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "板换进口";   value: Backend.exchangerIn.toFixed(2);        unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "板换出口";   value: Backend.exchangerOut.toFixed(2);       unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "供水压力"; value: Backend.supplyPressure.toFixed(3);  unit: "MPa" }
            ParamTile { Layout.fillWidth: true; label: "补水压力"; value: Backend.makeupPressure.toFixed(3);  unit: "MPa" }
            ParamTile { Layout.fillWidth: true; label: "供水流量"; value: Backend.supplyFlow.toFixed(1);      unit: "m³/h" }
            ParamTile { Layout.fillWidth: true; label: "电导率"; value: Backend.conductivity.toFixed(1); unit: "μS/cm"; alarm: Backend.conductivityAlarm }
            ParamTile { Layout.fillWidth: true; label: "水箱液位"; value: Backend.tankLevel.toFixed(3);     unit: "m" }
        }

        // ---------------- 工艺流程图（水管连接） ----------------
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.panelRadius
            color: "#0C2E55"
            border.color: Theme.panelBorder

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 4

                // ---- 图例（固定，不随图滚动） ----
                Row {
                    Layout.fillWidth: true
                    spacing: 16
                    LegendItem { swatch: "#19C8E6"; text: "一次侧冷却水" }
                    LegendItem { swatch: "#FFB74D"; text: "二次侧供水" }
                    LegendItem { swatch: "#FF7043"; text: "二次侧回水" }
                    LegendItem { swatch: "#7FE0F0"; text: "软化水补水" }
                    Text {
                        text: "（管内白点流向 = 水流方向，随运行状态联动）"
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontSmall
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "（Ctrl+滚轮 缩放，放大后拖动平移）"
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontSmall
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                // ---- 流程图（固定坐标，自动缩放适配） ----
                Flickable {
                    id: flowView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    // 知识点：
                    //   - baseScale  = 自动适配基线，把 900×600 的图刚好完整落入可视区；
                    //   - zoom       = 用户手动缩放倍数（Ctrl+滚轮调整，1.0 = 初始自动适配）；
                    //   - scaleFactor= baseScale * zoom，最终等比缩放系数。
                    property real baseScale: Math.max(0.6, Math.min(flowView.width / 900, flowView.height / 600))
                    property real zoom: 1.0
                    readonly property real scaleFactor: baseScale * zoom

                    contentWidth: Math.max(flowView.width, 900 * flowView.scaleFactor)
                    contentHeight: Math.max(flowView.height, 600 * flowView.scaleFactor)

                    // Ctrl+滚轮 缩放：放大后图溢出视口时，可用鼠标按住拖动平移查看。
                    // 知识点：WheelHandler 只接受按住 Ctrl 的滚轮，避免与 Flickable 自带
                    // 的滚动（手指/鼠标滚轮平移）冲突，两者职责分开。
                    WheelHandler {
                        acceptedModifiers: Qt.ControlModifier
                        onWheel: function(wheel) {
                            // 向前滚(angleDelta.y>0)=放大、向后滚=缩小，每一步 10%；
                            // 限制 0.5~4 倍，防止缩到看不清或放大到失去参照。
                            var step = wheel.angleDelta.y > 0 ? 1.1 : 0.9;
                            flowView.zoom = Math.max(0.5, Math.min(4.0, flowView.zoom * step));
                        }
                    }

                    Item {
                        id: diagram
                        width: 900
                        height: 600
                        // 缩放后居中显示
                        x: (flowView.contentWidth - 900 * flowView.scaleFactor) / 2
                        y: (flowView.contentHeight - 600 * flowView.scaleFactor) / 2
                        transform: Scale {
                            origin.x: 0
                            origin.y: 0
                            xScale: flowView.scaleFactor
                            yScale: flowView.scaleFactor
                        }

                        // ============ 水管（画在设备下层） ============

                        // ---- 一次侧冷却回路（青）：冷却塔 ⇄ 板换 ----
                        FlowPipe { anchors.fill: parent; flowing: Backend.coolingFan1Running; pipeColor: "#19C8E6"; label: "冷却水进"; points: [[200,50],[370,50]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.coolingFan1Running; pipeColor: "#19C8E6"; label: "冷却水回"; points: [[370,70],[200,70]] }

                        // ---- 二次侧供水（橙）：板换 → 泵竖列 → 供水总管 → 模具 ----
                        // 板换底部出水 → 吸入口母管（左侧竖管下沉）
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; points: [[420,80],[420,100],[280,100],[280,390]] }
                        // 吸入口母管 → 各泵左侧短桩（供入泵）
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[280,210],[310,210]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[280,300],[310,300]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[280,390],[310,390]] }
                        // 各泵右侧短桩 → 出口母管
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[410,210],[460,210]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[410,300],[460,300]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[410,390],[460,390]] }
                        // 出口母管（右侧竖管上升）→ 供水总管
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; points: [[460,390],[460,80]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: "供水 40℃"; points: [[460,80],[640,80]] }
                        // 模具回路供给竖管（下沉）+ 4 根供入短桩
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; points: [[640,80],[640,440]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[640,80],[670,80]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[640,200],[670,200]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[640,320],[670,320]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FFB74D"; label: ""; showArrow: false; points: [[640,440],[670,440]] }

                        // ---- 二次侧回水（橙红）：模具 → 回水竖管 → 左下折返 → 板换 ----
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FF7043"; label: ""; showArrow: false; points: [[770,80],[800,80]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FF7043"; label: ""; showArrow: false; points: [[770,200],[800,200]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FF7043"; label: ""; showArrow: false; points: [[770,320],[800,320]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FF7043"; label: ""; showArrow: false; points: [[770,440],[800,440]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.cyclePump1Running; pipeColor: "#FF7043"; label: "回水 45℃"; points: [[800,80],[800,470],[240,470],[240,80],[370,80]] }

                        // ---- 软化水补水（浅青）：补水泵 → 补水阀 → 膨胀罐/水箱 ----
                        FlowPipe { anchors.fill: parent; flowing: Backend.makeupValveOpen; pipeColor: "#7FE0F0"; label: "软化水补水"; points: [[200,540],[250,540]] }
                        FlowPipe { anchors.fill: parent; flowing: Backend.makeupValveOpen; pipeColor: "#7FE0F0"; label: ""; showArrow: false; points: [[350,540],[400,540]] }

                        // ============ 设备节点（画在管线上层） ============

                        // 一次侧：冷却塔 + 板式换热器
                        FlowNode {
                            cx: 150; cy: 50; type: "tower"; name: "冷却塔"; status: Backend.coolingFan1Running ? "run" : "stop"
                            detail: "出水温度 " + Backend.coolingTowerOutlet.toFixed(1) + " ℃\n" +
                                    "风机 " + (Backend.coolingFan1Running ? "运行" : "停止") + "\n" +
                                    "频率 " + Backend.coolingFanFreq.toFixed(1) + " Hz"
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 420; cy: 50; type: "exchanger"; name: "板式换热器"; status: Backend.valvePos1 > 5 ? "run" : "stop"
                            detail: "进水 " + Backend.exchangerIn.toFixed(2) + " ℃\n" +
                                    "出水 " + Backend.exchangerOut.toFixed(2) + " ℃\n" +
                                    "一次侧阀 " + Backend.valvePos1.toFixed(0) + " %"
                            onClicked: page.showNode(name, detail)
                        }

                        // 循环水泵组（竖列）
                        FlowNode {
                            cx: 360; cy: 210; type: "pump"; name: "循环泵#1"; status: Backend.cyclePump1Running ? "run" : "stop"
                            detail: "电流 " + Backend.cyclePump1Current.toFixed(1) + " A\n" +
                                    (Backend.cyclePump1Running ? "运行中" : "已停止")
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 360; cy: 300; type: "pump"; name: "循环泵#2"; status: Backend.cyclePump2Running ? "run" : "stop"
                            detail: "电流 " + Backend.cyclePump2Current.toFixed(1) + " A\n" +
                                    (Backend.cyclePump2Running ? "运行中" : "已停止")
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 360; cy: 390; type: "pump"; name: "循环泵#3"; status: Backend.cyclePump3Running ? "run" : "stop"
                            detail: "备用泵（N+1）\n" +
                                    (Backend.cyclePump3Running ? "已投入运行" : "待命停机")
                            onClicked: page.showNode(name, detail)
                        }

                        // 模具回路（竖列）
                        FlowNode {
                            cx: 720; cy: 80;  type: "circuit"; name: "模具回路A"; status: Backend.cyclePump1Running ? "run" : "stop"
                            detail: "供水 " + Backend.supplyTemp.toFixed(1) + " ℃\n" +
                                    "回水 " + Backend.returnTemp.toFixed(1) + " ℃\n" +
                                    "流量 " + Backend.supplyFlow.toFixed(1) + " m³/h"
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 720; cy: 200; type: "circuit"; name: "模具回路B"; status: Backend.cyclePump1Running ? "run" : "stop"
                            detail: "供水 " + Backend.supplyTemp.toFixed(1) + " ℃\n" +
                                    "回水 " + Backend.returnTemp.toFixed(1) + " ℃\n" +
                                    "流量 " + Backend.supplyFlow.toFixed(1) + " m³/h"
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 720; cy: 320; type: "circuit"; name: "模具回路C"; status: Backend.cyclePump1Running ? "run" : "stop"
                            detail: "供水 " + Backend.supplyTemp.toFixed(1) + " ℃\n" +
                                    "回水 " + Backend.returnTemp.toFixed(1) + " ℃\n" +
                                    "流量 " + Backend.supplyFlow.toFixed(1) + " m³/h"
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 720; cy: 440; type: "circuit"; name: "模具回路D"; status: Backend.cyclePump1Running ? "run" : "stop"
                            detail: "供水 " + Backend.supplyTemp.toFixed(1) + " ℃\n" +
                                    "回水 " + Backend.returnTemp.toFixed(1) + " ℃\n" +
                                    "流量 " + Backend.supplyFlow.toFixed(1) + " m³/h"
                            onClicked: page.showNode(name, detail)
                        }

                        // 补水定压系统
                        FlowNode {
                            cx: 150; cy: 540; type: "pump"; name: "补水泵"; status: Backend.makeupPump1Running ? "run" : "stop"
                            detail: "电流 " + Backend.makeupPump1Current.toFixed(1) + " A\n" +
                                    (Backend.makeupPump1Running ? "运行中" : "已停止")
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 300; cy: 540; type: "valve"; name: "补水电磁阀"; status: Backend.makeupValveOpen ? "run" : "stop"
                            detail: (Backend.makeupValveOpen ? "已打开" : "已关闭") + "\n" +
                                    "补水压力 " + Backend.makeupPressure.toFixed(3) + " MPa"
                            onClicked: page.showNode(name, detail)
                        }
                        FlowNode {
                            cx: 450; cy: 540; type: "tank"; name: "膨胀罐/水箱"; status: "run"
                            detail: "水箱液位 " + Backend.tankLevel.toFixed(3) + " m\n" +
                                    "膨胀罐液位 " + Backend.expansionLevel.toFixed(3) + " m"
                            onClicked: page.showNode(name, detail)
                        }

                    }
                }
            }

            // ---- 点击设备节点后弹出的参数浮窗（覆盖在流程图右上角） ----
            // 知识点：浮窗放在 flow 面板 Rectangle 内、z 值最高，盖在画布之上；
            // 内容绑定 page.selName / selDetail，数值实时刷新。
            Rectangle {
                id: infoPanel
                z: 10
                anchors.top: parent.top
                anchors.topMargin: Math.round(40 * Theme.uiScale)
                anchors.right: parent.right
                anchors.rightMargin: Math.round(10 * Theme.uiScale)
                width: Math.round(250 * Theme.uiScale)
                height: Math.round(116 * Theme.uiScale)
                visible: page.selVisible
                radius: 6
                color: "#12365F"
                border.color: Theme.navActive
                border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 5

                    RowLayout {
                        width: parent.width
                        Text {
                            text: page.selName
                            color: Theme.textTitle
                            font.pixelSize: Theme.fontNav
                            font.bold: true
                        }
                        Item { Layout.fillWidth: true; width: 1 }
                        Text {
                            text: "✕"
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontNav
                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: page.selVisible = false
                            }
                        }
                    }

                    Text {
                        width: parent.width
                        text: page.selDetail
                        color: Theme.textValue
                        font.pixelSize: Theme.fontSmall
                        lineHeight: 1.4
                    }
                }
            }
        }
    }

    // ============================================================
    // LegendItem —— 图例色块 + 文字
    // ============================================================
    component LegendItem: Row {
        property color swatch: "#19C8E6"
        property string text: ""
        spacing: 4

        Rectangle {
            width: 18; height: 6; radius: 2
            color: parent.swatch
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: parent.text
            color: Theme.textLabel
            font.pixelSize: Theme.fontSmall
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // ============================================================
    // FlowPipe —— 折线水管：画管 + 流动粒子 + 流向箭头 + 标签
    // 知识点：
    //   - `points` 是一组折线拐点 [[x,y],...]，用 PathSvg 的 M/L 指令拼路径。
    //   - 粒子沿折线匀速流动：段长累加 + 线性插值（posAt），避免长短段忽快忽慢。
    //   - `flowing` 绑定真实运行态：为 true 时白点循环流动，false 时水停。
    //   - 箭头末端回退 14px（arrowPos）避免被设备方框遮挡；短桩 showArrow=false。
    // ============================================================
    component FlowPipe: Item {
        property var points: []
        property bool flowing: false
        property color pipeColor: "#19C8E6"
        property string label: ""
        property real lineWidth: 3
        property bool showArrow: true

        property real t: 0
        NumberAnimation on t {
            running: flowing
            from: 0
            to: 1
            duration: 1600
            loops: Animation.Infinite
        }

        function pathString() {
            var s = "";
            for (var i = 0; i < points.length; i++) {
                s += (i === 0 ? "M" : "L") + " " + points[i][0] + " " + points[i][1] + " ";
            }
            return s;
        }

        function posAt(val) {
            if (points.length < 2) return { x: 0, y: 0 };
            var cum = [0], total = 0;
            for (var i = 0; i < points.length - 1; i++) {
                var dx = points[i + 1][0] - points[i][0];
                var dy = points[i + 1][1] - points[i][1];
                total += Math.sqrt(dx * dx + dy * dy);
                cum.push(total);
            }
            if (total <= 0) return { x: points[0][0], y: points[0][1] };
            var dist = val * total;
            for (var j = 0; j < points.length - 1; j++) {
                if (dist <= cum[j + 1]) {
                    var f = (dist - cum[j]) / (cum[j + 1] - cum[j]);
                    return { x: points[j][0] + f * (points[j + 1][0] - points[j][0]),
                             y: points[j][1] + f * (points[j + 1][1] - points[j][1]) };
                }
            }
            return { x: points[points.length - 1][0], y: points[points.length - 1][1] };
        }

        function endAngleDeg() {
            if (points.length < 2) return 0;
            var n = points.length;
            var dx = points[n - 1][0] - points[n - 2][0];
            var dy = points[n - 1][1] - points[n - 2][1];
            return Math.atan2(dy, dx) * 180 / Math.PI;
        }

        function arrowPos() {
            if (points.length < 2) return { x: 0, y: 0 };
            var n = points.length;
            var ex = points[n - 1][0], ey = points[n - 1][1];
            var sx = points[n - 2][0], sy = points[n - 2][1];
            var dx = ex - sx, dy = ey - sy;
            var len = Math.sqrt(dx * dx + dy * dy);
            if (len <= 0) return { x: ex, y: ey };
            var k = Math.min(1, 14 / len);
            return { x: ex - dx * k, y: ey - dy * k };
        }

        Shape {
            anchors.fill: parent
            ShapePath {
                strokeColor: pipeColor
                strokeWidth: lineWidth
                capStyle: ShapePath.RoundCap
                joinStyle: ShapePath.RoundJoin
                fillColor: "transparent"
                PathSvg { path: pathString() }
            }
        }

        Rectangle {
            visible: flowing
            width: 5; height: 5; radius: 2.5
            color: "#FFFFFF"
            x: posAt(t).x - 2.5
            y: posAt(t).y - 2.5
        }
        Rectangle {
            visible: flowing
            width: 5; height: 5; radius: 2.5
            color: "#FFFFFF"
            x: posAt((t + 0.5) % 1.0).x - 2.5
            y: posAt((t + 0.5) % 1.0).y - 2.5
        }

        Item {
            visible: showArrow && points.length >= 2
            x: arrowPos().x
            y: arrowPos().y
            rotation: endAngleDeg()
            Shape {
                x: -8; y: -5; width: 8; height: 10
                ShapePath {
                    fillColor: pipeColor
                    strokeColor: pipeColor
                    strokeWidth: 0.5
                    PathSvg { path: "M 8 5 L 0 0 L 0 10 Z" }
                }
            }
        }

        Rectangle {
            visible: label !== ""
            x: posAt(0.5).x - width / 2
            y: posAt(0.5).y - height / 2
            width: lbl.paintedWidth + 8
            height: 16
            radius: 3
            color: "#0B2B4E"
            opacity: 0.88
            Text {
                id: lbl
                anchors.centerIn: parent
                text: parent.parent.label
                color: Theme.textValue
                font.pixelSize: Theme.fontTiny
            }
        }
    }

    // ============================================================
    // FlowNode —— 设备节点：图标 + 名称 + 三色状态
    // ============================================================
    component FlowNode: Item {
        id: node
        property string type: "pump"
        property string name: ""
        property string status: "run"
        property string detail: ""   // 点击后浮窗展示的关键参数
        property real cx: 0
        property real cy: 0
        signal clicked()            // 点击设备节点发出，供外围弹浮窗

        width: 100
        height: 60
        x: cx - width / 2
        y: cy - height / 2

        Rectangle {
            anchors.fill: parent
            radius: 6
            color: "#12365F"
            border.color: node.status === "fault" ? Theme.fault : Theme.panelBorder
            border.width: 1
        }

        EquipmentIcon {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 2
            type: node.type
            running: node.status === "run"
            iconScale: 1.5
        }

        Row {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 3
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 3
            Text {
                text: node.name
                color: Theme.textTitle
                font.pixelSize: Theme.fontTiny
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
            StatusPill { status: node.status; text: "" }
        }

        // 覆盖整块的点击区（放在最上层拦截点击），点击设备看参数
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: node.clicked()
        }
    }
}