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

    BorderImage {
        id: background
        anchors.fill: parent
        // Fill page borders
        anchors.leftMargin: -listItem.ListView.view.anchors.leftMargin
        anchors.rightMargin: -listItem.ListView.view.anchors.rightMargin
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
    }

    Row {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        // make configurable
        Image {
            visible: false
            asynchronous: true
            source: icon
            width: 64
            height: 64
            fillMode: Image.PreserveAspectFit
            id: imgIcon
        }

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
        source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "")
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: background
        onClicked: {
            listItem.ListView.view.currentIndex = index;
            server.wrapDevice(serverModel.get(index));
            server.browse("0", "object.container", renderer.protocolInfo);
            browse.page = friendlyName
            pageStack.push(browse)
        }
    }
}
