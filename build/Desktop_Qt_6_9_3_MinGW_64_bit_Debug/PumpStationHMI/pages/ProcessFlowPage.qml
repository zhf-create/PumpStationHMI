// ============================================================
// ProcessFlowPage.qml —— 工艺流程画面（主界面，对应文档 2 截图）
// 布局思路（知识点）：
//   1) 顶部「参数带」：核心 温度/压力/电导率/流量/液位 用 GridLayout
//      等宽平铺（每行 5 个、共 2 行），数值直接绑定 C++ 后端 Backend.xxx。
//      相比之前的 Flow（固定宽度、换行无对齐），网格更整齐、更工业化，
//      且 Layout.fillWidth 让卡片随窗口宽度均匀拉伸，不会挤在一起或超出。
//   2) 中部「工艺流程」：按水流方向 左→右 排布 冷却补水→换热→末端，
//      设备用三色状态灯区分 绿运行/灰停止/红故障（文档 1.3）。
//      每个工艺段的设备列包在 Flickable 里，窗口变小时可上下滚动，保证不溢出。
//   3) 所有数值都是「绑定」，后端 QTimer 一改，QML 自动刷新（响应式）。
// ============================================================
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // ---------------- 顶部参数带 ----------------
        GridLayout {
            Layout.fillWidth: true
            columns: 5
            columnSpacing: 8
            rowSpacing: 8

            // 温度组（文档 5.x：冷却塔/板换进出口/供水/回水）
            // 知识点：value 是字符串，double 属性需 toFixed(n) 控制小数位
            ParamTile { Layout.fillWidth: true; label: "供水温度";   value: Backend.supplyTemp.toFixed(2);         unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "回水温度";   value: Backend.returnTemp.toFixed(2);         unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "冷却塔出水"; value: Backend.coolingTowerOutlet.toFixed(2); unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "板换进口";   value: Backend.exchangerIn.toFixed(2);        unit: "℃" }
            ParamTile { Layout.fillWidth: true; label: "板换出口";   value: Backend.exchangerOut.toFixed(2);       unit: "℃" }

            // 压力组
            ParamTile { Layout.fillWidth: true; label: "供水压力"; value: Backend.supplyPressure.toFixed(3);  unit: "MPa" }
            ParamTile { Layout.fillWidth: true; label: "补水压力"; value: Backend.makeupPressure.toFixed(3);  unit: "MPa" }

            // 流量 / 电导率 / 液位
            ParamTile { Layout.fillWidth: true; label: "供水流量"; value: Backend.supplyFlow.toFixed(1);      unit: "m³/h" }
            // 电导率超限时后端置 conductivityAlarm，卡自动标红（文档 5.5）
            ParamTile { Layout.fillWidth: true; label: "电导率"; value: Backend.conductivity.toFixed(1); unit: "μS/cm"; alarm: Backend.conductivityAlarm }
            ParamTile { Layout.fillWidth: true; label: "水箱液位"; value: Backend.tankLevel.toFixed(3);     unit: "m" }
        }

        // ---------------- 工艺流程图 ----------------
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 6

            // ---- 工艺段一：冷却补水 ----
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 300
                radius: Theme.panelRadius
                color: "#0F3560"
                border.color: Theme.panelBorder

                Text {
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "冷却补水段"
                    color: Theme.textLabel
                    font.pixelSize: Theme.fontLabel
                }

                // 知识点：设备列放进 Flickable，内容超高时可滚动，避免溢出面板
                Flickable {
                    anchors.top: parent.top
                    anchors.topMargin: 34
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 6
                    clip: true
                    contentWidth: width
                    contentHeight: coolingCol.implicitHeight
                    Column {
                        id: coolingCol
                        width: parent.width
                        spacing: 8
                        // 知识点：三目表达式把 bool 状态转成字符串，驱动状态灯
                        EquipmentBox {
                            type: "tower"
                            name: "南冷却塔"
                            status: Backend.coolingFan1Running ? "run" : "stop"
                            detail: "出水 " + Backend.coolingTowerOutlet.toFixed(2) + " ℃\n风机 " +
                                    (Backend.coolingFan1Running ? "运行" : "停止")
                        }
                        EquipmentBox {
                            type: "pump"
                            name: "喷淋泵 #1"
                            status: Backend.sprayPump1Running ? "run" : "stop"
                            detail: ""
                        }
                        EquipmentBox {
                            type: "pump"
                            name: "补水泵 #1"
                            status: Backend.makeupPump1Running ? "run" : "stop"
                            detail: Backend.makeupPump1Current.toFixed(1) + " A"
                        }
                        EquipmentBox {
                            type: "pump"
                            name: "补水泵 #2"
                            status: Backend.makeupPump2Running ? "run" : "stop"
                            detail: Backend.makeupPump2Current.toFixed(1) + " A"
                        }
                        EquipmentBox {
                            type: "tank"
                            name: "西水箱"
                            status: "run"
                            detail: "液位 " + Backend.tankLevel.toFixed(3) + " m"
                        }
                        EquipmentBox {
                            type: "valve"
                            name: "补水电磁阀"
                            status: Backend.makeupValveOpen ? "run" : "stop"
                            detail: "补水压 " + Backend.makeupPressure.toFixed(3) + " MPa"
                        }
                    }
                }
            }

            // 箭头连接符：补水开启时流动（联动动画）
            FlowArrow {
                flowing: Backend.makeupValveOpen
            }

            // ---- 工艺段二：换热 ----
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 240
                radius: Theme.panelRadius
                color: "#0F3560"
                border.color: Theme.panelBorder

                Text {
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "换热段"
                    color: Theme.textLabel
                    font.pixelSize: Theme.fontLabel
                }

                Flickable {
                    anchors.top: parent.top
                    anchors.topMargin: 34
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 6
                    clip: true
                    contentWidth: width
                    contentHeight: heatCol.implicitHeight
                    Column {
                        id: heatCol
                        width: parent.width
                        spacing: 8
                        // 阀位 > 5% 视为投入运行（运行时自动跟随后端阀位值变化）
                        EquipmentBox {
                            type: "exchanger"
                            name: "板式换热器 #1"
                            status: Backend.valvePos1 > 5 ? "run" : "stop"
                            detail: "阀位 " + Backend.valvePos1.toFixed(1) + " %\n进 " +
                                    Backend.exchangerIn.toFixed(2) + " / 出 " + Backend.exchangerOut.toFixed(2) + " ℃"
                        }
                        EquipmentBox {
                            type: "exchanger"
                            name: "板式换热器 #2"
                            status: Backend.valvePos2 > 5 ? "run" : "stop"
                            detail: "阀位 " + Backend.valvePos2.toFixed(1) + " %"
                        }
                        EquipmentBox {
                            type: "pump"
                            name: "循环水泵 #1"
                            status: Backend.cyclePump1Running ? "run" : "stop"
                            detail: Backend.cyclePump1Current.toFixed(1) + " A"
                        }
                        EquipmentBox {
                            type: "pump"
                            name: "循环水泵 #2"
                            status: Backend.cyclePump2Running ? "run" : "stop"
                            detail: Backend.cyclePump2Current.toFixed(1) + " A"
                        }
                        EquipmentBox {
                            type: "pump"
                            name: "循环水泵 #3"
                            status: Backend.cyclePump3Running ? "run" : "stop"
                            detail: "备用（N+1）"
                        }
                    }
                }
            }

            // 箭头连接符：循环泵运行时供水流动（联动动画）
            FlowArrow {
                flowing: Backend.cyclePump1Running
            }

            // ---- 工艺段三：末端模具回路 ----
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                radius: Theme.panelRadius
                color: "#0F3560"
                border.color: Theme.panelBorder

                Text {
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "末端供水（模具回路）"
                    color: Theme.textLabel
                    font.pixelSize: Theme.fontLabel
                }

                Flickable {
                    anchors.top: parent.top
                    anchors.topMargin: 34
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 6
                    clip: true
                    contentWidth: width
                    contentHeight: circCol.implicitHeight
                    Column {
                        id: circCol
                        width: parent.width
                        spacing: 8
                        // 回路供/回压力取供水/回水总管压力近似，温度取供水温度
                        EquipmentBox {
                            type: "circuit"
                            name: "回路 A"
                            status: "run"
                            detail: "供 " + Backend.supplyPressure.toFixed(3) + " / 回 " +
                                    Backend.returnPressure.toFixed(3) + " MPa\n" + Backend.supplyTemp.toFixed(2) + " ℃"
                        }
                        EquipmentBox {
                            type: "circuit"
                            name: "回路 B"
                            status: "run"
                            detail: "供 " + Backend.supplyPressure.toFixed(3) + " / 回 " +
                                    Backend.returnPressure.toFixed(3) + " MPa\n" + Backend.supplyTemp.toFixed(2) + " ℃"
                        }
                        EquipmentBox {
                            type: "circuit"
                            name: "回路 C"
                            status: "run"
                            detail: "供 " + Backend.supplyPressure.toFixed(3) + " / 回 " +
                                    Backend.returnPressure.toFixed(3) + " MPa\n" + Backend.supplyTemp.toFixed(2) + " ℃"
                        }
                        EquipmentBox {
                            type: "circuit"
                            name: "回路 D"
                            status: "run"
                            detail: "供 " + Backend.supplyPressure.toFixed(3) + " / 回 " +
                                    Backend.returnPressure.toFixed(3) + " MPa\n" + Backend.supplyTemp.toFixed(2) + " ℃"
                        }
                    }
                }
            }
        }
    }

    // ---------- 内联组件：流动箭头（水管流动动画） ----------
    component FlowArrow: Text {
        // flowing=true 时透明度脉冲，模拟水在管内流动；
        // false 时半透明静止，表示该段管路没有水流通过。
        property bool flowing: false
        Layout.alignment: Qt.AlignVCenter
        text: "▶"
        color: Theme.navActive
        font.pixelSize: 26
        opacity: 0.45

        SequentialAnimation on opacity {
            running: flowing
            loops: Animation.Infinite
            NumberAnimation { to: 0.35; duration: 650; easing.type: Easing.InOutQuad }
            NumberAnimation { to: 1.0; duration: 650; easing.type: Easing.InOutQuad }
        }
    }
}