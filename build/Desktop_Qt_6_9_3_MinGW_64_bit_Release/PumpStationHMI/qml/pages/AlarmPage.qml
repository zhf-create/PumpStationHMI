// ============================================================
// AlarmPage.qml —— 报警信息查询
// 报警分级：一级紧急/二级重要/三级一般。
//
// 知识点：
//   - ListView 直接绑 C++ AlarmModel（Backend.alarms），角色 level/time/type/alarmState。
//   - 状态列按 alarmState 着色（未确认=红 / 已确认=绿 / 已消除=灰），一眼可辨。
//   - 知识点：角色名若叫 “state” 会与 QML 每个 Item 自带的 state 属性冲突
//     （内置属性优先，角色值取不到），所以后端把角色改名为 alarmState，
//     这里读 alarmState，状态/操作列才能正常显示并驱动按钮。
//   - 操作列提供「确认」「复位」两个动作：未确认→确认、已确认→复位（消除），
//     均调用后端 Q_INVOKABLE，后端改状态 + dataChanged 信号驱动刷新。
//   - 顶部「未确认 N 条」绑定 unackedCount，N>0 时红色高亮（联动提示）。
// ============================================================
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Backend 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // ---- 顶部说明 + 未确认统计 ----
        Row {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            spacing: 16
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "报警信息（红=一级紧急 橙=二级重要 黄=三级一般）"
                color: Theme.textLabel
                font.pixelSize: Theme.fontLabel
            }
            Item { width: 16; height: 1 }   // 间隔
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "未确认 " + Backend.alarms.unackedCount + " 条"
                color: Backend.alarms.unackedCount > 0 ? Theme.fault : Theme.textMuted
                font.pixelSize: Theme.fontNav
                font.bold: Backend.alarms.unackedCount > 0
            }
        }

        // ---- 表头 ----
        Row {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            AlarmCell { text: "级别";   widthRatio: 0.12; header: true }
            AlarmCell { text: "发生时间"; widthRatio: 0.18; header: true }
            AlarmCell { text: "报警类型"; widthRatio: 0.38; header: true }
            AlarmCell { text: "状态";   widthRatio: 0.16; header: true }
            AlarmCell { text: "操作";   widthRatio: 0.16; header: true }
        }

        // ---- 报警列表 ----
        ListView {
            id: alarmList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 3
            model: Backend.alarms

            delegate: Row {
                width: alarmList.width
                height: 34

                // 级别（彩色小标签）
                Rectangle {
                    width: alarmList.width * 0.12
                    height: parent.height
                    color: "#12365F"
                    border.color: Theme.panelBorder; border.width: 1
                    Rectangle {
                        anchors.centerIn: parent
                        width: 56; height: 20; radius: 3
                        color: levelColor(level)
                        // 未确认报警：级别色块红/橙闪烁提醒
                        SequentialAnimation on opacity {
                            running: alarmState === "未确认"
                            loops: Animation.Infinite
                            NumberAnimation { from: 1.0; to: 0.35; duration: 500 }
                            NumberAnimation { from: 0.35; to: 1.0; duration: 500 }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: levelName(level)
                            color: "#FFFFFF"
                            font.pixelSize: Theme.fontSmall
                            font.bold: true
                        }
                    }
                }
                AlarmCell { text: time;  widthRatio: 0.18 }
                AlarmCell { text: type;  widthRatio: 0.38 }
                AlarmCell {
                    text: alarmState
                    widthRatio: 0.16
                    cellTextColor: stateColor(alarmState)
                }
                // 操作：未确认→确认；已确认→复位；已消除→无操作
                Rectangle {
                    width: parent.width * 0.16
                    height: parent.height
                    color: "#12365F"
                    border.color: Theme.panelBorder; border.width: 1
                    Row {
                        anchors.centerIn: parent
                        spacing: 4
                        Rectangle {
                            visible: alarmState === "未确认"
                            width: 44; height: 20; radius: 3
                            color: Theme.run
                            Text { anchors.centerIn: parent; text: "确认"; color: "#FFFFFF"; font.pixelSize: Theme.fontLabel }
                            MouseArea { anchors.fill: parent; onClicked: Backend.alarms.acknowledge(index) }
                        }
                        Rectangle {
                            visible: alarmState === "已确认"
                            width: 44; height: 20; radius: 3
                            color: Theme.navActive
                            Text { anchors.centerIn: parent; text: "复位"; color: "#06203A"; font.pixelSize: Theme.fontLabel; font.bold: true }
                            MouseArea { anchors.fill: parent; onClicked: Backend.alarms.reset(index) }
                        }
                        Text {
                            visible: alarmState === "已消除"
                            text: "—"
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontNav
                        }
                    }
                }
            }
        }
    }

    function levelName(l) { return l === 1 ? "一级" : (l === 2 ? "二级" : "三级") }
    function levelColor(l) { return l === 1 ? Theme.fault : (l === 2 ? "#FF8A00" : "#FFD54F") }
    function stateColor(s) { return s === "未确认" ? Theme.fault : (s === "已确认" ? Theme.run : Theme.textMuted) }

    // ---------- 内联组件：报警单元格 ----------
    component AlarmCell: Rectangle {
        required property string text
        required property double widthRatio
        property bool header: false
        property color cellTextColor: Theme.textValue

        width: parent.width * widthRatio
        height: parent.height
        color: header ? "#1B4E7E" : "#12365F"
        border.color: Theme.panelBorder
        border.width: 1
        Text {
            anchors.fill: parent
            anchors.margins: 4
            text: parent.text
            color: parent.header ? Theme.textTitle : parent.cellTextColor
            font.pixelSize: Theme.fontNav
            font.bold: parent.header
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}