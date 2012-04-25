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

#include <string.h>

#include "xml-util.h"

xmlNode *
xml_util_get_element (xmlNode *node,
                      ...)
{
        va_list var_args;

        va_start (var_args, node);

        while (TRUE) {
                const char *arg;

                arg = va_arg (var_args, const char *);
                if (!arg)
                        break;

                for (node = node->children; node; node = node->next) {
                        if (node->name == NULL)
                                continue;

                        if (!strcmp (arg, (char *) node->name))
                                break;
                }

                if (!node)
                        break;
        }

        va_end (var_args);

        return node;
}

GList *
xml_util_get_child_elements_by_name (xmlNode *node, const char *name)
{
       GList *children = NULL;

       for (node = node->children; node; node = node->next) {
               if (node->name == NULL) {
                       continue;
               }

               if (strcmp (name, (char *) node->name) == 0) {
                       children = g_list_append (children, node);
               }
       }

       return children;
}

const char *
xml_util_get_child_element_content (xmlNode    *node,
                                    const char *child_name)
{
        xmlNode *child_node;
        const char *content;

        child_node = xml_util_get_element (node, child_name, NULL);
        if (!child_node || !(child_node->children))
                return NULL;

        content = (const char *) child_node->children->content;
        if (!content)
                return NULL;

        return content;
}

const char *
xml_util_get_attribute_content (xmlNode    *node,
                                const char *attribute_name)
{
        xmlAttr *attribute;

        for (attribute = node->properties;
             attribute;
             attribute = attribute->next) {
                if (attribute->name == NULL)
                        continue;

                if (strcmp (attribute_name, (char *) attribute->name) == 0)
                        break;
        }

        if (attribute)
                return (const char *) attribute->children->content;
        else
                return NULL;
}

gboolean
xml_util_get_boolean_attribute (xmlNode    *node,
                                const char *attribute_name)
{
        const char *content;
        gchar   *str;
        gboolean ret;

        content = xml_util_get_attribute_content (node, attribute_name);
        if (!content)
                return FALSE;

        str = (char *) content;
        if (g_ascii_strcasecmp (str, "true") == 0 ||
            g_ascii_strcasecmp (str, "yes") == 0)
                ret = TRUE;
        else if (g_ascii_strcasecmp (str, "false") == 0 ||
                 g_ascii_strcasecmp (str, "no") == 0)
                ret = FALSE;
        else {
                int i;

                i = atoi (str);
                ret = i ? TRUE : FALSE;
        }

        return ret;
}

guint
xml_util_get_uint_attribute (xmlNode    *node,
                             const char *attribute_name,
                             guint       default_value)
{
        return (guint) xml_util_get_long_attribute (node,
                                                    attribute_name,
                                                    (glong) default_value);
}

gint
xml_util_get_int_attribute (xmlNode    *node,
                            const char *attribute_name,
                            gint        default_value)
{
        return (gint) xml_util_get_long_attribute (node,
                                                   attribute_name,
                                                   (glong) default_value);
}

glong
xml_util_get_long_attribute (xmlNode    *node,
                             const char *attribute_name,
                             glong       default_value)
{
    return (glong) xml_util_get_int64_attribute (node,
                                                 attribute_name,
                                                 (gint64) default_value);
}

gint64
xml_util_get_int64_attribute (xmlNode    *node,
                              const char *attribute_name,
                              gint64      default_value)
{
        const char *content;

        content = xml_util_get_attribute_content (node, attribute_name);
        if (!content)
                return default_value;

        return g_ascii_strtoll (content, NULL, 0);
}

xmlNode *
xml_util_set_child (xmlNode    *parent_node,
                    xmlNs      *namespace,
                    xmlDoc     *doc,
                    const char *name,
                    const char *value)
{
        xmlNode *node;
        xmlChar *escaped;

        node = xml_util_get_element (parent_node, name, NULL);
        if (node == NULL) {
                node = xmlNewChild (parent_node,
                                    namespace,
                                    (unsigned char *) name,
                                    NULL);
        }

        escaped = xmlEncodeSpecialChars (doc, (const unsigned char *) value);
        xmlNodeSetContent (node, escaped);
        xmlFree (escaped);

        return node;
}

gboolean
xml_util_verify_attribute_is_boolean (xmlNode    *node,
                                      const char *attribute_name)
{
        const char *content;
        char *str;

        content = xml_util_get_attribute_content (node, attribute_name);
        if (content == NULL)
            return FALSE;

        str = (char *) content;

        return g_ascii_strcasecmp (str, "true") == 0 ||
               g_ascii_strcasecmp (str, "yes") == 0 ||
               g_ascii_strcasecmp (str, "false") == 0 ||
               g_ascii_strcasecmp (str, "no") == 0 ||
               g_ascii_strcasecmp (str, "0") == 0 ||
               g_ascii_strcasecmp (str, "1") == 0;
}

