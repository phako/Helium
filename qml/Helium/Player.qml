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

    ProgressBar {
        id: progressBar
        value: renderer.progress
        width: parent.width
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        anchors.bottom: times.top
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        MouseArea {
            anchors.fill: parent
            onReleased: {
                renderer.seekRelative(mouseX / width)
            }
        }
    }
    Item {
        id: times
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        height: 30
        Text {
            text: renderer.position
            color: "white"
            anchors.left: parent.left
            font.pixelSize: 16
        }
        Text {
            text: renderer.duration
            color: "white"
            anchors.right: parent.right
            font.pixelSize: 16
        }
    }

    Row {
        id: buttons
        anchors.bottom: progressBar.top
        anchors.horizontalCenter: parent.horizontalCenter
        ToolIcon {
            enabled: renderer.state !== "PLAYING" || (renderer.state === "PLAYING" && renderer.canPause)
            id: btnPlayPause
            iconSource: "image://theme/icon-m-toolbar-mediacontrol-play" + (enabled ? "-white" : "")
            onClicked: {
                if (renderer.state === "PLAYING") {
                    renderer.pause();
                } else {
                    renderer.play();
                }
            }
        }

        ToolIcon {
            enabled: renderer.state !== "STOPPED"
            id: btnStop
            iconSource: "image://theme/icon-m-toolbar-mediacontrol-stop" + (enabled ? "-white" : "")
            onClicked: {
                renderer.stop();
            }
        }

        Connections {
            target: renderer
            onStateChanged: {
                if (renderer.state === "PLAYING" && renderer.canPause) {
                    btnPlayPause.iconSource = "image://theme/icon-m-toolbar-mediacontrol-pause" + (enabled ? "-white" : "");
                } else {
                    btnPlayPause.iconSource = "image://theme/icon-m-toolbar-mediacontrol-play" + (enabled ? "-white" : "");
                }
            }

            onCanPauseChanged: {
                if (renderer.state === "PLAYING" && renderer.canPause) {
                    btnPlayPause.iconSource = "image://theme/icon-m-toolbar-mediacontrol-pause" + (enabled ? "" : "-white");
                } else {
                    btnPlayPause.iconSource = "image://theme/icon-m-toolbar-mediacontrol-play" + (enabled ? "" : "-white");
                }
            }
        }
    }
}
