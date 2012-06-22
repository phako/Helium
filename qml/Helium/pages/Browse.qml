/*
This file is part of Helium.

Helium is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Helium is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Helium.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 1.1
import com.nokia.meego 1.0
import org.jensge.UPnP 1.0

import "../components"

Page {
    property alias page: pageHeader.text

    function setUri(r) {
        var uri = browseListView.currentItem.myModel.uri;
        var metadata = browseListView.currentItem.myModel.metadata;
        r.setUriAndPlay(uri, metadata);
    }

    RendererSheet {
        id: rendererSheet
    }

    Menu {
        id: itemContextMenu
        visualParent: main.tabs.currentTab
        content: MenuLayout {
            MenuItem {
                text: qsTr("Play on current device")
                enabled: renderer.available
                onClicked: setUri(renderer)
            }

            MenuItem {
                Connections {
                    target: rendererSheet
                    onAccepted: setUri(rendererModel.getDevice(rendererSheet.currentIndex));
                }

                text: qsTr("Play on other device...")
                onClicked: {
                    rendererSheet.open();
                }
            }
        }
    }

    QueryDialog {
        id: dlgError
        property string errorMessage
        property int    errorId

        titleText: qsTr("Error")
        message: qsTr("Error accessing server \"%1\":\n%2\n(Error code %3)").arg(page).arg(errorMessage).arg(errorId)
        acceptButtonText: qsTr("Ok")
        onAccepted: {
            browseModelStack.pop();
            if (browseModelStack.empty()) {
                main.pageStackBrowse.pop();
            }
        }
    }

    Connections {
        target: server
        onUnavailable: {
            dlgServerLost.open()
        }

        onError: {
            dlgError.errorMessage=message;
            dlgError.errorId=code;
            dlgError.open();
        }
    }

    QueryDialog {
        id: dlgServerLost
        titleText: qsTr("Server gone")
        message: qsTr("Connection to server \"%1\" was lost").arg(page);
        acceptButtonText: qsTr("Ok")
        onAccepted: {
            browseModelStack.clear();
            main.pageStackBrowse.pop();
        }
    }

    Rectangle {
        id: searchEntryBack
        state: "disabled"
        color: "black"
        anchors.top: pageHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        z: 1

        SearchEntry {
            id: searchEntry
            anchors.centerIn: parent
            width: parent.width - 10
        }

        states : [
            State {
                name: "enabled"
                PropertyChanges { target: searchEntryBack; height: searchEntry.height + 10; opacity: 1.0; z: 1}
                AnchorChanges { target: listViewBack; anchors.top: searchEntryBack.bottom }
            },
            State {
                name: "disabled"
                PropertyChanges { target: searchEntryBack; height: 0; opacity: 0.0; z: pageHeader.z - 1}
                AnchorChanges { target: listViewBack; anchors.top: pageHeader.bottom }
            }
        ]

        transitions: Transition {
            ParallelAnimation {
                NumberAnimation { duration: 300; properties: "opacity,height" }
                AnchorAnimation {}
            }
        }
    }

    Rectangle {
        id: listViewBack
        color: "black"
        anchors.top: pageHeader.bottom
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
            cacheBuffer: parent.height + 100
            snapMode: ListView.SnapToItem
            model: browseModel
            currentIndex: browseModel.lastIndex
            highlightMoveSpeed: -1

            Timer {
                id: timer
                interval: 1000
                repeat: false
                running: false

                onTriggered: {
                    if (browseListView.contentY < 0) {
                        searchEntryBack.state = "enabled"
                    }
                }
            }

            onMovementStarted: {
                if (contentY <= 0) {
                    timer.start()
                }
            }

            // Clear search entry
            onModelChanged: {
                searchEntryBack.state = "disabled"
                searchEntry.text = ""
            }

            delegate: BrowseDelegate {
                mainText: model.title
                subText: model.detail
                image: model.icon
                iconAnnotated: model.uri === ""
                iconVisible: settings.displayMediaArt
                drillDown: model.type === "container"

                ActiveSelection {
                    id: selectedHighlight
                    visible: model.type !== "container" && browseModel.lastIndex === index
                    anchors.fill: parent
                }

                onClicked: {
                    browseModel.lastIndex = index
                    if (type === "container") {
                        server.browse(upnpId, upnpClass, renderer.protocolInfo);
                    }
                }

                onPressAndHold: {
                    feedback.start();
                    if (settings.showDevicePopUp) {
                        browseModel.lastIndex = index
                        if (type !== "container") {
                            if (itemContextMenu.status === DialogStatus.Closed) {
                                itemContextMenu.open();
                            } else {
                                itemContextMenu.close();
                            }
                        }
                    } else {
                        if (type !== "container" && uri !== "") {
                            renderer.setUriAndPlay(uri, metadata);
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
        z: 40
    }
}
