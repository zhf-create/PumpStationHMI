// ============================================================
// AutoParamPage.qml —— 自控参数设定
// 对应文档 6.4（PID 工程整定）、5.5（报警阈值）、5.7（补水定压）。
//
// 知识点 / 布局原因：
//   - 之前是一列窄表单（label 固定 190px + 输入框 140px），整体挤在
//     左侧，右侧大片留白。现改为 GridLayout 两列：每张参数卡
//     Layout.fillWidth 自适应拉宽，参数均分到左右两列，占满整页。
//   - paramValue 直接绑定 C++ Backend 参数（含 WRITE）。
//   - TextField 的 text 是「初始化型绑定」：首次编辑后绑定自动断开，
//     避免后端回写覆盖用户正在输入的内容。
//   - onCommit 回调把编辑结果写回 Backend（如 Backend.tempSetpoint = v）。
// ============================================================
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

Item {
    // ---------- 内联组件：分组标题（横跨两列） ----------
    component GroupHeader: Rectangle {
        required property string text

        Layout.fillWidth: true
        Layout.columnSpan: 2           // 标题条占满两列
        height: 30
        radius: 4
        color: "#0F3560"
        border.color: Theme.panelBorder
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
            font.pixelSize: 15
            font.bold: true
        }
    }

    // ---------- 内联组件：一张参数卡（标签左 + 输入框右） ----------
    component ParamRow: Rectangle {
        id: card
        required property string label
        required property string unit
        required property real paramValue
        required property int decimals
        property var onCommit: null     // 编辑提交回调 (newValue) => void

        Layout.fillWidth: true          // 平分列宽，填满所在列
        height: 46
        radius: Theme.panelRadius
        color: Theme.panel
        border.color: Theme.panelBorder

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            spacing: 10

            Text {
                Layout.preferredWidth: 170
                text: card.label
                color: Theme.textTitle
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
            // 弹性占位，把输入框推到卡片右侧（与设备卡同一工业排版）
            Item { Layout.fillWidth: true }
            TextField {
                id: input
                Layout.preferredWidth: 130
                height: 30
                Layout.alignment: Qt.AlignVCenter
                text: card.paramValue.toFixed(card.decimals)
                horizontalAlignment: Text.AlignHCenter
                color: Theme.textValue
                background: Rectangle {
                    color: "#0F3560"
                    radius: 4
                    border.color: Theme.panelBorder
                }
                onEditingFinished: {
                    if (card.onCommit !== null)
                        card.onCommit(Number(text))
                }
            }
            Text {
                text: card.unit
                color: Theme.textLabel
                font.pixelSize: 13
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: grid.implicitHeight + 16
        clip: true

        GridLayout {
            id: grid
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 8
            columns: 2
            columnSpacing: 12
            rowSpacing: 10

            Text {
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: "自控参数（存为配方，保存后下发至 PLC）"
                color: Theme.textLabel
                font.pixelSize: 13
            }

            // ---- 设定值（文档 1.7：供水温度 25-50℃ / 压力 0.2-0.8MPa） ----
            GroupHeader { text: "设定值" }
            ParamRow { label: "供水温度设定值";  unit: "℃";   paramValue: Backend.tempSetpoint;   decimals: 1; onCommit: function(v){ Backend.tempSetpoint = v } }
            ParamRow { label: "供水压力设定值";  unit: "MPa"; paramValue: Backend.pressSetpoint;  decimals: 2; onCommit: function(v){ Backend.pressSetpoint = v } }
            ParamRow { label: "冷却塔出水设定";  unit: "℃";   paramValue: Backend.coolingSetpoint; decimals: 1; onCommit: function(v){ Backend.coolingSetpoint = v } }

            // ---- 温度回路 PID（文档 6.4：PI 控制，PB 50-100%，Ti 60-300s） ----
            GroupHeader { text: "温度回路 PID（板换出口 / 冷却塔出水）" }
            ParamRow { label: "比例带 PB";   unit: "%"; paramValue: Backend.tempProportional; decimals: 0; onCommit: function(v){ Backend.tempProportional = v } }
            ParamRow { label: "积分时间 Ti"; unit: "s"; paramValue: Backend.tempIntegral;     decimals: 0; onCommit: function(v){ Backend.tempIntegral = v } }

            // ---- 压力回路 PID（文档 6.4：PB 30-60%，Ti 5-15s） ----
            GroupHeader { text: "压力回路 PID（供水泵恒压）" }
            ParamRow { label: "比例带 PB";   unit: "%"; paramValue: Backend.pressProportional; decimals: 0; onCommit: function(v){ Backend.pressProportional = v } }
            ParamRow { label: "积分时间 Ti"; unit: "s"; paramValue: Backend.pressIntegral;     decimals: 0; onCommit: function(v){ Backend.pressIntegral = v } }

            // ---- 报警阈值（文档 5.5） ----
            GroupHeader { text: "报警阈值" }
            ParamRow { label: "供水温度一级报警"; unit: "℃";   paramValue: Backend.tempAlarmL1; decimals: 0; onCommit: function(v){ Backend.tempAlarmL1 = v } }
            ParamRow { label: "供水温度二级停泵"; unit: "℃";   paramValue: Backend.tempAlarmL2; decimals: 0; onCommit: function(v){ Backend.tempAlarmL2 = v } }
            ParamRow { label: "供水压力低压报警"; unit: "MPa"; paramValue: Backend.pressLowAlarm; decimals: 2; onCommit: function(v){ Backend.pressLowAlarm = v } }
            ParamRow { label: "电导率上限";     unit: "μS/cm"; paramValue: Backend.conductivityLimit; decimals: 0; onCommit: function(v){ Backend.conductivityLimit = v } }

            // 保存按钮 + 提示（横跨两列）
            Row {
                Layout.columnSpan: 2
                spacing: 12
                Rectangle {
                    width: 120
                    height: 40
                    radius: 6
                    color: Theme.navActive
                    Text {
                        anchors.centerIn: parent
                        text: "保存参数"
                        color: "#FFFFFF"
                        font.pixelSize: 15
                        font.bold: true
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: Backend.saveParams()
                    }
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: Backend.saveMessage
                    color: Theme.textValue
                    font.pixelSize: 13
                }
            }
        }
    }
}