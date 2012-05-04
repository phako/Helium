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

import "components"

Item {
    id: listItem

    property alias mainText: lblMainText.text
    property alias subText: lblSubText.text
    property alias drillDown: imgDrilldown.visible
    property alias iconAnnotated: imgIcon.annotated
    property alias showIcon: imgIcon.visible

    signal clicked
    signal pressAndHold

    height: 88
    width: parent.width

    SelectionBackground {
        id: background
        visible: mouseArea.pressed
    }

    AnnotatedImage {
        id: imgIcon
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.top: parent.top
        anchors.leftMargin: 10
        source: icon
    }

    Column {
        anchors.left: imgIcon.visible ? imgIcon.right : parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: imgIcon.verticalCenter
        anchors.right: (imgDrilldown.visible ? imgDrilldown.left : listItem.right)
        anchors.rightMargin: 10

        Label {
            id: lblMainText
            width: parent.width
            font.weight: Font.Bold
            font.pixelSize: 26
            elide: Text.ElideRight
        }

        Label {
            visible: model.detail !== ""
            id: lblSubText
            width: parent.width
            font.weight: Font.Light
            font.pixelSize: 22
            color: "dark grey"
            elide: Text.ElideRight
        }
    }

    Image {
        id: imgDrilldown
        source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "")
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: background
        onClicked: {
            listItem.clicked()
        }

        onPressAndHold: {
            listItem.pressAndHold()
        }
    }
}
