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
    property alias text: txtInternal.text
    property string color: "dark grey"

    height: txtInternal.height
    width: parent.width
    anchors.bottomMargin: 3
    anchors.topMargin: 3

    Rectangle {
        color: parent.color
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.left: parent.left
        width: parent.width - (txtInternal.visible ? (txtInternal.width + 30) : 0)
        height: 2
        anchors.verticalCenter: txtInternal.verticalCenter
    }

    Text {
        visible: text !== ""
        anchors.right: parent.right
        id: txtInternal
        color: parent.color
        font {
            pixelSize: 20
            //italic: true
        }
    }
}
