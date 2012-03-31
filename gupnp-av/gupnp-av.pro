TARGET=gupnpav
TEMPLATE=lib
CONFIG += staticlib link_pkgconfig
CONFIG -= qt

PKGCONFIG += glib-2.0 gupnp-1.0 libxml-2.0

HEADERS = gupnp-av-error.h \
          gupnp-av.h \
          gupnp-av-marshal.h \
          gupnp-didl-lite-container.h \
          gupnp-didl-lite-contributor.h \
          gupnp-didl-lite-contributor-private.h \
          gupnp-didl-lite-descriptor.h \
          gupnp-didl-lite-descriptor-private.h \
          gupnp-didl-lite-item.h \
          gupnp-didl-lite-object.h \
          gupnp-didl-lite-object-private.h \
          gupnp-didl-lite-parser.h \
          gupnp-didl-lite-resource.h \
          gupnp-didl-lite-resource-private.h \
          gupnp-didl-lite-writer.h \
          gupnp-dlna.h \
          gupnp-last-change-parser.h \
          gupnp-protocol-info.h \
          gupnp-search-criteria-parser.h \
          gvalue-util.h \
          xml-util.h
SOURCES = gupnp-av-error.c \
          gupnp-av-marshal.c \
          gupnp-didl-lite-container.c \
          gupnp-didl-lite-contributor.c \
          gupnp-didl-lite-descriptor.c \
          gupnp-didl-lite-item.c \
          gupnp-didl-lite-object.c \
          gupnp-didl-lite-parser.c \
          gupnp-didl-lite-resource.c \
          gupnp-didl-lite-writer.c \
          gupnp-dlna.c \
          gupnp-last-change-parser.c \
          gupnp-protocol-info.c \
          gupnp-search-criteria-parser.c \
          gvalue-util.c \
          xml-util.c
