// ============================================================
// EquipmentBox.qml —— 工艺流程中的设备图块（简易 3D 感卡片）
// 复用价值：工艺流程画面里 冷却塔/泵/板换/水箱/电磁阀/模具回路
// 都是「名称 + 三色状态 + 关键参数」的同一结构，抽成组件；
// 对应图中左上冷却补水、中部换热、右侧末端回路的设备图元。
//
// 用法示例：
//   EquipmentBox { name: "供水泵"; status: "run"; detail: "40.4 A" }
// ============================================================
import QtQuick 2.15

Rectangle {
    id: root

    property string name: ""      // 设备名
    property string type: "pump"  // 设备类型 -> 决定图标（pump/tower/exchanger/tank/valve/circuit）
    property string status: "run" // run / stop / fault
    property string detail: ""    // 关键参数（支持 \n 换行）
    property bool alarm: false    // 参数超限时 detail 变红

    // 知识点：不写死 120，改为随父容器（工艺段 Column）宽度拉伸，
    // 让设备卡与所在工艺段面板同宽，避免卡片挤在左侧、右侧大片留白；
    // 内部 Column 已把图形/名称/参数水平居中，拉伸后依然整齐。
    width: parent.width
    height: detail !== "" ? 116 : 82
    radius: Theme.panelRadius
    color: "#12365F"                 // 比面板略深的设备底色
    border.color: root.status === "fault" ? Theme.fault : Theme.panelBorder
    border.width: 1

    Column {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 3

        // 设备图形符号（按 type 绘制）
        EquipmentIcon {
            anchors.horizontalCenter: parent.horizontalCenter
            type: root.type
            running: root.status === "run"   // 运行时驱动对应类型动画
            iconScale: 1.6                    // 放大图标，与卡片尺寸更协调
        }

        // 名称 + 状态灯同一行
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 6
            Text {
                text: root.name
                color: Theme.textTitle
                font.pixelSize: 13
                font.bold: true
            }
            StatusPill { status: root.status; text: "" }
        }

        // 参数文本（可能多行）
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.detail
            color: root.alarm ? Theme.fault : Theme.textValue
            font.pixelSize: 11
            horizontalAlignment: Text.AlignHCenter
            visible: root.detail !== ""
        }
    }
}