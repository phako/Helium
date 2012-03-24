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
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
    }

    AnnotatedImage {
        id: imgIcon
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.top: parent.top
        anchors.leftMargin: 10
        annotated: uri === ""
        asynchronous: true
        source: icon
        width: 64
        height: 64
        fillMode: Image.PreserveAspectFit
    }

    Column {
        anchors.left: imgIcon.right
        anchors.leftMargin: 10
        anchors.verticalCenter: imgIcon.verticalCenter
        anchors.right: (imgDrilldown.visible ? imgDrilldown.left : listItem.right)
        anchors.rightMargin: 10

        Label {
            id: mainText
            width: parent.width
            text: model.title
            font.weight: Font.Bold
            font.pixelSize: 26
            elide: Text.ElideRight
        }

        Label {
            visible: model.detail !== ""
            id: subText
            width: parent.width
            text: model.detail
            font.weight: Font.Light
            font.pixelSize: 22
            color: "dark grey"
            elide: Text.ElideRight
        }
    }

    Image {
        id: imgDrilldown
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
                server.browse(upnpId, upnpClass, renderer.protocolInfo);
            }
        }

        onPressAndHold: {
            if (type !== "container" && uri !== "") {
                renderer.setAVTransportUri(uri, metadata);
                if (renderer.state === "STOPPED") {
                    renderer.play();
                }
            }
        }
    }
}
