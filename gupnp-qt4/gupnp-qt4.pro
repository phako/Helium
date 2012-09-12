TARGET=gupnp-qt4
QMAKE_CXXFLAGS += -std=gnu++0x
TEMPLATE=lib
CONFIG += staticlib link_pkgconfig

PKGCONFIG += glib-2.0 gupnp-1.0 libxml-2.0
INCLUDEPATH += ../gupnp-av

HEADERS = didlliteparser.h \
         didlliteparser_p.h \
         glib-utils.h \
         refptrg.h \
         serviceproxycall.h \
         serviceproxy.h \
         serviceproxy_p.h \
         serviceintrospection.h \
         serviceintrospection_p.h

SOURCES = didlliteparser.cpp \
          glib-utils.cpp \
          serviceproxycall.cpp \
          serviceproxy.cpp \
          serviceintrospection.cpp

