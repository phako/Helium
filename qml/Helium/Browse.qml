// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import org.jensge.UPnP 1.0

Page {
    property alias page: pageHeader.text

    Rectangle {
        color: "black"
        anchors.top: pageHeader.bottom
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        BusyIndicator {
            visible: browseModel.busy
            running: browseModel.busy
            platformStyle: BusyIndicatorStyle {
                size: "large"
            }
            anchors.centerIn: parent
        }

        ListView {
            visible: !browseModel.busy
            id: browseListView
            width: parent.width
            height: parent.height
            spacing: 15
            cacheBuffer: parent.height + 100
            snapMode: ListView.SnapToItem
            model: browseModel

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
                    spacing: 10

                    Image {
                        asynchronous: true
                        source: icon
                        width: 64
                        height: 64
                        fillMode: Image.PreserveAspectFit
                        id: imgIcon
                    }

                    Column {
                        anchors.verticalCenter: imgIcon.verticalCenter

                        Label {
                            id: mainText
                            text: model.title
                            font.weight: Font.Bold
                            font.pixelSize: 26
                            elide: Text.ElideRight
                        }

                        Label {
                            //visible: model.type === "container";
                            id: subText
                            text: model.detail
                            font.weight: Font.Light
                            font.pixelSize: 22
                            color: "#cc6633"
                        }
                    }
                }

                Image {
                    visible: type === "container"
                    source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "")
                    anchors.right: parent.right;
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: background
                    onClicked: {
                        if (type === "container") {
                            server.browse(upnpId, upnpClass);
                        }
                    }

                    onPressAndHold: {
                        if (type !== "container") {
                            renderer.setAVTransportUri(uri);
                            if (renderer.state === "STOPPED") {
                                renderer.play();
                            }
                        }
                    }
                }
            }
        }
        ScrollDecorator {
            flickableItem: browseListView
        }
    }

    PageHeader {
        id: pageHeader
        busy: !browseModel.busy && !browseModel.done
        onClicked: browseModel.refresh()
    }
}
