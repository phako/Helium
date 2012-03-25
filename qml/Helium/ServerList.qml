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

Page {
    property alias model: deviceListView.model
    property string role
    property alias title: pageHeader.text
    property alias delegate: deviceListView.delegate

    Connections {
        target: renderer
        onUnavailable: {
            if (role !== "server") {
                dlgPlayerLost.open()
            }
        }
    }

    QueryDialog {
        id: dlgPlayerLost
        titleText: qsTr("Player gone")
        message: qsTr("Connection to current player was lost")
        acceptButtonText: qsTr("Ok")
        onAccepted: {
            deviceListView.currentIndex = -1
        }
    }

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
            highlightMoveSpeed: -1
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
