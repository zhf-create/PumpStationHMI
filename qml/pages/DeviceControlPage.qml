// DeviceControlPage.qml —— 设备控制画面
// 设备启停与手自动切换、远程启停需二次确认、
//          循环水泵组 N+1 多泵协同，多泵站/多泵切换查看。
// 知识点：
//   ListView + C++ QAbstractListModel（Backend.devices）：
//     模型在 C++、视图在 QML，点击按钮即调用模型的 Q_INVOKABLE 方法，
//     数据变化经 dataChanged 信号自动刷新界面，无需 QML 自己存数据。
//   泵组切换：顶部 FilterChip 条调用 Backend.devices.filter = 组名，
//     C++ 侧做分组过滤只返回该组设备，实现切换不同水泵。
//   行内用 RowLayout 撑满整行：设备名/状态在左，操作组被弹性占位
//     推到最右，避免所有栏目挤在左侧、右侧大片留白。
//   阀门（key 含 "Valve"）显示打开/关闭，泵/风机显示运行/停止。
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        //  顶部说明 
        Row {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            spacing: 16
            Text {
                text: "设备控制（共 " + Backend.devices.count + " 台）"
                color: Theme.textTitle
                font.pixelSize: Theme.fontMedium
                font.bold: true
            }
            Text {
                text: "远程启停需二次确认"
                color: Theme.textMuted
                font.pixelSize: Theme.fontLabel
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: Backend.loggedIn ? "已登录，可操作设备" : "未登录，设备操作已锁定（请从右上角登录）"
                color: Backend.loggedIn ? Theme.textValue : Theme.textMuted
                font.pixelSize: Theme.fontLabel
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        //  泵组筛选条（对应切换不同水泵） 
        Row {
            id: filterRow
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            spacing: 8

            FilterChip {
                text: "全部"
                active: Backend.devices.filter === ""
                onClicked: Backend.devices.filter = ""
            }
            // 遍历所有泵组名生成切换胶囊；点某个组就切换到该组设备。
            Repeater {
                model: Backend.devices.groups
                delegate: Item {
                    // 用 index 从 groups 取组名；外层用普通 delegate 保证 index 可解析
                    property string group: Backend.devices.groups[index]
                    width: chip.width
                    height: chip.height
                    FilterChip {
                        id: chip
                        text: group
                        active: Backend.devices.filter === group
                        onClicked: Backend.devices.filter = group
                    }
                }
            }
        }

        //  设备列表 
        ListView {
            id: deviceList
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            model: Backend.devices   // C++ 模型，角色：name/running/manual/key/group

            delegate: Rectangle {
                // 知识点：用 deviceList（id）取宽度，避免 ListView.view 未附着时为 null
                width: deviceList.width
                height: 56
                radius: Theme.panelRadius
                color: Theme.panel
                border.color: Theme.panelBorder

                // 阀门（key 含 "Valve"）用打开/关闭，泵/风机用运行/停止
                readonly property bool isValve: key.indexOf("Valve") >= 0
                // 循环泵才有模拟故障按钮（水泵故障切备用泵）
                readonly property bool isCyclePump: key.indexOf("cyclePump") >= 0

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    spacing: 16

                    // 设备名（来自 C++ 角色 name）
                    Text {
                        Layout.preferredWidth: 150
                        text: name
                        color: Theme.textTitle
                        font.pixelSize: Theme.fontMedium
                        font.bold: true
                    }

                    // 三色状态灯（running 角色）
                    StatusPill {
                        status: running ? "run" : "stop"
                        text: running ? (isValve ? "打开" : "运行") : (isValve ? "关闭" : "停止")
                    }

                    // 弹性占位：把右侧操作组推到最右
                    Item { Layout.fillWidth: true }

                    // 手动 / 自动（manual 角色）
                    Text { text: "自动"; color: Theme.textLabel; font.pixelSize: Theme.fontNav }
                    Switch {
                        checked: manual
                        enabled: Backend.loggedIn
                        // 知识点：加 if (checked !== manual) 防止checked 绑 manual、
                        // 又写回 manual形成双向绑定循环（QML 会报 Binding loop）。
                        onCheckedChanged: if (checked !== manual) Backend.devices.setManual(index, checked)
                    }
                    Text { text: "手动"; color: Theme.textLabel; font.pixelSize: Theme.fontNav }

                    // 启/停按钮
                    Button {
                        text: running ? (isValve ? "关闭" : "停止") : (isValve ? "打开" : "启动")
                        // 知识点：自动模式（manual=false）下禁用手动启停，只有切到“手动”
                        // 才能点；否则自动逻辑在控制该设备时用户又能手动点，产生冲突。
                        enabled: Backend.loggedIn && manual
                        contentItem: Text {
                            text: parent.text
                            color: "#FFFFFF"
                            font.pixelSize: Theme.fontNav
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: (Backend.loggedIn && manual) ? (running ? Theme.fault : Theme.run) : Theme.textMuted
                        }
                        // 知识点：控制动作下沉到 C++ 模型，便于统一加权限/二次确认
                        onClicked: Backend.devices.toggle(index)
                    }

                    // 模拟故障按钮（仅循环泵，水泵故障→切备用泵+报警）
                    Button {
                        visible: isCyclePump
                        text: "模拟故障"
                        enabled: Backend.loggedIn
                        contentItem: Text {
                            text: parent.text
                            color: "#FFFFFF"
                            font.pixelSize: Theme.fontLabel
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: Backend.loggedIn ? Theme.fault : Theme.textMuted
                        }
                        // 知识点：触发后端 Q_INVOKABLE，模拟故障并自动切备用泵
                        onClicked: Backend.simulatePumpFault(key)
                    }
                }
            }
        }
    }

    //  内联组件：泵组筛选胶囊 
    component FilterChip: Rectangle {
        required property string text
        required property bool active
        signal clicked()

        width: label.implicitWidth + 28
        height: 30
        radius: 15
        // 知识点：active=true 高亮为青色，false 为深蓝，直观显示当前选中的泵组
        color: active ? Theme.navActive : "#12365F"
        border.color: active ? Theme.navActive : Theme.panelBorder
        border.width: 1
        Text {
            id: label
            anchors.centerIn: parent
            text: parent.text
            color: active ? "#06203A" : Theme.textValue
            font.pixelSize: Theme.fontNav
            font.bold: active
        }
        MouseArea {
            anchors.fill: parent
            onClicked: parent.clicked()
        }
    }
}