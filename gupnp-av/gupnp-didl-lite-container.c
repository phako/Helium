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

/**
 * SECTION:gupnp-didl-lite-container
 * @short_description: DIDL-Lite Container
 *
 * #GUPnPDIDLLiteContainer respresents a DIDL-Lite container element.
 */

#include <string.h>

#include "gupnp-didl-lite-container.h"
#include "xml-util.h"

G_DEFINE_TYPE (GUPnPDIDLLiteContainer,
               gupnp_didl_lite_container,
               GUPNP_TYPE_DIDL_LITE_OBJECT);

enum {
        PROP_0,
        PROP_SEARCHABLE,
        PROP_CHILD_COUNT,
        PROP_STORAGE_USED
};

static void
gupnp_didl_lite_container_init (GUPnPDIDLLiteContainer *container)
{
        /* Nothing to initialize, yay! */
}

static void
gupnp_didl_lite_container_get_property (GObject    *object,
                                        guint       property_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
        GUPnPDIDLLiteContainer *container;

        container = GUPNP_DIDL_LITE_CONTAINER (object);

        switch (property_id) {
        case PROP_SEARCHABLE:
                g_value_set_boolean
                        (value,
                         gupnp_didl_lite_container_get_searchable (container));
                break;
        case PROP_CHILD_COUNT:
                g_value_set_int
                        (value,
                         gupnp_didl_lite_container_get_child_count (container));
                break;
        case PROP_STORAGE_USED:
                g_value_set_long
                        (value,
                         gupnp_didl_lite_container_get_storage_used (container));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_container_set_property (GObject      *object,
                                        guint         property_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)

{
        GUPnPDIDLLiteContainer *container;

        container = GUPNP_DIDL_LITE_CONTAINER (object);

        switch (property_id) {
        case PROP_SEARCHABLE:
                gupnp_didl_lite_container_set_searchable
                                        (container,
                                         g_value_get_boolean (value));
                break;
        case PROP_CHILD_COUNT:
                gupnp_didl_lite_container_set_child_count
                                        (container,
                                         g_value_get_int (value));
                break;
        case PROP_STORAGE_USED:
                gupnp_didl_lite_container_set_storage_used
                                        (container,
                                         g_value_get_int64 (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_container_class_init (GUPnPDIDLLiteContainerClass *klass)
{
        GObjectClass *object_class;

        object_class = G_OBJECT_CLASS (klass);

        object_class->get_property = gupnp_didl_lite_container_get_property;
        object_class->set_property = gupnp_didl_lite_container_set_property;

        /**
         * GUPnPDIDLLiteContainer:searchable:
         *
         * Whether this container is searchable.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_SEARCHABLE,
                 g_param_spec_boolean ("searchable",
                                       "Searchable",
                                       "Whether this container is searchable.",
                                       FALSE,
                                       G_PARAM_READWRITE |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteContainer:child-count:
         *
         * The child count of this container.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_CHILD_COUNT,
                 g_param_spec_int ("child-count",
                                   "ChildCount",
                                   "The child count of this container.",
                                   0,
                                   G_MAXINT,
                                   0,
                                   G_PARAM_READWRITE |
                                   G_PARAM_STATIC_NAME |
                                   G_PARAM_STATIC_NICK |
                                   G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteContainer:storage-used:
         *
         * The number of bytes used by all child items of this container.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_STORAGE_USED,
                 g_param_spec_int64 ("storage-used",
                                     "Storage Used",
                                     "The Number of bytes used by all child "
                                     "items of this container.",
                                     -1,
                                     G_MAXINT64,
                                     -1,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_NAME |
                                     G_PARAM_STATIC_NICK |
                                     G_PARAM_STATIC_BLURB));
}

/**
 * gupnp_didl_lite_container_get_searchable:
 * @container: #GUPnPDIDLLiteContainer
 *
 * Checks whether @container is searchable.
 *
 * Return value: #TRUE if @container is searchable.
 **/
gboolean
gupnp_didl_lite_container_get_searchable (GUPnPDIDLLiteContainer *container)
{
        xmlNode *xml_node;

        g_return_val_if_fail (container != NULL, FALSE);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container), FALSE);

        xml_node = gupnp_didl_lite_object_get_xml_node
                                (GUPNP_DIDL_LITE_OBJECT (container));

        return xml_util_get_boolean_attribute (xml_node, "searchable");
}

/**
 * gupnp_didl_lite_container_get_child_count:
 * @container: #GUPnPDIDLLiteContainer
 *
 * Get the child count of the @container.  If the child count is unknown, -1 is
 * returned.
 *
 * Return value: The child count of the @container, or -1 if it is unknown.
 **/
gint
gupnp_didl_lite_container_get_child_count (GUPnPDIDLLiteContainer *container)
{
        xmlNode *xml_node;

        g_return_val_if_fail (container != NULL, 0);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container), 0);

        xml_node = gupnp_didl_lite_object_get_xml_node
                                (GUPNP_DIDL_LITE_OBJECT (container));

        return xml_util_get_int_attribute (xml_node, "childCount", -1);
}

/**
 * gupnp_didl_lite_container_get_create_classes:
 * @container: #GUPnPDIDLLiteContainer
 *
 * Gets the list of create classes of the @object.
 *
 * Returns: (element-type utf8) (transfer full): The list of create classes
 * belonging to @object, or %NULL.
 * #g_list_free the returned list after usage and #g_free each string in it.
 **/
GList *
gupnp_didl_lite_container_get_create_classes (GUPnPDIDLLiteContainer *container)
{
        GList *classes = NULL;
        GList *ret = NULL;
        GList *l;

        g_return_val_if_fail (container != NULL, NULL);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container), NULL);

        classes = gupnp_didl_lite_object_get_properties (
                        GUPNP_DIDL_LITE_OBJECT (container),
                        "createClass");

        for (l = classes; l; l = l->next) {
                char *create_class;
                xmlNode *node;

                node = (xmlNode *) l->data;
                create_class = g_strdup ((const char *) node->content);

                ret = g_list_append (ret, create_class);
        }

        g_list_free (classes);

        return ret;
}

/**
 * gupnp_didl_lite_container_get_search_classes:
 * @container: #GUPnPDIDLLiteContainer
 *
 * Gets the list of search classes of the @object.
 *
 * Return value: (element-type utf8) (transfer full): The list of search classes
 * belonging to @object, or %NULL. #g_list_free the returned list after usage
 * and #g_free each string in it.
 **/
GList *
gupnp_didl_lite_container_get_search_classes (GUPnPDIDLLiteContainer *container)
{
        GList *classes = NULL;
        GList *ret = NULL;
        GList *l;

        g_return_val_if_fail (container != NULL, NULL);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container), NULL);

        classes = gupnp_didl_lite_object_get_properties (
                        GUPNP_DIDL_LITE_OBJECT (container),
                        "searchClass");

        for (l = classes; l; l = l->next) {
                char *search_class;
                xmlNode *node;

                node = (xmlNode *) l->data;
                search_class = g_strdup ((const char *) node->content);

                ret = g_list_append (ret, search_class);
        }

        g_list_free (classes);

        return ret;
}

