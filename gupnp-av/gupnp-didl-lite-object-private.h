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

#ifndef __GUPNP_DIDL_LITE_OBJECT_PRIVATE_H__
#define __GUPNP_DIDL_LITE_OBJECT_PRIVATE_H__

#include <glib-object.h>
#include <libxml/tree.h>

G_BEGIN_DECLS

G_GNUC_INTERNAL GUPnPDIDLLiteObject *
gupnp_didl_lite_object_new_from_xml     (xmlNode     *xml_node,
                                         GUPnPXMLDoc *xml_doc,
                                         xmlNs       *upnp_ns,
                                         xmlNs       *dc_ns,
                                         xmlNs       *dlna_ns);

G_END_DECLS

#endif /* __GUPNP_DIDL_LITE_OBJECT_PRIVATE_H__ */
