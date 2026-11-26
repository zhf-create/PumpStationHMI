// ============================================================
// ParamTile.qml —— 参数显示卡片（标签 + 数值 + 单位）
// 复用价值：温度/压力/电导率/流量/液位等所有模拟量都用同一
// 弹窗样式，保持一致。
// 图中蓝色圆角面板 + 黄色大字体数值。
//
// 用法示例：
//   ParamTile { label: "供水温度"; value: "32.07"; unit: "℃" }
// ============================================================
import QtQuick 2.15

Rectangle {
    id: root

    // 对外属性
    property string label: ""            // 参数名
    property string value: "--"          // 数值（文本，便于保留小数位）
    property string unit: ""             // 单位
    property bool alarm: false           // 超限时数值变红（报警联动）

    width: 130
    height: Math.round(52 * Theme.uiScale)
    radius: Theme.panelRadius
    color: Theme.panel
    border.color: Theme.panelBorder
    border.width: 1

    Column {
        anchors.fill: parent
        anchors.margins: 5
        spacing: 2

        // 标签（浅蓝灰，小字）
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.label
            color: Theme.textLabel
            font.pixelSize: Theme.fontLabel
        }

        // 数值 + 单位（黄色大字体；报警时红色）
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.value + (root.unit !== "" ? " " + root.unit : "")
            color: root.alarm ? Theme.fault : Theme.textValue
            font.pixelSize: Theme.fontParam
            font.bold: true
        }
    }
}