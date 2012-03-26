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

#ifndef __GUPNP_DIDL_LITE_DESCRIPTOR_H__
#define __GUPNP_DIDL_LITE_DESCRIPTOR_H__

#include <stdarg.h>
#include <glib-object.h>
#include <libxml/tree.h>
#include <libgupnp/gupnp.h>

G_BEGIN_DECLS

GType
gupnp_didl_lite_descriptor_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DIDL_LITE_DESCRIPTOR \
                (gupnp_didl_lite_descriptor_get_type ())
#define GUPNP_DIDL_LITE_DESCRIPTOR(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_DESCRIPTOR, \
                 GUPnPDIDLLiteDescriptor))
#define GUPNP_DIDL_LITE_DESCRIPTOR_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_DIDL_LITE_DESCRIPTOR, \
                 GUPnPDIDLLiteDescriptorClass))
#define GUPNP_IS_DIDL_LITE_DESCRIPTOR(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_DESCRIPTOR))
#define GUPNP_IS_DIDL_LITE_DESCRIPTOR_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_DIDL_LITE_DESCRIPTOR))
#define GUPNP_DIDL_LITE_DESCRIPTOR_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_DIDL_LITE_DESCRIPTOR, \
                 GUPnPDIDLLiteDescriptorClass))

typedef struct _GUPnPDIDLLiteDescriptorPrivate GUPnPDIDLLiteDescriptorPrivate;

typedef struct {
        GObject parent;

        GUPnPDIDLLiteDescriptorPrivate *priv;
} GUPnPDIDLLiteDescriptor;

typedef struct {
        GObjectClass parent_class;

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
} GUPnPDIDLLiteDescriptorClass;

xmlNode *
gupnp_didl_lite_descriptor_get_xml_node (GUPnPDIDLLiteDescriptor *descriptor);

const char *
gupnp_didl_lite_descriptor_get_content  (GUPnPDIDLLiteDescriptor *descriptor);

const char *
gupnp_didl_lite_descriptor_get_id       (GUPnPDIDLLiteDescriptor *descriptor);

const char *
gupnp_didl_lite_descriptor_get_metadata_type
                                        (GUPnPDIDLLiteDescriptor *descriptor);

const char *
gupnp_didl_lite_descriptor_get_name_space
                                        (GUPnPDIDLLiteDescriptor *descriptor);

void
gupnp_didl_lite_descriptor_set_content  (GUPnPDIDLLiteDescriptor *descriptor,
                                         const char              *content);

void
gupnp_didl_lite_descriptor_set_id       (GUPnPDIDLLiteDescriptor *descriptor,
                                         const char              *id);

void
gupnp_didl_lite_descriptor_set_metadata_type
                                        (GUPnPDIDLLiteDescriptor *descriptor,
                                         const char              *type);

void
gupnp_didl_lite_descriptor_set_name_space
                                        (GUPnPDIDLLiteDescriptor *descriptor,
                                         const char              *name_space);

G_END_DECLS

#endif /* __GUPNP_DIDL_LITE_DESCRIPTOR_H__ */
