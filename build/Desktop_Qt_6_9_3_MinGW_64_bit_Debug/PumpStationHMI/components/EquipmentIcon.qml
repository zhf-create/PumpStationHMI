// ============================================================
// EquipmentIcon.qml —— 设备图形符号（矢量绘制 + 六类运行动画）
// 复用价值：工艺流程画面里每个设备都要一个「示意符号」辅助辨识，
// 抽成组件后 EquipmentBox 只传 type 就能显示对应图标。
//
// 知识点：用 QtQuick.Shapes 的 Shape/ShapePath/PathSvg 画矢量图，
//   - 不需要任何外部图片资源（SVG/PNG 都不用），零依赖；
//   - 路径用 SVG 语法（M/L/A/Q/Z 等指令）描述，缩放不失真；
//   - 全部走同一套描边色，风格统一。
//
// 为什么不用图片：图片要额外打包进 qrc、换图标要重新出图，
// 矢量用代码即可维护，且天然适配深蓝 SCADA 底色。
//
// 动画知识点（本组件为「全部做动画」的设计）：
//   - 每个设备类型在 running=true 时都有对应运行动画：
//       pump      —— 叶轮连续旋转（RotationAnimator，渲染线程，最省 CPU）
//       tower     —— 顶部风机旋转
//       exchanger —— 斜向流道虚线滚动（dashOffset 表达工质流动）
//       tank      —— 液位条上下起伏（表达水箱蓄/放水）
//       valve     —— 水平流道虚线滚动（电磁阀通断时介质流向）
//       circuit   —— 蛇形水道虚线滚动（模具回路内冷却水流动）
//   - RotationAnimator 在渲染线程执行，适合连续无限旋转；
//     dashOffset + dashPattern 是 Qt Quick Shapes 内建虚线流动方案。
//   - iconScale 允许外部把 40x24 图标整体放大，适配不同卡片尺寸。
//
// 用法示例：
//   EquipmentIcon { type: "pump"; running: true; iconScale: 1.6 }
// ============================================================
import QtQuick
import QtQuick.Shapes

