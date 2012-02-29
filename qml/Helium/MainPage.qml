import QtQuick 1.1
import com.nokia.meego 1.0
import org.jensge.UPnP 1.0

Page {
    DeviceLister {
        id: rendererDeviceLister

        onListViewChanged: {
            listView.model = rendererModel
        }

        onIndexChanged: {
            renderer.wrapDevice(rendererModel.get(index))
//            btnRendererSelect.iconSource = renderer.icon
//            btnRendererSelect.text = renderer.friendlyName
        }
    }

    DeviceLister {
        id: serverDeviceLister

        onListViewChanged: {
            listView.model = serverModel
        }

        onIndexChanged: {
            server.wrapDevice(serverModel.get(index))
            btnServerSelect.text = server.friendlyName
            //browseList.model = server.browse()
        }
    }
    Rectangle {
        id: topRect
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 70
        color: "black"
        Button {
            id: btnServerSelect
            anchors.top: parent.top
            text: qsTr("Select Server")
            width: parent.width
            iconSource: "pixmaps/upnp-device.png"
            onClicked: { pageStack.push(serverDeviceLister); }
        }
    }

    Rectangle {
        color: "black"
        anchors.top: topRect.bottom
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        ListView {
            width: parent.width
            height: parent.height

            id: browseList;
        }
    }
}
