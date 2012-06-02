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
    Label {
        id: txtTitle
        anchors.bottom: buttons.top
        anchors.leftMargin: 16
        anchors.rightMargin: 16
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
        anchors.leftMargin: 16
        anchors.rightMargin: 16
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
        anchors.leftMargin: 16
        anchors.rightMargin: 16
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

    Rectangle {
        color: "blue"
        id: volumeControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 16
        anchors.rightMargin: 16

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

    Row {
        id: buttons
        anchors.bottom: progressBar.top
        anchors.horizontalCenter: parent.horizontalCenter
        ToolIcon {
            enabled: renderer.state !== "PLAYING" || (renderer.state === "PLAYING" && renderer.canPause)
            id: btnPlayPause
            iconId: "toolbar-mediacontrol-play" + (enabled ? "" : "-dimmed")
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
            iconId: "toolbar-mediacontrol-stop" + (enabled ? "" : "-dimmed")
            onClicked: {
                renderer.stop();
            }
        }

        Connections {
            target: renderer
            onStateChanged: {
                var icon;
                if (renderer.state === "PLAYING" && renderer.canPause) {
                    icon = "toolbar-mediacontrol-pause";
                } else {
                    icon = "toolbar-mediacontrol-play";
                }
                if (!btnPlayPause.enabled) {
                    icon += "-dimmed";
                }
                btnPlayPause.iconId = icon;
            }

            onCanPauseChanged: {
                var icon;
                if (renderer.state === "PLAYING" && renderer.canPause) {
                    icon = "toolbar-mediacontrol-pause";
                } else {
                    icon = "toolbar-mediacontrol-play";
                }
                if (!btnPlayPause.enabled) {
                    icon += "-dimmed";
                }
                btnPlayPause.iconId = icon;
            }
        }
    }
}
