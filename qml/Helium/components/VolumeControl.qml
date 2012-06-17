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
    height: muteButton.height
    ImageToggleButton {
        id: muteButton
        checked: renderer.mute
        enabled: renderer.canMute
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        offImage: "image://theme/icon-m-toolbar-volume-white"
        onImage: "image://theme/icon-m-toolbar-volume-off-white"
        width: 64
        height: 64
        onCheckedChanged: renderer.mute = checked
    }

    Slider {
        enabled: renderer.canVolume
        minimumValue: 0
        maximumValue: renderer.maxVolume
        value: renderer.volume
        stepSize: 1
        anchors.left: muteButton.right
        anchors.right: parent.right
        anchors.verticalCenter: muteButton.verticalCenter
        anchors.rightMargin: -10
        onValueChanged: renderer.volume = value
    }
}
