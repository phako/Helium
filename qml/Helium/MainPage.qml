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

Page {
    property alias pageStackBrowse: pageStackBrowse
    property alias pageStackPlayer: pageStackPlayer
    property bool minimalToolbar: tabGroup.currentTab.currentPage === about ||
                                  tabGroup.currentTab.currentPage === settingsPage
    property alias tabs: tabGroup

    tools: ToolBarLayout {
        ThemeToolIcon {
            enabled: tabGroup.currentTab.depth > 1
            baseIcon: "toolbar-back"
            onClicked: {
                if (minimalToolbar) {
                    tabGroup.currentTab.pop();

                    return;
                }

                if (tabGroup.currentTab === pageStackBrowse) {
                    browseModelStack.pop();
                    if (browseModelStack.empty()) {
                        pageStackBrowse.pop()
                        server.wrapDevice("");
                    }
                }
            }
        }

        ButtonRow {
            visible: !minimalToolbar
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
            visible: !minimalToolbar
            iconId: "toolbar-view-menu"
            onClicked: {
                if (menu.status === DialogStatus.Closed) {
                    menu.open();
                } else {
                    menu.close();
                }
            }
        }
    }

    Menu {
        id: menu
        visualParent: tabGroup.currentTab
        content: MenuLayout {
            MenuItem {
                text: qsTr("Settings")
                onClicked: tabGroup.currentTab.push(settingsPage)
            }

            MenuItem {
                text: qsTr("About...")
                onClicked: tabGroup.currentTab.push(about)
            }
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

    Component.onCompleted: {
        if (!networkControl.online) {
            networkControl.connect();
        }
    }
}
