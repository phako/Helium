// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    property alias model: deviceListView.model
    property string role
    property alias title: pageHeader.text

    Rectangle {
        color: "black"
        anchors.top: pageHeader.bottom
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Component {
            id: selectedHighlight
            Rectangle {
                visible: role === "renderer"
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#1078d8" }
                    GradientStop {position: 1.0; color: "#3890e0" }
                }
            }
        }

        ListView {
            Component.onCompleted: currentIndex = -1
            id: deviceListView
            width: parent.width
            height: parent.height
            spacing: 15
            cacheBuffer: parent.height + 100
            snapMode: ListView.SnapToItem
            model : serverModel
            highlight: selectedHighlight

            delegate:  Item {
                id: listItem
                height: 88
                width: parent.width

                BorderImage {
                    id: background
                    anchors.fill: parent
                    // Fill page borders
                    anchors.leftMargin: -serverList.anchors.leftMargin
                    anchors.rightMargin: -serverList.anchors.rightMargin
                    visible: mouseArea.pressed
                    source: "image://theme/meegotouch-list-background-pressed-center"
                }

                Row {
                    anchors.fill: parent

                    Column {
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            id: mainText
                            text: friendlyName
                            font.weight: Font.Bold
                            font.pixelSize: 26
                        }

                        Label {
                            id: subText
                            text: udn
                            font.weight: Font.Light
                            font.pixelSize: 22
                            color: "#cc6633"

                            visible: text != ""
                        }
                    }
                }

                Image {
                    visible: role === "server"
                    source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "")
                    anchors.right: parent.right;
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: background
                    onClicked: {
                        deviceListView.currentIndex = index;
                        if (role === "server") {
                            server.wrapDevice(serverModel.get(index));
                            server.browse("0");
                            browse.page = friendlyName
                            pageStack.push(browse)
                        } else {
                            renderer.wrapDevice(rendererModel.get(index));
                            pageStack.push(player)
                        }
                    }

                    onPressAndHold: renderer.stop();
                }
            }
        }
        ScrollDecorator {
            flickableItem: deviceListView
        }
    }

    PageHeader {
        id: pageHeader
        busy: false
        onClicked: model.refresh()
    }
}
