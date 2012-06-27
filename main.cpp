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

#include <glib-object.h>

#include <QtDBus/QDBusInterface>
#include <QtFeedback/QFeedbackHapticsEffect>
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

#include "networkcontrol.h"
#include "settings.h"

#include <QtDeclarative>

QDeclarativeContext *rootContext;

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    g_type_init ();

    // Used in GUPnP User-Agent
    g_set_application_name("Helium");

    // QML glue
    qmlRegisterType<UPnPRenderer>("org.jensge.UPnP", 1, 0, "UPnPRenderer");
    qmlRegisterType<UPnPMediaServer>("org.jensge.UPnP", 1, 0, "UPnPMediaServer");
    qmlRegisterType<BrowseModel>("org.jensge.UPnP", 1, 0, "BrowseModel");

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QString locale = QLocale::system().name();
    QTranslator translator;
    if (!(translator.load(QLatin1String("tr_") + locale, QLatin1String(":/")))) {
        translator.load(QLatin1String("tr_en"), QLatin1String(":/"));
    }

    app->installTranslator(&translator);

    UPnPServerModel serverModel;
    UPnPRendererModel rendererModel;
    Settings settings;

    // only annoy the user on start-up by showing the connection dialog.
    // GUPnP can handle appearing and disappearing network devices itself quite
    // fine so there's no need to listen and pop-up that stupid connection
    // dialog all the time
    NetworkControl control;

    QtMobility::QFeedbackHapticsEffect effect;
    effect.setIntensity(1.0);
    effect.setDuration(100);

    QmlApplicationViewer viewer;
    rootContext = viewer.rootContext();
    rootContext->setContextProperty(QLatin1String("serverModel"), &serverModel);
    rootContext->setContextProperty(QLatin1String("rendererModel"), &rendererModel);
    rootContext->setContextProperty(QLatin1String("browseModel"), &BrowseModel::empty());
    rootContext->setContextProperty(QLatin1String("browseModelStack"), &BrowseModelStack::getDefault());
    rootContext->setContextProperty(QLatin1String("networkControl"), &control);
    rootContext->setContextProperty(QLatin1String("feedback"), &effect);
    rootContext->setContextProperty(QLatin1String("settings"), &settings);
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.setMainQmlFile(QLatin1String("qml/Helium/main.qml"));
    viewer.showExpanded();

#ifdef HARMATTAN
    if (settings.startMediaSharing()) {
        QDBusInterface fdo(QLatin1String("org.freedesktop.DBus"), QLatin1String("/"), QLatin1String("org.freedesktop.DBus"));
        bool isRunning = fdo.call(QLatin1String("NameHasOwner"), Settings::RYGEL_DBUS_IFACE).arguments().first().toBool();
        if (settings.mediaSharingAvailable() && not isRunning) {
            // Can't use D-Bus activation since we need to keep this particular port
            QProcess::startDetached (QLatin1String("/usr/bin/rygel"),
                                     QStringList() << QLatin1String("--port=57734"));
        }
    }
#endif

    return app->exec();
}