Item {
    id: root

    // 图标可视缩放倍数（1.0 = 原始 40x24，>1 放大）
    property real iconScale: 1.0
    // 图标类型
    property string type: "pump"
    // 是否运行（驱动各类运行动画）
    property bool running: false

    // 统一描边色：用「水系统青色」，与红/绿/灰三色状态灯区分开，
    // 图标只表达「这是什么设备」，状态仍由 StatusPill 负责。
    readonly property color stroke: Theme.navActive

    // 外层尺寸随缩放系数变化；内部画布固定 40x24 设计，再用 scale 放大。
    width: 40 * root.iconScale
    height: 24 * root.iconScale

    // 知识点：ShapePath 不是 Item，没有 visible 属性，不能用可见性切换图标；
    // 这里把「外形」与「各类型动画元素」都放进固定 40x24 画布，
    // 各自按 type 控制 visible，再整体 scale 放大。
    Item {
        id: canvas
        width: 40
        height: 24
        anchors.centerIn: parent
        scale: root.iconScale

        // 按 type 返回外形 SVG 路径（不含动画元素）
        function strokePath() {
            switch (root.type) {
            case "pump":      // 水泵：进出口短管 + 泵体圆（两个半圆弧拼成整圆）
                return "M 3 12 L 13 12 M 27 12 L 37 12 " +
                       "M 20 5 A 7 7 0 1 1 20 19 A 7 7 0 1 1 20 5 Z"
            case "tower":     // 冷却塔：上窄下宽塔体 + 顶部风机轴 + 百叶
                return "M 14 5 L 26 5 L 33 20 L 7 20 Z " +
                       "M 20 2 L 20 5 M 11 10 L 29 10 M 9 15 L 31 15"
            case "exchanger": // 板式换热器：外框 + 接口短管 + 斜向流道 + 板片
                return "M 6 4 L 34 4 L 34 20 L 6 20 Z " +
                       "M 2 12 L 6 12 M 34 12 L 38 12 M 6 20 L 34 4 " +
                       "M 13 4 L 13 20 M 20 4 L 20 20 M 27 4 L 27 20"
            case "tank":      // 水箱：外框（水面/液位由动画元素绘制）
                return "M 10 4 L 30 4 L 30 20 L 10 20 Z"
            case "valve":     // 电磁阀：蝶形阀体 + 进出口短管 + 顶部线圈
                return "M 2 12 L 6 12 M 34 12 L 38 12 " +
                       "M 6 12 L 20 8 L 20 16 Z M 34 12 L 20 8 L 20 16 Z " +
                       "M 17 2 L 23 2 L 23 6 L 17 6 Z"
            case "circuit":   // 模具回路：上下模版 + 蛇形冷却水道
                return "M 8 3 L 32 3 L 32 8 L 8 8 Z " +
                       "M 8 16 L 32 16 L 32 21 L 8 21 Z " +
                       "M 8 12 L 12 12 L 16 9 L 20 15 L 24 9 L 28 15 L 32 12"
            }
            return ""
        }

        // 设备外形（线框式示意，所有类型统一）
        Shape {
            anchors.fill: parent
            ShapePath {
                strokeColor: root.stroke
                fillColor: "transparent"
                strokeWidth: 1.5
                capStyle: ShapePath.RoundCap
                joinStyle: ShapePath.RoundJoin
                PathSvg { path: canvas.strokePath() }
            }
        }

        // ---- 水泵叶轮：运行时匀速旋转，表达「正在抽水」 ----
        Item {
            id: impeller
            visible: root.type === "pump"
            width: 14
            height: 14
            x: 20 - width / 2
            y: 12 - height / 2
            Shape {
                anchors.fill: parent
                ShapePath {
                    strokeColor: root.stroke
                    strokeWidth: 2.2
                    capStyle: ShapePath.RoundCap
                    fillColor: "transparent"
                    PathSvg { path: "M 7 7 L 7 2 M 7 7 L 11.3 9.5 M 7 7 L 2.7 9.5" }
                }
            }
            RotationAnimator on rotation {
                running: root.running
                from: 0
                to: 360
                duration: 800
                loops: Animation.Infinite
            }
        }

        // ---- 冷却塔：顶部风机旋转（表达散热风机运转） ----
        Item {
            id: fan
            visible: root.type === "tower"
            width: 12
            height: 12
            x: 14
            y: -3
            Shape {
                anchors.fill: parent
                ShapePath {
                    strokeColor: root.stroke
                    strokeWidth: 1.6
                    capStyle: ShapePath.RoundCap
                    fillColor: "transparent"
                    PathSvg { path: "M 6 6 L 6 2 M 6 6 L 9.5 8 M 6 6 L 2.5 8" }
                }
            }
            RotationAnimator on rotation {
                running: root.running
                from: 0
                to: 360
                duration: 1000
                loops: Animation.Infinite
            }
        }

        // ---- 板式换热器：斜向流道虚线滚动（表达工质流动） ----
        Shape {
            visible: root.type === "exchanger"
            anchors.fill: parent
            ShapePath {
                strokeColor: root.stroke
                strokeWidth: 1.4
                capStyle: ShapePath.RoundCap
                fillColor: "transparent"
                dashPattern: [3, 5]
                PathSvg { path: "M 6 20 L 34 4" }
                // 虚线沿流道滚动（dashOffset 递增）即「流动」视觉
                NumberAnimation on dashOffset {
                    running: root.running
                    from: 0
                    to: 8
                    duration: 600
                    loops: Animation.Infinite
                }
            }
        }

        // ---- 水箱：液位条上下起伏（表达蓄/放水的水位变化） ----
        Item {
            id: tankWater
            visible: root.type === "tank"
            // 水位高度（自内底 y=20 向上，范围约 0~16）
            property real waterLevel: 14
            x: 10
            width: 20
            y: 20 - waterLevel
            height: waterLevel
            Rectangle {
                anchors.fill: parent
                color: Qt.rgba(0.28, 0.66, 0.92, 0.35)   // 半透明水蓝填充
            }
            SequentialAnimation on waterLevel {
                running: root.running
                loops: Animation.Infinite
                NumberAnimation { from: 13; to: 16; duration: 1500 }
                NumberAnimation { from: 16; to: 13; duration: 1500 }
            }
        }

        // ---- 电磁阀：水平流道虚线滚动（通断时介质流向） ----
        Shape {
            visible: root.type === "valve"
            anchors.fill: parent
            ShapePath {
                strokeColor: root.stroke
                strokeWidth: 1.4
                capStyle: ShapePath.RoundCap
                fillColor: "transparent"
                dashPattern: [3, 5]
                PathSvg { path: "M 2 12 L 38 12" }
                NumberAnimation on dashOffset {
                    running: root.running
                    from: 0
                    to: 8
                    duration: 600
                    loops: Animation.Infinite
                }
            }
        }

        // ---- 模具回路：蛇形水道虚线滚动（模具内冷却水流动） ----
        Shape {
            visible: root.type === "circuit"
            anchors.fill: parent
            ShapePath {
                strokeColor: root.stroke
                strokeWidth: 1.4
                capStyle: ShapePath.RoundCap
                fillColor: "transparent"
                dashPattern: [3, 5]
                PathSvg { path: "M 8 12 L 12 12 L 16 9 L 20 15 L 24 9 L 28 15 L 32 12" }
                NumberAnimation on dashOffset {
                    running: root.running
                    from: 0
                    to: 8
                    duration: 700
                    loops: Animation.Infinite
                }
            }
        }
    }
}