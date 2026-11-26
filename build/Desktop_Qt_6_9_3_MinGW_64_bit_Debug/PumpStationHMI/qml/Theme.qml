// Theme.qml —— 全局配色/尺寸单例
// 知识点：
//   `pragma Singleton`：把这个文件声明为单例，配合 CMake 里的
//     QT_QML_SINGLETON_TYPE，全模块共享同一份实例。
//   为什么要抽单例：6 个页面 + 组件都要用同一套 DISEN 蓝风格、
//     三色状态色（绿=运行/灰=停止/红=故障）和字号。集中定义可以
//     一处改、处处生效。
pragma Singleton
import QtQuick

QtObject {
    //  背景（SCADA 风格蓝色渐变） 
    readonly property color bgTop: "#0A3D7A"       // 渐变顶：深蓝
    readonly property color bgBottom: "#1162B8"    // 渐变底：亮一点的蓝

    //  面板 / 卡片 
    readonly property color panel: "#143B6C"       // 参数面板底色（蓝）
    readonly property color panelBorder: "#2E6DA6" // 面板描边
    //  全局缩放 
    // uiScale 由 Main.qml 按窗口尺寸实时更新（基准 1280×800），
    // 字号/圆角/关键尺寸统一乘它，窗口缩放时内容等比自适应、不错位。
    property real uiScale: 1.0

    // 知识点：panelRadius 是圆角半径，数值语义，必须用 real/int，
    // 误写成 color 会在运行时报 “Invalid property assignment: color expected”。
    readonly property real panelRadius: Math.max(4, 8 * uiScale)

    //  三色状态（绿运行/灰停止/红故障） 
    readonly property color run: "#00C853"    // 运行（绿）
    readonly property color stop: "#7A8CA6"   // 停止（灰）
    readonly property color fault: "#FF3B30"  // 故障（红）

    //  文本 
    readonly property color textTitle: "#FFFFFF"   // 标题白
    readonly property color textValue: "#FFE082"   // 数值（黄色高亮，便于屏幕远观）
    readonly property color textLabel: "#AFC9E8"   // 标签浅蓝灰
    readonly property color textMuted: "#7F9BC0"   // 弱化文字

    //  导航条 
    readonly property color navActive: "#19C8E6"   // 当前页（青色高亮）
    readonly property color navIdle: "#24456F"     // 非当前页

    //  字号（均随 uiScale 等比缩放） 
    readonly property int fontTitle:  Math.round(22 * uiScale)  // 页面/标题
    readonly property int fontValue:  Math.round(20 * uiScale)  // 大数值
    readonly property int fontParam:  Math.round(18 * uiScale)  // 参数卡片数值
    readonly property int fontMedium: Math.round(16 * uiScale)  // LOGO/时间
    readonly property int fontBody:   Math.round(14 * uiScale)  // 正文
    readonly property int fontNav:    Math.round(13 * uiScale)  // 导航按钮
    readonly property int fontLabel:  Math.round(12 * uiScale)  // 标签
    readonly property int fontSmall:  Math.round(11 * uiScale)  // 次要文字
    readonly property int fontTiny:   Math.round(10 * uiScale)  // 流程图内小字
}