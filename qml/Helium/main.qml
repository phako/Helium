/*
This file is part of Helium.

Helium is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Helium is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU General Public License
along with Helium.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 1.1
import com.nokia.meego 1.0
import org.jensge.UPnP 1.0

PageStackWindow {
    id: appWindow

    property Style platformLabelStyle: LabelStyle {
        inverted: theme.inverted
    }
    property Style platformDialogStyle: SelectionDialogStyle {
        inverted: theme.inverted
    }

    initialPage: MainPage {}

    ServerList {
        id: serverList
        model: serverModel
        role: "server"
    }

    ServerList {
        id: playerList
        model: rendererModel
        role: "renderer"
    }

    Browse {
        id: browse
    }

    UPnPRenderer {
        id: renderer
    }

    UPnPMediaServer {
        id: server
    }

    Component.onCompleted: {
        theme.inverted = true
    }
}
