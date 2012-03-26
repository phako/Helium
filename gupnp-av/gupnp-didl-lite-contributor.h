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

#ifndef __GUPNP_DIDL_LITE_CONTRIBUTOR_H__
#define __GUPNP_DIDL_LITE_CONTRIBUTOR_H__

#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS

GType
gupnp_didl_lite_contributor_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DIDL_LITE_CONTRIBUTOR \
                (gupnp_didl_lite_contributor_get_type ())
#define GUPNP_DIDL_LITE_CONTRIBUTOR(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_CONTRIBUTOR, \
                 GUPnPDIDLLiteContributor))
#define GUPNP_DIDL_LITE_CONTRIBUTOR_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_CONTRIBUTOR, \
                 GUPnPDIDLLiteContributorClass))
#define GUPNP_IS_DIDL_LITE_CONTRIBUTOR(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_CONTRIBUTOR))
#define GUPNP_IS_DIDL_LITE_CONTRIBUTOR_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_CONTRIBUTOR))
#define GUPNP_DIDL_LITE_CONTRIBUTOR_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_DIDL_LITE_CONTRIBUTOR, \
                 GUPnPDIDLLiteContributorClass))

typedef struct _GUPnPDIDLLiteContributorPrivate GUPnPDIDLLiteContributorPrivate;

typedef struct {
        GObject parent;

        GUPnPDIDLLiteContributorPrivate *priv;
} GUPnPDIDLLiteContributor;

typedef struct {
        GObjectClass parent_class;

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
} GUPnPDIDLLiteContributorClass;

const char *
gupnp_didl_lite_contributor_get_role    (GUPnPDIDLLiteContributor *contributor);

const char *
gupnp_didl_lite_contributor_get_name    (GUPnPDIDLLiteContributor *contributor);

void
gupnp_didl_lite_contributor_set_role    (GUPnPDIDLLiteContributor *contributor,
                                         const char               *role);

void
gupnp_didl_lite_contributor_set_name    (GUPnPDIDLLiteContributor *contributor,
                                         const char               *name);

xmlNode *
gupnp_didl_lite_contributor_get_xml_node
                                        (GUPnPDIDLLiteContributor *contributor);

G_END_DECLS

#endif /* __GUPNP_DIDL_LITE_CONTRIBUTOR_H__ */
