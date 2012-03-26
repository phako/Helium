/*
 * Copyright (C) 2007, 2008 OpenedHand Ltd.
 *
 * Authors: Jorn Baayen <jorn@openedhand.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GUPNP_DIDL_LITE_WRITER_H__
#define __GUPNP_DIDL_LITE_WRITER_H__


#include <stdarg.h>
#include <glib-object.h>
#include <libsoup/soup-uri.h>

#include "gupnp-dlna.h"
#include "gupnp-didl-lite-item.h"
#include "gupnp-didl-lite-container.h"
#include "gupnp-didl-lite-resource.h"
#include "gupnp-didl-lite-descriptor.h"

G_BEGIN_DECLS

GType
gupnp_didl_lite_writer_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DIDL_LITE_WRITER \
                (gupnp_didl_lite_writer_get_type ())
#define GUPNP_DIDL_LITE_WRITER(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_WRITER, \
                 GUPnPDIDLLiteWriter))
#define GUPNP_DIDL_LITE_WRITER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_WRITER, \
                 GUPnPDIDLLiteWriterClass))
#define GUPNP_IS_DIDL_LITE_WRITER(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_WRITER))
#define GUPNP_IS_DIDL_LITE_WRITER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_WRITER))
#define GUPNP_DIDL_LITE_WRITER_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_DIDL_LITE_WRITER, \
                 GUPnPDIDLLiteWriterClass))

typedef struct _GUPnPDIDLLiteWriterPrivate GUPnPDIDLLiteWriterPrivate;

typedef struct {
        GObject parent;

        GUPnPDIDLLiteWriterPrivate *priv;
} GUPnPDIDLLiteWriter;

typedef struct {
        GObjectClass parent_class;

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
} GUPnPDIDLLiteWriterClass;

#define GUPNP_DIDL_LITE_WRITER_NAMESPACE_DC   "dc"
#define GUPNP_DIDL_LITE_WRITER_NAMESPACE_UPNP "upnp"
#define GUPNP_DIDL_LITE_WRITER_NAMESPACE_DLNA "dlna"

GUPnPDIDLLiteWriter *
gupnp_didl_lite_writer_new              (const char *language);

GUPnPDIDLLiteItem *
gupnp_didl_lite_writer_add_item         (GUPnPDIDLLiteWriter *writer);

GUPnPDIDLLiteContainer *
gupnp_didl_lite_writer_add_container    (GUPnPDIDLLiteWriter *writer);

GUPnPDIDLLiteDescriptor *
gupnp_didl_lite_writer_add_descriptor   (GUPnPDIDLLiteWriter *writer);

xmlNode *
gupnp_didl_lite_writer_get_xml_node     (GUPnPDIDLLiteWriter   *writer);

char *
gupnp_didl_lite_writer_get_string       (GUPnPDIDLLiteWriter   *writer);

const char *
gupnp_didl_lite_writer_get_language     (GUPnPDIDLLiteWriter   *writer);

void
gupnp_didl_lite_writer_filter           (GUPnPDIDLLiteWriter   *writer,
                                         const char            *filter);

G_END_DECLS

#endif /* __GUPNP_DIDL_LITE_WRITER_H__ */
