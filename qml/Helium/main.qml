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

    initialPage: serverList

    ServerList {
        id: serverList
    }

    MainPage {
        id: mainPage
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
