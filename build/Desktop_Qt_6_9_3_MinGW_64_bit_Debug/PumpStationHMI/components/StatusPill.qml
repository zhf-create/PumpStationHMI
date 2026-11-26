// ============================================================
// StatusPill.qml —— 三色状态指示灯（文档 1.3 的三色区分）
// 复用价值：设备运行/停止/故障 状态在多个页面都需要展示，
// 抽成组件避免重复写「圆点 + 文字 + 颜色」三行代码。
//
// 用法示例：
//   StatusPill { status: "run";   text: "运行" }
//   StatusPill { status: "stop";  text: "停止" }
//   StatusPill { status: "fault"; text: "故障" }
// ============================================================
import QtQuick 2.15

Rectangle {
    id: root

    // 三个可绑定属性（对外接口）
    property string status: "run"     // run / stop / fault
    property string text: ""
    property bool blink: false        // 故障时闪烁（文档 7.3：紧急报警红色闪烁）

    // 宽度按内容自适应，高度固定 22
    width: dot.width + (text !== "" ? txt.implicitWidth + 10 : 0)
    height: 22
    radius: height / 2                 // 胶囊形
    color: "transparent"

    // 知识点：根据 status 字符串映射颜色。用三目表达式还是函数？
    // 三目嵌套可读性差，这里用一个函数集中映射，便于维护。
    function colorFor(s) {
        if (s === "run")   return Theme.run
        if (s === "fault") return Theme.fault
        return Theme.stop                // 默认 = 停止（灰）
    }

    // 状态小圆点
    Rectangle {
        id: dot
        anchors.verticalCenter: parent.verticalCenter
        width: 10
        height: 10
        radius: width / 2
        color: root.colorFor(root.status)

        // 知识点：故障 + blink 时，让圆点透明度随 Timer 周期跳变，
        // 模拟 HMI 上的"红色闪烁"报警提示。
        opacity: (root.status === "fault" && root.blink) ? blinkTimer.flag : 1.0

        Timer {
            id: blinkTimer
            // 闪烁数据由父级 opacity 通过 flag 读取，驱动重绘
            property int flag: 1
            interval: 500
            running: root.status === "fault" && root.blink
            repeat: true
            onTriggered: flag = (flag === 1 ? 0.2 : 1)
        }
    }

    Text {
        id: txt
        anchors.left: dot.right
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        text: root.text
        color: root.colorFor(root.status)
        font.pixelSize: Theme.fontLabel
    }
}