// Main.qml —— 主窗口
// 结构：
//   顶部  ：DISEN 标识 + 系统标题 + 日期时间
//   中部  ：StackLayout 托管 6 个页面，按底部导航切换
//   底部  ：6 个功能按钮（工艺流程/运行数据/设备控制/自控参数/报表/报警）
//   右下角：品牌水印 "DISEN保驾护航"
// 关键知识点：
//   ApplicationWindow：QML 顶层窗口，提供标题、菜单栏等外壳。
//   StackLayout：同一时刻只显示其中一个子项，切换 = 换 currentIndex。
//     相比 StackView 更轻量，适合"底部 Tab + 平级页面"的场景。
//   Repeater + ListModel：用数据驱动批量生成 6 个按钮，避免写 6 份。
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

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

    // 登录弹窗开关（由顶部登录按钮打开）
    property bool loginOpen: false

    // 知识点：登录提交 / 关闭抽成函数，供按钮与回车键复用。
    function closeLogin() {
        loginOpen = false
        userField.text = ""
        pwdField.text = ""
    }
    function submit() {
        if (Backend.login(userField.text, pwdField.text))
            closeLogin()
    }

    // 基准设计尺寸（1280×800），用于计算全局缩放系数
    readonly property real designW: 1280
    readonly property real designH: 800
    minimumWidth: 960
    minimumHeight: 600

    // 知识点：窗口尺寸变化时更新 Theme.uiScale，字号/圆角/尺寸统一等比缩放，
    // 实现内容自适应、不错位。缩放比取宽高较小值，并限制在 0.6~1.6 区间。
    function updateScale() {
        var s = Math.min(root.width / designW, root.height / designH);
        Theme.uiScale = Math.max(0.6, Math.min(1.6, s));
    }
    Component.onCompleted: updateScale()
    onWidthChanged: updateScale()
    onHeightChanged: updateScale()

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

    //  整体布局 
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        //  顶部标题栏 
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.round(64 * Theme.uiScale)
            radius: Theme.panelRadius
            color: "#0F3560"
            border.color: Theme.panelBorder

            // 知识点：这里用 RowLayout（而非 Row）是为了让中间的分隔项
            // 能通过 Layout.fillWidth 撑开，把时间推到最右端。
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 14

                // DISEN 品牌标识（图中左上角红白 LOGO）
                Rectangle {
                    width: Math.round(84 * Theme.uiScale)
                    height: Math.round(44 * Theme.uiScale)
                    radius: 6
                    color: "#E63946"
                    Column {
                        anchors.centerIn: parent
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "DISEN"
                            color: "#FFFFFF"
                            font.pixelSize: Theme.fontMedium
                            font.bold: true
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "戴森科技"
                            color: "#FFFFFF"
                            font.pixelSize: Theme.fontTiny
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

                // 登录状态指示 + 登录/退出入口（统一入口，各受限页面据此提示登录）
                Text {
                    Layout.alignment: Qt.AlignVCenter
                    text: Backend.loggedIn ? Backend.currentUser : "未登录"
                    color: Backend.loggedIn ? Theme.textValue : Theme.textMuted
                    font.pixelSize: Theme.fontNav
                }
                Rectangle {
                    Layout.alignment: Qt.AlignVCenter
                    width: Math.round(56 * Theme.uiScale)
                    height: Math.round(28 * Theme.uiScale)
                    radius: 6
                    color: Backend.loggedIn ? Theme.navIdle : Theme.navActive
                    Text {
                        anchors.centerIn: parent
                        text: Backend.loggedIn ? "退出" : "登录"
                        color: "#FFFFFF"
                        font.pixelSize: Theme.fontNav
                        font.bold: true
                    }
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (Backend.loggedIn) Backend.logout()
                            else root.loginOpen = true
                        }
                    }
                }

                // 日期时间（右上角，图中 2026-5-13 10:10:11）
                Text {
                    Layout.alignment: Qt.AlignVCenter
                    text: root.nowText
                    color: Theme.textValue
                    font.pixelSize: Theme.fontMedium
                }
            }
        }

        //  Modbus 未连接警告条（知识点） 
        // modbusConnected=false 时显示：提示当前为内部模拟数据兜底，避免操作员误以为是实时数据。
        // 连接成功(ConnectedState)后自动隐藏；visible=false 在 ColumnLayout 中不占高度，无空隙。
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.round(30 * Theme.uiScale)
            visible: !Backend.modbusConnected
            radius: Theme.panelRadius
            color: "#3A2A10"          // 暗琥珀底，与故障红区分（这是警告不是报警）
            border.color: "#E8A33D"
            border.width: 1

            Row {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "提醒"
                    color: "#E8A33D"
                    font.pixelSize: Theme.fontBody
                    font.bold: true
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Modbus 从站未连接，当前显示为内部模拟数据；请启动从站 " +
                          Backend.modbusHost + ":" + Backend.modbusPort + " 后自动接入实时数据源。"
                    color: "#F0C674"
                    font.pixelSize: Theme.fontBody
                }
            }
        }

        //  中部页面容器 
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

        //  底部导航栏 
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.round(56 * Theme.uiScale)
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

    //  品牌水印（右下角） 
    Text {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        text: qsTr("DISEN保驾护航")
        color: "#5A86B8"
        font.pixelSize: Theme.fontBody
        font.bold: true
        // 半透明水印，不干扰主界面阅读
        opacity: 0.6
    }

    //  登录弹窗（模态浮层） 
    // 知识点：全屏半透明遮罩 + 居中卡片，模拟模态对话框；
    // 登录成功才关闭，错误信息经 Backend.loginMessage 实时显示。
    Rectangle {
        id: loginOverlay
        z: 100
        anchors.fill: parent
        visible: root.loginOpen
        color: "#66000000"   // 半透明遮罩，聚焦弹窗、挡住下层交互

        // 遮罩层拦截点击，避免误触下层
        MouseArea { anchors.fill: parent }

        Rectangle {
            width: Math.round(320 * Theme.uiScale)
            height: Math.round(270 * Theme.uiScale)
            anchors.centerIn: parent
            radius: Theme.panelRadius
            color: Theme.panel
            border.color: Theme.panelBorder
            border.width: 1

            Column {
                anchors.fill: parent
                anchors.margins: Math.round(20 * Theme.uiScale)
                spacing: Math.round(12 * Theme.uiScale)

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "登录验证"
                    color: Theme.textTitle
                    font.pixelSize: Theme.fontTitle
                    font.bold: true
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "参数修改 / 设备操作需管理员登录"
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontLabel
                }

                TextField {
                    id: userField
                    width: parent.width
                    height: Math.round(34 * Theme.uiScale)
                    font.pixelSize: Theme.fontBody
                    placeholderText: "用户名"
                    color: Theme.textValue
                    placeholderTextColor: Theme.textMuted
                    background: Rectangle { color: "#0F3560"; radius: 4; border.color: Theme.panelBorder }
                }
                TextField {
                    id: pwdField
                    width: parent.width
                    height: Math.round(34 * Theme.uiScale)
                    font.pixelSize: Theme.fontBody
                    placeholderText: "密码"
                    echoMode: TextInput.Password
                    color: Theme.textValue
                    placeholderTextColor: Theme.textMuted
                    background: Rectangle { color: "#0F3560"; radius: 4; border.color: Theme.panelBorder }
                    onAccepted: submit()
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Backend.loginMessage
                    color: Theme.fault
                    font.pixelSize: Theme.fontLabel
                    visible: !Backend.loggedIn && Backend.loginMessage !== ""
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Math.round(12 * Theme.uiScale)
                    Rectangle {
                        width: Math.round(90 * Theme.uiScale); height: Math.round(34 * Theme.uiScale); radius: 6; color: Theme.navActive
                        Text { anchors.centerIn: parent; text: "登录"; color: "#FFFFFF"; font.pixelSize: Theme.fontMedium; font.bold: true }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: submit()
                        }
                    }
                    Rectangle {
                        width: Math.round(90 * Theme.uiScale); height: Math.round(34 * Theme.uiScale); radius: 6; color: Theme.navIdle
                        Text { anchors.centerIn: parent; text: "取消"; color: Theme.textTitle; font.pixelSize: Theme.fontMedium }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: closeLogin()
                        }
                    }
                }
            }
        }
    }
}