/**
 * gupnp_didl_lite_container_get_storage_used:
 * @container: #GUPnPDIDLLiteContainer
 *
 * Get the number of bytes used by all child items of the @container.
 * If storage used is unknown, -1 is returned.
 *
 * Return value: The number of bytes used by all children of the @container,
 * or -1 if it is unknown.
 **/
gint64
gupnp_didl_lite_container_get_storage_used (GUPnPDIDLLiteContainer *container)
{
        GList *storage = NULL;
        xmlNode *xml_node;
        const char *str;

        g_return_val_if_fail (container != NULL, 0);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container), 0);

        storage = gupnp_didl_lite_object_get_properties (
                                        GUPNP_DIDL_LITE_OBJECT (container),
                                        "storageUsed");
        if (storage == NULL)
                return -1;

        /* only return value from first node */
        xml_node = (xmlNode *) storage->data;

        g_list_free (storage);

        str = (const char *) xml_node->content;

        return g_ascii_strtoll (str, NULL, 10);
}
/**
 * gupnp_didl_lite_container_set_searchable:
 * @container: #GUPnPDIDLLiteContainer
 * @searchable: The searchibility
 *
 * (Un)set the searchibility of @container.
 **/
void
gupnp_didl_lite_container_set_searchable (GUPnPDIDLLiteContainer *container,
                                          gboolean                searchable)
{
        xmlNode *xml_node;
        const char *str;

        g_return_if_fail (container != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container));

        xml_node = gupnp_didl_lite_object_get_xml_node
                                (GUPNP_DIDL_LITE_OBJECT (container));

        if (searchable)
                str = "1";
        else
                str = "0";
        xmlSetProp (xml_node,
                    (unsigned char *) "searchable",
                    (unsigned char *) str);

        g_object_notify (G_OBJECT (container), "searchable");
}

/**
 * gupnp_didl_lite_container_set_child_count:
 * @container: #GUPnPDIDLLiteContainer
 * @child_count: The child count
 *
 * Set the child count of the @container.
 **/
void
gupnp_didl_lite_container_set_child_count (GUPnPDIDLLiteContainer *container,
                                           gint                    child_count)
{
        xmlNode *xml_node;
        char *str;

        g_return_if_fail (container != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container));

        xml_node = gupnp_didl_lite_object_get_xml_node
                                (GUPNP_DIDL_LITE_OBJECT (container));

        str = g_strdup_printf ("%d", child_count);
        xmlSetProp (xml_node,
                    (unsigned char *) "childCount",
                    (unsigned char *) str);
        g_free (str);

        g_object_notify (G_OBJECT (container), "child-count");
}

