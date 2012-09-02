TARGET=gupnpav
TEMPLATE=lib
CONFIG += staticlib link_pkgconfig
CONFIG -= qt

PKGCONFIG += glib-2.0 gupnp-1.0 libxml-2.0

QMAKE_CFLAGS_RELEASE += -w
QMAKE_CFLAGS_DEBUG += -w

marshal_header.CONFIG += target_predeps no_link
marshal_header.output = gupnp-av-marshal.h
marshal_header.input = MARSHAL_LIST
marshal_header.commands = glib-genmarshal --prefix=gupnp_av_marshal ${QMAKE_FILE_NAME} --header > ${QMAKE_FILE_OUT}
marshal_header.variable_out = HEADERS
QMAKE_EXTRA_COMPILERS += marshal_header

marshal_body.CONFIG += target_predeps no_link
marshal_body.output = gupnp-av-marshal.c
marshal_body.input = MARSHAL_LIST
marshal_body.commands = glib-genmarshal --prefix=gupnp_av_marshal ${QMAKE_FILE_NAME} --header --body > ${QMAKE_FILE_OUT}
marshal_body.variable_out = SOURCES
QMAKE_EXTRA_COMPILERS += marshal_body

# Work-around old GUPnP version on Harmattan
contains(MEEGO_EDITION, harmattan) {
    DEFINES+=GUPNP_XML_ERROR_INVALID_ATTRIBUTE=GUPNP_XML_ERROR_PARSE
}

MARSHAL_LIST = gupnp-av/libgupnp-av/gupnp-av-marshal.list

HEADERS = gupnp-av/libgupnp-av/gupnp-av-error.h \
          gupnp-av/libgupnp-av/gupnp-av.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-container.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-contributor.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-contributor-private.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-descriptor.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-descriptor-private.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-item.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-object.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-object-private.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-parser.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-resource.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-resource-private.h \
          gupnp-av/libgupnp-av/gupnp-didl-lite-writer.h \
          gupnp-av/libgupnp-av/gupnp-dlna.h \
          gupnp-av/libgupnp-av/gupnp-last-change-parser.h \
          gupnp-av/libgupnp-av/gupnp-protocol-info.h \
          gupnp-av/libgupnp-av/gupnp-search-criteria-parser.h \
          gupnp-av/libgupnp-av/gvalue-util.h \
          gupnp-av/libgupnp-av/xml-util.h
SOURCES = gupnp-av/libgupnp-av/gupnp-av-error.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-container.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-contributor.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-descriptor.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-item.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-object.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-parser.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-resource.c \
          gupnp-av/libgupnp-av/gupnp-didl-lite-writer.c \
          gupnp-av/libgupnp-av/gupnp-dlna.c \
          gupnp-av/libgupnp-av/gupnp-last-change-parser.c \
          gupnp-av/libgupnp-av/gupnp-protocol-info.c \
          gupnp-av/libgupnp-av/gupnp-search-criteria-parser.c \
          gupnp-av/libgupnp-av/gvalue-util.c \
          gupnp-av/libgupnp-av/xml-util.c
