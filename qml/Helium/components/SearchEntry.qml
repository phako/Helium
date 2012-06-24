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

TextField {
    id: cmpSearchEntry
    placeholderText: qsTr("Filter...")
    platformStyle: TextFieldStyle { paddingRight: button.width }
    onTextChanged: updateIcon()
    inputMethodHints: Qt.ImhNoPredictiveText

    function updateIcon() {
        if (text === "") {
            button.source = "image://theme/icon-m-common-search"
        } else {
            button.source = "image://theme/icon-m-input-clear"
        }
    }

    Image {
        id: button
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        source: "image://theme/icon-m-common-search"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                text = ""
            }
        }
    }
}
