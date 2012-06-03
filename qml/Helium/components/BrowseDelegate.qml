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

    property alias mainText: lblImage.mainText
    property alias subText: lblImage.subText
    property alias image: lblImage.image
    property alias drillDown: lblImage.drilldownVisisble
    property alias iconAnnotated: lblImage.annotated
    property alias iconVisible: lblImage.iconVisible
    property variant myModel: model

    signal clicked
    signal pressAndHold

    height: 88
    width: parent.width

    SelectionBackground {
        id: background
        visible: mouseArea.pressed
    }

    LabelImage {
        id: lblImage
        anchors.left: parent.left
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
