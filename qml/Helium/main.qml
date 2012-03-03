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
