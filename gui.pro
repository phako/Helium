TARGET=Helium
QMAKE_CXXFLAGS += -std=gnu++0x
# Add more folders to ship with the application, here
folder_01.source = qml/Helium
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE4B8798C

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
CONFIG += mobility
MOBILITY += feedback

# Include GUPnP libs - do this before boostable stuff otherwise that won't work properly
CONFIG += link_pkgconfig
PKGCONFIG += gupnp-1.0 gq-gconf

# include static version of gupnp-av - thanks madde :(
SUBDIRS += gupnp-av
LIBS += -Lgupnp-av -lgupnpav

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components

CONFIG += no_keywords

QT += network dbus

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    upnp/upnprenderer.cpp \
    upnp/upnpmediaserver.cpp \
    upnp/upnpdevice.cpp \
    upnp/upnpdevicemodel.cpp \
    upnp/upnpservermodel.cpp \
    upnp/upnprenderermodel.cpp \
    upnp/browsemodelstack.cpp \
    networkcontrol.cpp \
    settings.cpp \
    upnp/serviceproxycall.cpp \
    upnp/serviceproxy.cpp \
    upnp/serviceintrospection.cpp \
    upnp/glib-utils.cpp \
    upnp/browsemodel_p.cpp \
    upnp/browsemodel.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    TODO \
    qtc_packaging/debian_harmattan/postinst \
    qtc_packaging/debian_harmattan/prerm

HEADERS += \
    upnp/upnprenderer.h \
    upnp/upnpmediaserver.h \
    upnp/upnpdevice.h \
    upnp/upnpdevicemodel.h \
    upnp/upnpservermodel.h \
    upnp/upnprenderermodel.h \
    upnp/refptrg.h \
    upnp/browsemodelstack.h \
    networkcontrol.h \
    settings.h \
    upnp/serviceproxy_p.h \
    upnp/serviceproxycall.h \
    upnp/serviceproxy.h \
    upnp/serviceintrospection.h \
    upnp/serviceintrospection_p.h \
    upnp/glib-utils.h \
    upnp/browsemodel_p.h \
    upnp/browsemodel.h

RESOURCES += \
    res.qrc

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
