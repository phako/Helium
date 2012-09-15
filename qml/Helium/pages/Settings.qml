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

import "../components"
import ".."

Page {
    Flickable {
        id: flckContent
        clip: true
        anchors.top: rtnTitle.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        contentWidth: parent.width
        contentHeight: column.height
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: column
            width: parent.width
            spacing: 24

            Separator { text: qsTr("Display") }

            LabelledSwitch {
                text: qsTr("Show device icons")
                checked: settings.displayDeviceIcons;
                onCheckedChanged: settings.displayDeviceIcons = checked
            }

            LabelledSwitch {
                text: qsTr("Show thumbnails and album art")
                checked: settings.displayMediaArt;
                onCheckedChanged: settings.displayMediaArt = checked
            }

            LabelledSwitch {
                text: qsTr("Show Device pop-up on long press")
                checked: settings.showDevicePopUp;
                onCheckedChanged: settings.showDevicePopUp = checked;
            }

            Separator { text: qsTr("Filtering") }

            ButtonRow {
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: btnFilterTitleOnly
                    text: qsTr("Title only")
                    checked: !settings.filterInDetails
                    onCheckedChanged: settings.filterInDetails = !checked
                }

                Button {
                    id: btnFilterAll
                    text: qsTr("All details")
                    checked: settings.filterInDetails
                    onCheckedChanged: settings.filterInDetails = checked
                }
            }

            Separator { text: qsTr("Sharing") }

            LabelledSwitch {
                text: qsTr("Launch media sharing on start")
                checked: settings.startMediaSharing
                onCheckedChanged: settings.startMediaSharing = checked
                enabled: settings.mediaSharingAvailable
            }

            Separator { text: qsTr("Debugging") }

            LabelledSwitch {
                text: qsTr("Enable debug log")
                checked: settings.debug
                onCheckedChanged: settings.debug = checked
            }

            Label {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 40
                anchors.rightMargin: 10
                text: qsTr("Output path for log files:")
                enabled: settings.debug
            }

            TextField {
                anchors.left: parent.left
                anchors.leftMargin: 40
                anchors.right: parent.right
                anchors.rightMargin: 10
                text: settings.debugPath

                onTextChanged: {
                    if (!activeFocus && text !== settings.debugPath) {
                        settings.debugPath = text
                    }
                }

                onActiveFocusChanged: {
                    if (!activeFocus && text !== settings.debugPath) {
                        settings.debugPath = text;
                    }
                }

                Keys.onReturnPressed: {
                    if (text !== settings.debugPath) {
                        settings.debugPath = text;
                    }
                }
            }
        }
    }

    PageHeader {
        id: rtnTitle
        refreshable: false
        busy: false
        text: qsTr("Settings");
    }
}