/**
 * gupnp_didl_lite_container_add_create_class:
 * @container: #GUPnPDIDLLiteContainer
 * @create_class: The createClass to add.
 *
 * Add a new create class to the @object. includeDerived defaults to "0".
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_container_add_create_class (
                GUPnPDIDLLiteContainer *container,
                const char             *create_class)
{
        gupnp_didl_lite_container_add_create_class_full (container,
                                                         create_class,
                                                         FALSE);
}

/**
 * gupnp_didl_lite_container_add_create_class_full:
 * @container: #GUPnPDIDLLiteContainer
 * @create_class: The createClass to add.
 * @include_derived: Whether object with dervied classes may be created in
 * this container or not.
 *
 * Add a new create class to the @object.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_container_add_create_class_full (
                GUPnPDIDLLiteContainer *container,
                const char             *create_class,
                gboolean                include_derived)
{
        xmlNode *container_node, *new_node;
        xmlNs *namespace;
        const char *str;

        g_return_if_fail (container != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container));

        container_node = gupnp_didl_lite_object_get_xml_node
                                (GUPNP_DIDL_LITE_OBJECT (container));
        namespace = gupnp_didl_lite_object_get_upnp_namespace
                                (GUPNP_DIDL_LITE_OBJECT (container));

        new_node = xmlNewChild (container_node,
                                namespace,
                                (unsigned char *) "createClass",
                                (unsigned char *) create_class);
        if (include_derived)
                str = "1";
        else
                str = "0";

        xmlSetProp (new_node,
                    (unsigned char *) "includeDerived",
                    (unsigned char *) str);
}

/**
 * gupnp_didl_lite_container_add_search_class:
 * @container: #GUPnPDIDLLiteContainer
 * @search_class: The searchClass to add.
 *
 * Add a new search class to the @object.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_container_add_search_class (
                GUPnPDIDLLiteContainer *container,
                const char             *search_class)
{
        gupnp_didl_lite_container_add_search_class_full (container,
                                                         search_class,
                                                         TRUE);
}

/**
 * gupnp_didl_lite_container_add_search_class_full:
 * @container: #GUPnPDIDLLiteContainer
 * @search_class: The searchClass to add.
 * @include_derived: includeDerived attribute of the DIDL
 *
 * Add a new search class to the @object.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_container_add_search_class_full (
                GUPnPDIDLLiteContainer *container,
                const char             *search_class,
                gboolean                include_derived)
{
        xmlNode *xml_node;
        xmlNode *new_xml_node;
        xmlNs *namespace;
        const char *str;

        g_return_if_fail (container != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container));

        xml_node = gupnp_didl_lite_object_get_xml_node
                                (GUPNP_DIDL_LITE_OBJECT (container));
        namespace = gupnp_didl_lite_object_get_upnp_namespace
                                (GUPNP_DIDL_LITE_OBJECT (container));

        new_xml_node = xmlNewChild (xml_node,
                                    namespace,
                                    (unsigned char *) "searchClass",
                                    (unsigned char *) search_class);

        if (include_derived)
                str = "1";
        else
                str = "0";

        xmlSetProp (new_xml_node,
                    (unsigned char*) "includeDerived",
                    (unsigned char*) str);
}

/**
 * gupnp_didl_lite_container_set_storage_used:
 * @container: #GUPnPDIDLLiteContainer
 * @storage_used: The number of bytes used by all child items of the
 *                @container or -1 if unknown.
 *
 * Set the number of bytes used by all child items of the @container.
 **/
void
gupnp_didl_lite_container_set_storage_used (
                GUPnPDIDLLiteContainer *container,
                gint64                  storage_used)
{
        GList *storage = NULL;
        xmlNode *xml_node;
        xmlNs *namespace;
        char *str;

        g_return_if_fail (container != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_CONTAINER (container));

        xml_node = gupnp_didl_lite_object_get_xml_node
                                (GUPNP_DIDL_LITE_OBJECT (container));

        namespace = gupnp_didl_lite_object_get_upnp_namespace
                                (GUPNP_DIDL_LITE_OBJECT (container));

        str = g_strdup_printf ("%"G_GINT64_FORMAT, storage_used);

        storage = gupnp_didl_lite_object_get_properties (
                                        GUPNP_DIDL_LITE_OBJECT (container),
                                        "storageUsed");
        if (storage == NULL)
                xmlNewChild (xml_node,
                             namespace,
                             (unsigned char *) "storageUsed",
                             (unsigned char *) str);
        else
                xmlNodeSetContent ((xmlNode *) storage->data,
                                   (unsigned char *) str);

        g_free (str);

        g_object_notify (G_OBJECT (container), "storage-used");
}
