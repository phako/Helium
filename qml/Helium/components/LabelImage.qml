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
    property alias annotated: imgIcon.annotated
    property alias image: imgIcon.source
    property alias mainText: lblMainText.text
    property alias subText: lblSubText.text
    property alias drilldownVisisble: imgDrilldown.visible
    property alias iconVisible: imgIcon.visible

    width: parent.width
    anchors.verticalCenter: parent.verticalCenter

    AnnotatedImage {
        id: imgIcon
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors.left: imgIcon.visible ? imgIcon.right : parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: imgIcon.verticalCenter
        anchors.right: (imgDrilldown.visible ? imgDrilldown.left : parent.right)
        anchors.rightMargin: 10

        Label {
            id: lblMainText
            width: parent.width
            font.weight: Font.Bold
            font.pixelSize: 26
            elide: Text.ElideRight
        }

        Label {
            id: lblSubText
            visible: text !== ""
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
}
