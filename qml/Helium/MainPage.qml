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

Page {
    property alias pageStackBrowse: pageStackBrowse
    property alias pageStackPlayer: pageStackPlayer

    tools: ToolBarLayout {
        ToolIcon {
            enabled: tabGroup.currentTab.depth > 1
            iconId: enabled ? "toolbar-back" : "toolbar-back-dimmed"
            onClicked: {
                if (tabGroup.currentTab === pageStackBrowse) {
                    browseModelStack.pop();
                    if (browseModelStack.empty()) {
                        pageStackBrowse.pop()
                    }
                }
            }
        }

        ButtonRow {
            platformStyle: TabButtonStyle {}
            TabButton {
                iconSource: "image://theme/icon-m-toolbar-list-white"
                tab: pageStackBrowse
            }
            TabButton {
                iconSource: "image://theme/icon-m-toolbar-mediacontrol-play-white"
                tab: pageStackPlayer
            }
            /*TabButton {
                iconSource: "image://theme/icon-m-content-playlist-inverse"
            }*/
        }
        ToolIcon {
            iconId: "toolbar-view-menu"
            onClicked: tabGroup.currentTab = pageStackPlayer
        }
    }

    TabGroup {
        id: tabGroup
        currentTab: pageStackBrowse
        PageStack {
            id: pageStackBrowse
        }
        PageStack {
            id: pageStackPlayer
        }
        Component.onCompleted: {
            pageStackBrowse.push(serverList);
            pageStackPlayer.push(playerList);
        }
    }
}
