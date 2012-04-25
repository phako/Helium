/*
 * Copyright (C) 2006, 2007 OpenedHand Ltd.
 * Copyright (C) 2007 Zeeshan Ali.
 *
 * Author: Jorn Baayen <jorn@openedhand.com>
 * Author: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
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

#ifndef __XML_UTIL_H__
#define __XML_UTIL_H__

#include <glib.h>
#include <libxml/tree.h>
#include <stdarg.h>

/* Misc utilities for inspecting xmlNodes */
G_GNUC_INTERNAL xmlNode *
xml_util_get_element                    (xmlNode    *node,
                                         ...) G_GNUC_NULL_TERMINATED;

G_GNUC_INTERNAL GList *
xml_util_get_child_elements_by_name     (xmlNode *node,
                                         const char *name);

G_GNUC_INTERNAL const char *
xml_util_get_child_element_content      (xmlNode    *node,
                                         const char *child_name);

G_GNUC_INTERNAL const char *
xml_util_get_attribute_content          (xmlNode    *node,
                                         const char *attribute_name);

G_GNUC_INTERNAL gboolean
xml_util_get_boolean_attribute          (xmlNode    *node,
                                         const char *attribute_name);

G_GNUC_INTERNAL guint
xml_util_get_uint_attribute             (xmlNode    *node,
                                         const char *attribute_name,
                                         guint       default_value);

G_GNUC_INTERNAL gint
xml_util_get_int_attribute              (xmlNode    *node,
                                         const char *attribute_name,
                                         gint        default_value);

G_GNUC_INTERNAL glong
xml_util_get_long_attribute             (xmlNode    *node,
                                         const char *attribute_name,
                                         glong       default_value);

G_GNUC_INTERNAL gint64
xml_util_get_int64_attribute            (xmlNode    *node,
                                         const char *attribute_name,
                                         gint64      default_value);

G_GNUC_INTERNAL xmlNode *
xml_util_set_child                      (xmlNode    *parent_node,
                                         xmlNs      *namespace,
                                         xmlDoc     *doc,
                                         const char *name,
                                         const char *value);

G_GNUC_INTERNAL gboolean
xml_util_verify_attribute_is_boolean    (xmlNode    *node,
                                         const char *attribute_name);

#endif /* __XML_UTIL_H__ */
