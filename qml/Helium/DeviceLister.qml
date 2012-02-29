// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import org.jensge.UPnP 1.0

Page {
    property ListView listView : internalListView
    property int index: listView.currentIndex

    ToolBarLayout {
        id: tbLister
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: { pageStack.pop() }
        }
    }

    tools: tbLister

    ScrollDecorator {
        flickableItem: listView
    }

    Component {
        id: deviceDelegate
        Item {
            width: parent.width
            height: platformDialogStyle.itemHeight
            Image {
                id: imageItem
                width: 48
                height: 48
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                source: icon
            }
            Text {
                anchors.left: imageItem.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 20
                font {
                    family: platformLabelStyle.fontFamily
                    pixelSize: platformLabelStyle.fontPixelSize
                    bold: true
                }
                color: platformLabelStyle.textColor
                text: friendlyName
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    listView.currentIndex = index
                }
            }
        }
    }

    ListView {
        id: internalListView
        anchors.fill: parent
        highlight: Rectangle {
            width: -internalListView.width
            color: platformDialogStyle.itemSelectedBackgroundColor
            radius: 5
        }
        delegate: deviceDelegate
        highlightFollowsCurrentItem: true
        focus: true
        highlightMoveSpeed: -1
        model: listModel
    }
}
