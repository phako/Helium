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

    Rectangle {
        id: tooltip
        opacity: 0.8
        z: 10
        color: "white"
        property alias text : time.text
        radius: 12.5
        visible: false
        anchors.bottom: seekHelper.top
        anchors.bottomMargin: 30
        width: 120
        height: 40
        Text {
            id: time
            anchors.centerIn: parent
            font.pixelSize: 19
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

    Row {
        id: buttons
        anchors.bottom: seekHelper.top
        anchors.horizontalCenter: parent.horizontalCenter
        ThemeToolIcon {
            id: btnPrev
            enabled: renderer.state === "STOPPED" || renderer.state === "PLAYING"
            baseIcon: "toolbar-mediacontrol-previous"
            onClicked: renderer.prev();
        }

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

        ThemeToolIcon {
            id: btnNext
            enabled: renderer.state === "STOPPED" || renderer.state === "PLAYING"
            baseIcon: "toolbar-mediacontrol-next"
            onClicked: renderer.next();
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

    Rectangle {
        id: seekHelper
        color: "black"
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        anchors.bottom: volumeControl.top

        ProgressBar {
            id: progressBar
            value: renderer.progress
            width: parent.width
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.right: parent.right
        }

        Item {
            id: times
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            Text {
                text: renderer.position
                color: "white"
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                font.pixelSize: 26
            }
            Text {
                text: renderer.duration
                color: "white"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                font.pixelSize: 26
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: renderer.canSeek
            preventStealing: true
            onPressed: {
                tooltip.visible = true
            }

            onMouseXChanged: {
                var tooltipLeft = mouseX - (tooltip.width / 2.0);
                var tooltipRight = tooltipLeft + tooltip.width
                if (tooltipLeft <= 0) {
                    tooltipLeft = 0
                } else if (tooltipRight > parent.width) {
                    tooltipLeft = parent.width - tooltip.width;
                }
                tooltip.x = tooltipLeft
                tooltip.text = renderer.getRelativeTime(mouseX / width)
            }

            onReleased: {
                tooltip.visible = false
                renderer.seekRelative(mouseX / width)
            }
        }
    }

    VolumeControl {
        id: volumeControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
