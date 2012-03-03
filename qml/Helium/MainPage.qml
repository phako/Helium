// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
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
                iconSource: "image://theme/icon-m-toolbar-list"
                tab: pageStackBrowse
            }
            TabButton {
                iconSource: "image://theme/icon-m-toolbar-mediacontrol-play"
                tab: pageStackPlayer
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
}
