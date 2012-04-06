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
        anchors.right: refreshable ? refreshButton.left : parent.right
        anchors.rightMargin: 16
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
