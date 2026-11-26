// ============================================================
// Main.qml —— 主窗口
// 结构（对应文档 2「界面参考」截图与 1.3「设计风格」）：
//   顶部  ：DISEN 标识 + 系统标题 + 日期时间
//   中部  ：StackLayout 托管 6 个页面，按底部导航切换
//   底部  ：6 个功能按钮（工艺流程/运行数据/设备控制/自控参数/报表/报警）
//   右下角：品牌水印 "DISEN保驾护航"
//
// 关键知识点：
//   - ApplicationWindow：QML 顶层窗口，提供标题、菜单栏等外壳。
//   - StackLayout：同一时刻只显示其中一个子项，切换 = 换 currentIndex。
//     相比 StackView 更轻量，适合"底部 Tab + 平级页面"的场景。
//   - Repeater + ListModel：用数据驱动批量生成 6 个按钮，避免写 6 份。
// ============================================================
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    width: 1280
    height: 800
    visible: true
    title: qsTr("二号泵站模具冷却供水控制系统")

    // 背景：SCADA 蓝色渐变（Theme 定义）
    // 知识点：Rectangular 的 gradient 实现垂直渐变，比纯色更像工业 HMI。
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.bgTop }
            GradientStop { position: 1.0; color: Theme.bgBottom }
        }
    }

    // 当前页索引，由底部导航按钮更新
    property int currentIndex: 0

    // 六个页面的标题，供顶部栏与按钮复用
    readonly property var pageTitles: [
        "工艺流程画面", "运行数据画面", "设备控制画面",
        "自控参数设定", "数据报表查询", "报警信息查询"
    ]

    // 顶部时间（1 秒刷新一次）
    property string nowText: Qt.formatDateTime(new Date(), "yyyy-M-d hh:mm:ss")
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: root.nowText = Qt.formatDateTime(new Date(), "yyyy-M-d hh:mm:ss")
    }

    // ---------------- 整体布局 ----------------
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        // ---------------- 顶部标题栏 ----------------
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            radius: Theme.panelRadius
            color: "#0F3560"
            border.color: Theme.panelBorder

            // 知识点：这里用 RowLayout（而非 Row）是为了让中间的分隔项
            // 能通过 Layout.fillWidth 撑开，把时间推到最右端。
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 14

                // DISEN 品牌标识（文档 1.9 / 图中左上角红白 LOGO）
                Rectangle {
                    width: 84
                    height: 44
                    radius: 6
                    color: "#E63946"
                    Column {
                        anchors.centerIn: parent
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "DISEN"
                            color: "#FFFFFF"
                            font.pixelSize: 16
                            font.bold: true
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "戴森科技"
                            color: "#FFFFFF"
                            font.pixelSize: 10
                        }
                    }
                }

                // 系统标题（Layout.alignment 让其在 RowLayout 内垂直居中）
                Text {
                    Layout.alignment: Qt.AlignVCenter
                    text: qsTr("二号泵站模具冷却供水控制系统")
                    color: Theme.textTitle
                    font.pixelSize: Theme.fontTitle
                    font.bold: true
                }

                // 占位，把右侧时间推到最右
                Item { Layout.fillWidth: true; width: 1 }

                // 日期时间（右上角，图中 2026-5-13 10:10:11）
                Text {
                    Layout.alignment: Qt.AlignVCenter
                    text: root.nowText
                    color: Theme.textValue
                    font.pixelSize: 16
                }
            }
        }

        // ---------------- 中部页面容器 ----------------
        StackLayout {
            id: pageStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: root.currentIndex

            // 6 个页面（按文件名引用，模块内已自动注册为类型）
            ProcessFlowPage {}
            OperationDataPage {}
            DeviceControlPage {}
            AutoParamPage {}
            ReportPage {}
            AlarmPage {}
        }

        // ---------------- 底部导航栏 ----------------
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            radius: Theme.panelRadius
            color: "#0F3560"
            border.color: Theme.panelBorder

            Row {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 6

                // 知识点：Repeater 按 model 数量生成 6 个按钮；
                // 当前页按钮青色(Theme.navActive)，其余深蓝(Theme.navIdle)。
                Repeater {
                    model: root.pageTitles
                    delegate: Rectangle {
                        // modelData = 当前按钮标题字符串
                        required property string modelData
                        required property int index

                        width: (parent.width - parent.spacing * 5) / 6
                        height: parent.height
                        radius: 6
                        color: (root.currentIndex === index) ? Theme.navActive : Theme.navIdle

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            color: (root.currentIndex === index) ? "#FFFFFF" : Theme.textLabel
                            font.pixelSize: Theme.fontNav
                            font.bold: root.currentIndex === index
                        }

                        // 知识点：MouseArea 放在最上层拦截点击，绑定到 currentIndex
                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.currentIndex = index
                        }
                    }
                }
            }
        }
    }

    // ---------------- 品牌水印（右下角） ----------------
    Text {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        text: qsTr("DISEN保驾护航")
        color: "#5A86B8"
        font.pixelSize: 14
        font.bold: true
        // 半透明水印，不干扰主界面阅读
        opacity: 0.6
    }
}