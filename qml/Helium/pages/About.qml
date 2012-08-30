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
        id: flickable
        anchors.top: rtnTitle.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        contentWidth: parent.width
        contentHeight: txtAbout.height
        Text {
            width: parent.width
            color: "white"
            font.pixelSize: 24
            id: txtAbout
            textFormat: Text.RichText
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: "An UPnP control point for your N9" +
                  "<p><strong>Version " + VERSION + "</strong></p>" +
                  "<p>Copyright © 2012<br />Jens Georg &lt;<a href=\"mailto:mail+helium@jensge.org\">mail+helium@jensge.org</a>&gt;</p>" +
                  "<p><strong>Website</strong><br />" +
                  "<a href=\"http://jensge.org/helium-a-m-dmc-for-your-n9/\">http://jensge.org/helium-a-m-dmc-for-your-n9/</a>" +

                  "<p><strong>Source repository</strong><br />" +
                  "<a href=\"https://gitorious.org/helium/helium\">https://gitorious.org/helium/helium</a>" +

                  "<p><strong>Issue tracker</strong><br />" +
                  "<a href=\"https://github.com/phako/Helium/issues\">https://github.com/phako/Helium/issues</a></p>" +

                  "<p>This program is free software: you can redistribute it and/or modify " +
                  "it under the terms of the GNU General Public License as published by " +
                  "the Free Software Foundation, either version 3 of the License, or " +
                  "(at your option) any later version.</p>" +

                  "<p>This program is distributed in the hope that it will be useful, " +
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of " +
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the " +
                  "GNU General Public License for more details.</p>" +

                  "<p>You should have received a copy of the GNU General Public License " +
                  "along with this program.  If not, see <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>."
            onLinkActivated : Qt.openUrlExternally(link);
        }
    }

    ScrollDecorator {
        flickableItem: flickable
    }

    PageHeader {
        id: rtnTitle
        text: qsTr("About Helium")
        refreshable: false
        busy: false
    }
}
