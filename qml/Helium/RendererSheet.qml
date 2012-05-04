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
import org.jensge.UPnP 1.0

import "components"

Sheet {
    property alias currentIndex : rendererListView.currentIndex

    acceptButtonText: qsTr("Select")
    rejectButtonText: qsTr("Cancel")

    content : ListView {
        id: rendererListView

        anchors.fill: parent
        cacheBuffer: parent.height + 100
        snapMode: ListView.SnapToItem
        highlightMoveSpeed: -1

        highlight: ActiveSelection {}


        delegate: Item {
            height: 88
            width: parent.width

            LabelImage {
                image: model.icon
                iconVisible: settings.displayDeviceIcons
                mainText: model.friendlyName
                anchors.left: parent.left
                height: 88
                drilldownVisisble: false
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    rendererListView.currentIndex = index;
                }
            }
        }
        model: rendererModel

        onCurrentIndexChanged: {
            // ugly hack; Later versions of QML Sheet have a acceptButton property
            getButton("acceptButton").enabled = currentIndex != -1;
        }

        Component.onCompleted: currentIndex = -1
    }

    ScrollDecorator {
        flickableItem: rendererListView
    }

    Component.onCompleted: {
        // ugly hack; Later versions of QML Sheet have a acceptButton property
        getButton("acceptButton").enabled = currentIndex != -1;
    }
}
