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

Item {
    id: listItem
    height: 88
    width: parent.width
    property real detailOpacity: 0.0

    ListView.onIsCurrentItemChanged: {
        if (!ListView.isCurrentItem) {
            state = "normal"
        }
    }

    SelectionBackground {
        id: background
        visible: mouseArea.pressed
    }

    AnnotatedImage {
        id: imgIcon
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10
        source: icon
        visible: settings.displayDeviceIcons
    }

    Label {
        id: mainText
        anchors.left: imgIcon.visible ? imgIcon.right : parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: imgIcon.verticalCenter
        anchors.right: tiClose.left
        anchors.rightMargin: 10
        elide: Text.ElideRight
        text: friendlyName
        font.weight: Font.Bold
        font.pixelSize: 26
    }

    MouseArea {
        id: mouseArea
        anchors.fill: background
        enabled: detailOpacity === 0.0
        onClicked: {
            listItem.state = "DetailedPlayer"
            listItem.ListView.view.currentIndex = index;
            renderer.wrapDevice(rendererModel.get(index));
        }
    }

    states: [
        State {
            name: "DetailedPlayer"
            AnchorChanges { target: mainText; anchors.top: listItem.top; anchors.verticalCenter: undefined }
            AnchorChanges { target: imgIcon; anchors.top: listItem.top; anchors.verticalCenter: undefined }
            PropertyChanges { target: listItem; height: 268; detailOpacity: 1.0 }
        },

        State {
            name: "normal"
            AnchorChanges { target: mainText; anchors.top: undefined; anchors.verticalCenter: listItem.verticalCenter }
            AnchorChanges { target: imgIcon; anchors.top: undefined; anchors.verticalCenter: listItem.verticalCenter }
            PropertyChanges { target: listItem; height: 88; detailOpacity: 0.0 }
        }
   ]

    ToolIcon {
        id: tiClose
        iconId: "toolbar-up"
        opacity: detailOpacity
        anchors.right: listItem.right
        anchors.verticalCenter: mainText.verticalCenter
        anchors.rightMargin: 10
        onClicked: {
            listItem.ListView.view.currentIndex = -1;
            listItem.state = "normal"
            renderer.wrapDevice("");
        }
    }

    Player {
        anchors.left: imgIcon.visible ? imgIcon.right : parent.left
        anchors.bottom: parent.bottom
        id: playerControls
        anchors.right: parent.right
        opacity: listItem.detailOpacity
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { duration: 300; properties: "detailOpacity,height" }
            AnchorAnimation {}
        }
    }

    Component.onCompleted: {
        listItem.state = "normal"
    }
}
