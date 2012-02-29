#include <glib-object.h>

#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include "upnp/upnprenderer.h"
#include "upnp/upnpdevice.h"
#include "upnp/upnpdevicemodel.h"
#include "upnp/upnpmediaserver.h"
#include "upnp/upnpservermodel.h"
#include "upnp/upnprenderermodel.h"
#include "upnp/browsemodel.h"
#include "upnp/browsemodelstack.h"

#include <QtDeclarative>

QDeclarativeContext *rootContext;

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    g_type_init ();
    // QML glue
    qmlRegisterType<UPnPRenderer>("org.jensge.UPnP", 1, 0, "UPnPRenderer");
    qmlRegisterType<UPnPMediaServer>("org.jensge.UPnP", 1, 0, "UPnPMediaServer");
    qmlRegisterType<BrowseModel>("org.jensge.UPnP", 1, 0, "BrowseModel");

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    UPnPServerModel serverModel;
    UPnPRendererModel rendererModel;

    QmlApplicationViewer viewer;
    rootContext = viewer.rootContext();
    rootContext->setContextProperty("serverModel", &serverModel);
    rootContext->setContextProperty("rendererModel", &rendererModel);
    rootContext->setContextProperty("browseModel", 0);
    rootContext->setContextProperty("browseModelStack", &BrowseModelStack::getDefault());
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.setMainQmlFile(QLatin1String("qml/Helium/main.qml"));
    viewer.showExpanded();


    return app->exec();
}
