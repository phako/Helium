/*
 * Copyright (C) 2007 Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 * Copyright (C) 2007 OpenedHand Ltd
 *
 * Authors: Zeeshan Ali Khattak <zeenix@gstreamer.net>
 *          Jorn Baayen <jorn@openedhand.com>
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

#ifndef __GUPNP_LAST_CHANGE_PARSER_H__
#define __GUPNP_LAST_CHANGE_PARSER_H__

#include <libgupnp/gupnp.h>

G_BEGIN_DECLS

GType
gupnp_last_change_parser_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_LAST_CHANGE_PARSER \
                (gupnp_last_change_parser_get_type ())
#define GUPNP_LAST_CHANGE_PARSER(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_LAST_CHANGE_PARSER, \
                 GUPnPLastChangeParser))
#define GUPNP_LAST_CHANGE_PARSER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_LAST_CHANGE_PARSER, \
                 GUPnPLastChangeParserClass))
#define GUPNP_IS_LAST_CHANGE_PARSER(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_LAST_CHANGE_PARSER))
#define GUPNP_IS_LAST_CHANGE_PARSER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_LAST_CHANGE_PARSER))
#define GUPNP_LAST_CHANGE_PARSER_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_LAST_CHANGE_PARSER, \
                 GUPnPLastChangeParserClass))

typedef struct {
        GObject parent;
} GUPnPLastChangeParser;

typedef struct {
        GObjectClass parent_class;

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
} GUPnPLastChangeParserClass;

GUPnPLastChangeParser *
gupnp_last_change_parser_new           (void);

gboolean
gupnp_last_change_parser_parse_last_change_valist
                                       (GUPnPLastChangeParser *parser,
                                        guint                  instance_id,
                                        const char            *last_change_xml,
                                        GError               **error,
                                        va_list                var_args);

gboolean
gupnp_last_change_parser_parse_last_change
                                       (GUPnPLastChangeParser *parser,
                                        guint                  instance_id,
                                        const char            *last_change_xml,
                                        GError               **error,
                                        ...) G_GNUC_NULL_TERMINATED;

G_END_DECLS

#endif /* __GUPNP_LAST_CHANGE_PARSER_H__ */

