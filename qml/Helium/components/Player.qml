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
    anchors.leftMargin: 16
    anchors.rightMargin: 16

    function updatePlayButton() {
        if (renderer.state === "PLAYING" && renderer.canPause) {
            btnPlayPause.baseIcon = "toolbar-mediacontrol-pause";
        } else {
            btnPlayPause.baseIcon = "toolbar-mediacontrol-play";
        }
    }

    Label {
        id: txtTitle
        anchors.bottom: buttons.top
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        elide: Text.ElideRight
        text: renderer.title
    }

    ProgressBar {
        id: progressBar
        value: renderer.progress
        width: parent.width
        anchors.bottom: times.top
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        MouseArea {
            enabled: renderer.canSeek
            anchors.fill: parent
            onReleased: {
                renderer.seekRelative(mouseX / width)
            }
        }
    }

    Item {
        id: times
        anchors.bottom: volumeControl.top
        anchors.bottomMargin: 80
        anchors.left: parent.left
        anchors.right: parent.right
        Text {
            text: renderer.position
            color: "white"
            anchors.left: parent.left
            font.pixelSize: 26
        }
        Text {
            text: renderer.duration
            color: "white"
            anchors.right: parent.right
            font.pixelSize: 26
        }
    }

    VolumeControl {
        id: volumeControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Row {
        id: buttons
        anchors.bottom: progressBar.top
        anchors.horizontalCenter: parent.horizontalCenter
        ThemeToolIcon {
            enabled: renderer.state !== "PLAYING" || (renderer.state === "PLAYING" && renderer.canPause)
            id: btnPlayPause
            baseIcon: "toolbar-mediacontrol-play"
            onClicked: {
                if (renderer.state === "PLAYING") {
                    renderer.pause();
                } else {
                    renderer.play();
                }
            }
        }

        ThemeToolIcon {
            enabled: renderer.state !== "STOPPED"
            id: btnStop
            baseIcon: "toolbar-mediacontrol-stop"
            onClicked: {
                renderer.stop();
            }
        }

        Connections {
            target: renderer
            onStateChanged: {
                if (renderer.state !== "TRANSITIONING") {
                    updatePlayButton();
                }
            }

            onCanPauseChanged: updatePlayButton();
        }
    }
}
