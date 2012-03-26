/*
 * Copyright (C) 2009 Nokia Corporation.
 * Copyright (C) 2007, 2008 OpenedHand Ltd.
 *
 * Authors: Zeeshan Ali (Khattak) <zeeshan.ali@nokia.com>
 *                                <zeeshanak@gnome.org>
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

#ifndef __GUPNP_PROTOCOL_INFO_H__
#define __GUPNP_PROTOCOL_INFO_H__

#include <stdarg.h>
#include <glib-object.h>

#include "gupnp-dlna.h"

G_BEGIN_DECLS

GType
gupnp_protocol_info_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_PROTOCOL_INFO \
                (gupnp_protocol_info_get_type ())
#define GUPNP_PROTOCOL_INFO(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_PROTOCOL_INFO, \
                 GUPnPProtocolInfo))
#define GUPNP_PROTOCOL_INFO_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_PROTOCOL_INFO, \
                 GUPnPProtocolInfoClass))
#define GUPNP_IS_PROTOCOL_INFO(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_PROTOCOL_INFO))
#define GUPNP_IS_PROTOCOL_INFO_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_PROTOCOL_INFO))
#define GUPNP_PROTOCOL_INFO_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_PROTOCOL_INFO, \
                 GUPnPProtocolInfoClass))

typedef struct _GUPnPProtocolInfoPrivate GUPnPProtocolInfoPrivate;

typedef struct {
        GObject parent;

        GUPnPProtocolInfoPrivate *priv;
} GUPnPProtocolInfo;

typedef struct {
        GObjectClass parent_class;

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
} GUPnPProtocolInfoClass;

GUPnPProtocolInfo *
gupnp_protocol_info_new                 (void);

GUPnPProtocolInfo *
gupnp_protocol_info_new_from_string     (const char        *protocol_info,
                                         GError           **error);

char *
gupnp_protocol_info_to_string           (GUPnPProtocolInfo *info);

gboolean
gupnp_protocol_info_is_compatible       (GUPnPProtocolInfo *info1,
                                         GUPnPProtocolInfo *info2);

void
gupnp_protocol_info_set_protocol        (GUPnPProtocolInfo *info,
                                         const char        *protocol);

void
gupnp_protocol_info_set_network         (GUPnPProtocolInfo *info,
                                         const char        *network);

void
gupnp_protocol_info_set_mime_type       (GUPnPProtocolInfo *info,
                                         const char        *mime_type);

void
gupnp_protocol_info_set_dlna_profile    (GUPnPProtocolInfo *info,
                                         const char        *profile);

void
gupnp_protocol_info_set_play_speeds     (GUPnPProtocolInfo *info,
                                         const char       **speeds);

void
gupnp_protocol_info_set_dlna_conversion (GUPnPProtocolInfo  *info,
                                         GUPnPDLNAConversion conversion);

void
gupnp_protocol_info_set_dlna_operation  (GUPnPProtocolInfo *info,
                                         GUPnPDLNAOperation operation);

void
gupnp_protocol_info_set_dlna_flags      (GUPnPProtocolInfo *info,
                                         GUPnPDLNAFlags     flags);

const char *
gupnp_protocol_info_get_protocol        (GUPnPProtocolInfo *info);

const char *
gupnp_protocol_info_get_network         (GUPnPProtocolInfo *info);

const char *
gupnp_protocol_info_get_mime_type       (GUPnPProtocolInfo *info);

const char *
gupnp_protocol_info_get_dlna_profile    (GUPnPProtocolInfo *info);

const char **
gupnp_protocol_info_get_play_speeds     (GUPnPProtocolInfo *info);

GUPnPDLNAConversion
gupnp_protocol_info_get_dlna_conversion (GUPnPProtocolInfo *info);

GUPnPDLNAOperation
gupnp_protocol_info_get_dlna_operation  (GUPnPProtocolInfo *info);

GUPnPDLNAFlags
gupnp_protocol_info_get_dlna_flags      (GUPnPProtocolInfo *info);

G_END_DECLS

#endif /* __GUPNP_PROTOCOL_INFO_H__ */
