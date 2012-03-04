// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    property alias text: text.text
    property bool refreshable: true
    property alias busy: busyIndicator.visible

    signal clicked()

    id: title
    width: parent.width
    height: 70
    color: "#CC09BA"

    Label {
        id: text
        elide: Text.ElideRight
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 16
        platformStyle: LabelStyle {
            textColor: "white"
            fontPixelSize: 28
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: refreshButton
        running: visible
        style: BusyIndicatorStyle {
            size: "small"
        }
    }

    ToolIcon {
        visible: refreshable && !busyIndicator.running
        id: refreshButton
        platformIconId: "icon-m-common-refresh"
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        onClicked: title.clicked()
    }
}
