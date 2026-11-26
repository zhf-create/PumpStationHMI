// ============================================================
// Theme.qml —— 全局配色/尺寸单例
// 知识点：
//   - `pragma Singleton`：把这个文件声明为「单例」，配合 CMake 里的
//     QT_QML_SINGLETON_TYPE，全模块共享同一份实例。
//   - 为什么要抽单例：6 个页面 + 组件都要用同一套 DISEN 蓝风格、
//     三色状态色（绿=运行/灰=停止/红=故障）和字号。集中定义可以
//     一处改、处处生效，符合文档「设计风格统一」的要求。
// ============================================================
pragma Singleton
import QtQuick

QtObject {
    // ---------- 背景（SCADA 风格蓝色渐变） ----------
    readonly property color bgTop: "#0A3D7A"       // 渐变顶：深蓝
    readonly property color bgBottom: "#1162B8"    // 渐变底：亮一点的蓝

    // ---------- 面板 / 卡片 ----------
    readonly property color panel: "#143B6C"       // 参数面板底色（蓝）
    readonly property color panelBorder: "#2E6DA6" // 面板描边
    // 知识点：panelRadius 是「圆角半径」，数值语义，必须用 real/int，
    // 误写成 color 会在运行时报 “Invalid property assignment: color expected”。
    readonly property real panelRadius: 8

    // ---------- 三色状态（文档 1.3：绿运行/灰停止/红故障） ----------
    readonly property color run: "#00C853"    // 运行（绿）
    readonly property color stop: "#7A8CA6"   // 停止（灰）
    readonly property color fault: "#FF3B30"  // 故障（红）

    // ---------- 文本 ----------
    readonly property color textTitle: "#FFFFFF"   // 标题白
    readonly property color textValue: "#FFE082"   // 数值（黄色高亮，便于屏幕远观）
    readonly property color textLabel: "#AFC9E8"   // 标签浅蓝灰
    readonly property color textMuted: "#7F9BC0"   // 弱化文字

    // ---------- 导航条 ----------
    readonly property color navActive: "#19C8E6"   // 当前页（青色高亮）
    readonly property color navIdle: "#24456F"     // 非当前页

    // ---------- 字号 ----------
    readonly property int fontTitle: 22
    readonly property int fontLabel: 12
    readonly property int fontValue: 20
    readonly property int fontNav: 13
}