/*
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Authors: Zeeshan Ali (Khattak) <zeeshan.ali@nokia.com>
 *                                <zeeshanak@gnome.org>
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

#ifndef __GUPNP_DIDL_LITE_ITEM_H__
#define __GUPNP_DIDL_LITE_ITEM_H__

#include <glib-object.h>
#include "gupnp-didl-lite-object.h"

G_BEGIN_DECLS

GType
gupnp_didl_lite_item_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DIDL_LITE_ITEM \
                (gupnp_didl_lite_item_get_type ())
#define GUPNP_DIDL_LITE_ITEM(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_ITEM, \
                 GUPnPDIDLLiteItem))
#define GUPNP_DIDL_LITE_ITEM_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_ITEM, \
                 GUPnPDIDLLiteItemClass))
#define GUPNP_IS_DIDL_LITE_ITEM(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_ITEM))
#define GUPNP_IS_DIDL_LITE_ITEM_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_ITEM))
#define GUPNP_DIDL_LITE_ITEM_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_DIDL_LITE_ITEM, \
                 GUPnPDIDLLiteItemClass))

typedef struct {
        GUPnPDIDLLiteObject parent;
} GUPnPDIDLLiteItem;

typedef struct {
        GUPnPDIDLLiteObjectClass parent_class;

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
} GUPnPDIDLLiteItemClass;

const char *
gupnp_didl_lite_item_get_ref_id         (GUPnPDIDLLiteItem *item);

void
gupnp_didl_lite_item_set_ref_id         (GUPnPDIDLLiteItem *item,
                                         const char        *ref_id);

G_END_DECLS

#endif /* __GUPNP_DIDL_LITE_ITEM_H__ */
