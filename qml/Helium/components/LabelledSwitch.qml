/*
    This file is part of TweakUp.

    Copyright (C) 2012 Jens Georg <mail+tweakup@jensge.org>

    TweakUp is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TweakUp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TweakUp.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    property alias checked: swInternalSwitch.checked
    property alias text: lblInternalLabel.text
    property bool indent: false

    height: Math.max(swInternalSwitch.height, lblInternalLabel.height)
    width: parent.width
    onWidthChanged: lblInternalLabel.width = width - swInternalSwitch.width - lblInternalLabel.anchors.leftMargin - 10

    Label {
        wrapMode: Text.WordWrap
        anchors.left: parent.left
        anchors.leftMargin: indent ? 40 : 10;
        anchors.right: swInternalSwitch.left
        anchors.rightMargin: 16
        id: lblInternalLabel
        font.pixelSize: 26
        font.bold: true
        color: enabled ? "white" : "dark grey"
    }

    Switch {
        id: swInternalSwitch
        anchors.rightMargin: 10
        anchors.right: parent.right
        anchors.verticalCenter: lblInternalLabel.verticalCenter
    }
}
