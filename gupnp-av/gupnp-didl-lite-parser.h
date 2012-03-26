/*
 * Copyright (C) 2007 Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *
 * Authors: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
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

#ifndef __GUPNP_DIDL_LITE_PARSER_H__
#define __GUPNP_DIDL_LITE_PARSER_H__

#include <libgupnp/gupnp.h>
#include "gupnp-didl-lite-container.h"
#include "gupnp-didl-lite-item.h"

G_BEGIN_DECLS

GType
gupnp_didl_lite_parser_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DIDL_LITE_PARSER \
                (gupnp_didl_lite_parser_get_type ())
#define GUPNP_DIDL_LITE_PARSER(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_PARSER, \
                 GUPnPDIDLLiteParser))
#define GUPNP_DIDL_LITE_PARSER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_PARSER, \
                 GUPnPDIDLLiteParserClass))
#define GUPNP_IS_DIDL_LITE_PARSER(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_PARSER))
#define GUPNP_IS_DIDL_LITE_PARSER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_PARSER))
#define GUPNP_DIDL_LITE_PARSER_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_DIDL_LITE_PARSER, \
                 GUPnPDIDLLiteParserClass))

typedef struct {
        GObject parent;

        gpointer gupnp_reserved;
} GUPnPDIDLLiteParser;

typedef struct {
        GObjectClass parent_class;

        /* signals */
        void (* object_available)    (GUPnPDIDLLiteParser    *parser,
                                      GUPnPDIDLLiteObject    *object);
        void (* item_available)      (GUPnPDIDLLiteParser    *parser,
                                      GUPnPDIDLLiteItem      *item);
        void (* container_available) (GUPnPDIDLLiteParser    *parser,
                                      GUPnPDIDLLiteContainer *container);

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
        void (* _gupnp_reserved5) (void);
} GUPnPDIDLLiteParserClass;

GUPnPDIDLLiteParser *
gupnp_didl_lite_parser_new              (void);

gboolean
gupnp_didl_lite_parser_parse_didl       (GUPnPDIDLLiteParser *parser,
                                         const char          *didl,
                                         GError             **error);

G_END_DECLS

#endif /* __GUPNP_DIDL_LITE_PARSER_H__ */
