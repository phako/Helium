/*
 * Copyright (C) 2007, 2008 OpenedHand Ltd.
 *
 * Authors: Jorn Baayen <jorn@openedhand.com>
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
 * SECTION:gupnp-didl-lite-writer
 * @short_description: DIDL-Lite fragment writer
 *
 * #GUPnPDIDLLiteWriter is a helper class for writing DIDL-Lite fragments.
 */

#include <string.h>

#include "gupnp-didl-lite-writer.h"
#include "gupnp-didl-lite-object.h"
#include "gupnp-didl-lite-object-private.h"
#include "gupnp-didl-lite-descriptor-private.h"

#include "xml-util.h"

G_DEFINE_TYPE (GUPnPDIDLLiteWriter,
               gupnp_didl_lite_writer,
               G_TYPE_OBJECT);

struct _GUPnPDIDLLiteWriterPrivate {
        xmlNode     *xml_node;
        GUPnPXMLDoc *xml_doc;

        xmlNs       *upnp_ns;
        xmlNs       *dc_ns;
        xmlNs       *dlna_ns;

        char        *language;
};

enum {
        PROP_0,
        PROP_XML_NODE,
        PROP_LANGUAGE,
};

static int
compare_prop (const char *a, xmlAttr *attr)
{
        const char *p;
        char *parent_name;
        char *attr_name;
        int ret = -1;

        if (attr->ns != NULL)
                attr_name = g_strjoin (":", attr->ns->prefix, attr->name, NULL);
        else
                attr_name = g_strdup ((const char *) attr->name);

        if (attr->parent->ns != NULL)
                parent_name = g_strjoin (":",
                                         attr->parent->ns->prefix,
                                         attr->parent->name,
                                         NULL);
        else
                parent_name = g_strdup ((const char *) attr->parent->name);

        p = strstr (a, "@");
        if (p)
                if (p == a)
                        /* Top-level property */
                        ret = strcmp (a + 1, attr_name);
                else
                        ret = strncmp (a, parent_name, p - a) ||
                              strcmp (p + 1, attr_name);
        else
                ret = strcmp (a, attr_name);

        g_free (attr_name);
        g_free (parent_name);

        return ret;
}

static gboolean
is_attribute_forbidden (xmlAttr *attr,
                        GList   *allowed)
{
        return g_list_find_custom (allowed,
                                   attr,
                                   (GCompareFunc) compare_prop) == NULL;
}

static int
compare_node_name (const char *a, const char *b)
{
        const char *p;
        int len;

        if (a[0] == '@')
                /* Filer is for top-level property */
                return -1;

        p = strstr (a, "@");
        if (p != NULL)
                /* Compare only the string before '@' */
                len = p - a;
        else
                len = strlen (a);

        return strncmp (a, b, len);
}

static gboolean
is_node_forbidden (xmlNode     *node,
                    GList      *allowed,
                    const char *ns)
{
        char *name;
        gboolean ret;

        if (ns != NULL)
                name = g_strjoin (":", ns, node->name, NULL);
        else
                name = g_strdup ((const char *) node->name);

        ret = g_list_find_custom (allowed,
                                  name,
                                  (GCompareFunc) compare_node_name) == NULL;

        g_free (name);

        return ret;
}

static gboolean
is_container_standard_prop (const char *name,
                            const char *namespace,
                            const char *upnp_class)
{
        return g_strcmp0 (upnp_class, "object.container.storageFolder") == 0 &&
               g_strcmp0 (namespace, "upnp") == 0 &&
               strcmp (name, "storageUsed") == 0;
}

static gboolean
is_standard_prop (const char *name,
                  const char *namespace,
                  const char *parent_name)
{
        return strcmp (name, "id") == 0 ||
               strcmp (name, "parentID") == 0 ||
               strcmp (name, "restricted") == 0 ||
               (g_strcmp0 (namespace, "dc") == 0 &&
                strcmp (name, "title") == 0) ||
               (g_strcmp0 (namespace, "upnp") == 0 &&
                strcmp (name, "class") == 0) ||
               (g_strcmp0 (parent_name, "res") == 0 &&
                strcmp (name, "protocolInfo") == 0);
}

static void
filter_attributes (xmlNode             *node,
                   GList               *allowed,
                   GUPnPDIDLLiteWriter *writer)
{
        xmlAttr *attr;
        GList   *forbidden = NULL;
        GList   *l;

        /* Find forbidden properties */
        for (attr = node->properties; attr != NULL; attr = attr->next)
                if (!is_standard_prop ((const char *) attr->name,
                                        NULL,
                                        (const char *) attr->parent->name) &&
                    is_attribute_forbidden (attr, allowed))
                        forbidden = g_list_append (forbidden, attr);

        /* Now unset forbidden properties */
        for (l = forbidden; l != NULL; l = l->next)
                xmlRemoveProp ((xmlAttr *) l->data);

        g_list_free (forbidden);
}

static void
filter_node (xmlNode             *node,
             GList               *allowed,
             GUPnPDIDLLiteWriter *writer)
{
        xmlNode *child;
        GList   *forbidden = NULL;
        GList   *l;
        gboolean is_container;
        const char *container_class = NULL;

        filter_attributes (node, allowed, writer);

        if (strcmp ((const char *) node->name, "container") == 0) {
            is_container = TRUE;
            container_class = xml_util_get_child_element_content (node,
                                                                  "class");
        }

        forbidden = NULL;
        for (child = node->children; child != NULL; child = child->next) {
                const char *ns = NULL;

                if (xmlNodeIsText (child))
                        continue;

                if (child->ns != NULL)
                        ns = (const char *) child->ns->prefix;

                if (!(is_container && is_container_standard_prop
                                            ((const char *) child->name,
                                             ns,
                                             container_class)) &&
                    !is_standard_prop ((const char *) child->name,
                                       ns,
                                       (const char *)  node->name) &&
                    is_node_forbidden (child, allowed, ns))
                        forbidden = g_list_append (forbidden, child);
        }

        /* Now remove the forbidden nodes */
        for (l = forbidden; l != NULL; l = l->next) {
                xmlNode *n;

                n = (xmlNode *) l->data;

                xmlUnlinkNode (n);
                xmlFreeNode (n);
        }

        g_list_free (forbidden);

        /* Recurse */
        for (child = node->children; child != NULL; child = child->next)
                if (!xmlNodeIsText (child))
                        filter_node (child, allowed, writer);
}

static void
gupnp_didl_lite_writer_init (GUPnPDIDLLiteWriter *writer)
{
        writer->priv = G_TYPE_INSTANCE_GET_PRIVATE (writer,
                                                    GUPNP_TYPE_DIDL_LITE_WRITER,
                                                    GUPnPDIDLLiteWriterPrivate);
}

static void
gupnp_didl_lite_writer_set_property (GObject      *object,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)

{
        GUPnPDIDLLiteWriter *writer;

        writer = GUPNP_DIDL_LITE_WRITER (object);

        switch (property_id) {
        case PROP_LANGUAGE:
                writer->priv->language = g_value_dup_string (value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_writer_get_property (GObject    *object,
                                     guint       property_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
        GUPnPDIDLLiteWriter *writer;

        writer = GUPNP_DIDL_LITE_WRITER (object);

        switch (property_id) {
        case PROP_XML_NODE:
                g_value_set_pointer
                        (value, gupnp_didl_lite_writer_get_xml_node (writer));
                break;
        case PROP_LANGUAGE:
                g_value_set_string
                        (value, gupnp_didl_lite_writer_get_language (writer));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_writer_constructed (GObject *object)
{
        GObjectClass               *object_class;
        GUPnPDIDLLiteWriterPrivate *priv;
        xmlDoc                     *doc;

        priv = GUPNP_DIDL_LITE_WRITER (object)->priv;

        doc = xmlNewDoc ((unsigned char *) "1.0");
        priv->xml_doc = gupnp_xml_doc_new (doc);

        priv->xml_node = xmlNewDocNode (priv->xml_doc->doc,
                                        NULL,
                                        (unsigned char *) "DIDL-Lite",
                                        NULL);
        xmlDocSetRootElement (priv->xml_doc->doc, priv->xml_node);
        priv->dc_ns = xmlNewNs (priv->xml_node,
                                (unsigned char *)
                                "http://purl.org/dc/elements/1.1/",
                                (unsigned char *)
                                GUPNP_DIDL_LITE_WRITER_NAMESPACE_DC);
        priv->upnp_ns = xmlNewNs (priv->xml_node,
                                  (unsigned char *)
                                  "urn:schemas-upnp-org:metadata-1-0/upnp/",
                                  (unsigned char *)
                                  GUPNP_DIDL_LITE_WRITER_NAMESPACE_UPNP);
        priv->dlna_ns = xmlNewNs (priv->xml_node,
                                  (unsigned char *)
                                  "urn:schemas-dlna-org:metadata-1-0/",
                                  (unsigned char *)
                                  GUPNP_DIDL_LITE_WRITER_NAMESPACE_DLNA);
        xmlNewNs (priv->xml_node,
                  (unsigned char *)
                  "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/",
                  NULL);

        if (priv->language)
                xmlSetProp (priv->xml_node,
                            (unsigned char *) "lang",
                            (unsigned char *) priv->language);

        object_class = G_OBJECT_CLASS (gupnp_didl_lite_writer_parent_class);
        if (object_class->constructed != NULL)
                object_class->constructed (object);
}

static void
gupnp_didl_lite_writer_dispose (GObject *object)
{
        GObjectClass               *object_class;
        GUPnPDIDLLiteWriterPrivate *priv;

        priv = GUPNP_DIDL_LITE_WRITER (object)->priv;

        if (priv->xml_doc) {
                g_object_unref (priv->xml_doc);
                priv->xml_doc = NULL;
        }

        object_class = G_OBJECT_CLASS (gupnp_didl_lite_writer_parent_class);
        object_class->dispose (object);
}

static void
gupnp_didl_lite_writer_finalize (GObject *object)
{
        GObjectClass               *object_class;
        GUPnPDIDLLiteWriterPrivate *priv;

        priv = GUPNP_DIDL_LITE_WRITER (object)->priv;

        if (priv->language)
                g_free (priv->language);

        object_class = G_OBJECT_CLASS (gupnp_didl_lite_writer_parent_class);
        object_class->finalize (object);
}

static void
gupnp_didl_lite_writer_class_init (GUPnPDIDLLiteWriterClass *klass)
{
        GObjectClass *object_class;

        object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gupnp_didl_lite_writer_set_property;
        object_class->get_property = gupnp_didl_lite_writer_get_property;
        object_class->constructed = gupnp_didl_lite_writer_constructed;
        object_class->dispose = gupnp_didl_lite_writer_dispose;
        object_class->finalize = gupnp_didl_lite_writer_finalize;

        g_type_class_add_private (klass, sizeof (GUPnPDIDLLiteWriterPrivate));

        /**
         * GUPnPDIDLLiteWriter:xml-node:
         *
         * The pointer to root node in XML document.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_XML_NODE,
                 g_param_spec_pointer ("xml-node",
                                       "XMLNode",
                                       "The pointer to root node in XML"
                                       " document.",
                                       G_PARAM_READABLE |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteWriter:language:
         *
         * The language the DIDL-Lite fragment is in.
         *
         **/
        g_object_class_install_property
                (object_class,
                 PROP_LANGUAGE,
                 g_param_spec_string ("language",
                                      "Language",
                                      "The language the DIDL-Lite fragment"
                                      " is in.",
                                      NULL,
                                      G_PARAM_CONSTRUCT_ONLY |
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));
}

/**
 * gupnp_didl_lite_writer_new:
 * @language: The language the DIDL-Lite fragment is in, or NULL
 *
 * Return value: A new #GUPnPDIDLLiteWriter object.
 **/
GUPnPDIDLLiteWriter *
gupnp_didl_lite_writer_new (const char *language)
{
        return g_object_new (GUPNP_TYPE_DIDL_LITE_WRITER,
                             "language", language,
                             NULL);
}

/**
 * gupnp_didl_lite_writer_add_item:
 * @writer: A #GUPnPDIDLLiteWriter
 *
 * Creates a new item, attaches it to @writer and returns it.
 *
 * Returns: (transfer full): A new #GUPnPDIDLLiteItem object. Unref after usage.
 **/
GUPnPDIDLLiteItem *
gupnp_didl_lite_writer_add_item (GUPnPDIDLLiteWriter *writer)
{
        xmlNode *item_node;
        GUPnPDIDLLiteObject *object;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_WRITER (writer), NULL);

        item_node = xmlNewChild (writer->priv->xml_node,
                                NULL,
                                (unsigned char *) "item",
                                NULL);

        object = gupnp_didl_lite_object_new_from_xml (item_node,
                                                      writer->priv->xml_doc,
                                                      writer->priv->upnp_ns,
                                                      writer->priv->dc_ns,
                                                      writer->priv->dlna_ns);
        return GUPNP_DIDL_LITE_ITEM (object);
}

/**
 * gupnp_didl_lite_writer_add_container:
 * @writer: A #GUPnPDIDLLiteWriter
 *
 * Creates a new container, attaches it to @writer and returns it.
 *
 * Returns: (transfer full): A new #GUPnPDIDLLiteContainer object. Unref after usage.
 **/
GUPnPDIDLLiteContainer *
gupnp_didl_lite_writer_add_container (GUPnPDIDLLiteWriter *writer)
{
        xmlNode *container_node;
        GUPnPDIDLLiteObject *object;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_WRITER (writer), NULL);

        container_node = xmlNewChild (writer->priv->xml_node,
                                      NULL,
                                      (unsigned char *) "container",
                                      NULL);

        object = gupnp_didl_lite_object_new_from_xml (container_node,
                                                      writer->priv->xml_doc,
                                                      writer->priv->upnp_ns,
                                                      writer->priv->dc_ns,
                                                      writer->priv->dlna_ns);
        return GUPNP_DIDL_LITE_CONTAINER (object);
}

/**
 * gupnp_didl_lite_writer_add_descriptor:
 * @writer: A #GUPnPDIDLLiteWriter
 *
 * Creates a new descriptor, attaches it to @object and returns it.
 *
 * Returns: (transfer full): A new #GUPnPDIDLLiteDescriptor object. Unref after usage.
 **/
GUPnPDIDLLiteDescriptor *
gupnp_didl_lite_writer_add_descriptor (GUPnPDIDLLiteWriter *writer)
{
        xmlNode *desc_node;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_WRITER (writer), NULL);

        desc_node = xmlNewChild (writer->priv->xml_node,
                                 NULL,
                                 (unsigned char *) "desc",
                                 NULL);

        return gupnp_didl_lite_descriptor_new_from_xml (desc_node,
                                                        writer->priv->xml_doc);
}

/**
 * gupnp_didl_lite_writer_get_string:
 * @writer: A #GUPnPDIDLLiteWriter
 *
 * Creates a string representation of the DIDL-Lite XML document.
 *
 * Return value: The DIDL-Lite XML string, or %NULL. #g_free after usage.
 **/
char *
gupnp_didl_lite_writer_get_string (GUPnPDIDLLiteWriter *writer)
{
        xmlBuffer *buffer;
        char      *ret;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_WRITER (writer), NULL);

        buffer = xmlBufferCreate ();
        xmlNodeDump (buffer,
                     writer->priv->xml_doc->doc,
                     writer->priv->xml_node,
                     0,
                     0);
        ret = g_strndup ((char *) xmlBufferContent (buffer),
                         xmlBufferLength (buffer));
        xmlBufferFree (buffer);

        return ret;
}

/**
 * gupnp_didl_lite_writer_get_xml_node:
 * @writer: The #GUPnPDIDLLiteWriter
 *
 * Get the pointer to root node in XML document.
 *
 * Returns: (transfer none): The pointer to root node in XML document.
 **/
xmlNode *
gupnp_didl_lite_writer_get_xml_node (GUPnPDIDLLiteWriter *writer)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_WRITER (writer), NULL);

        return writer->priv->xml_node;
}

/**
 * gupnp_didl_lite_writer_get_language:
 * @writer: #GUPnPDIDLLiteWriter
 *
 * Get the language the DIDL-Lite fragment is in.
 *
 * Returns: (transfer none): The language of the @writer, or %NULL.
 **/
const char *
gupnp_didl_lite_writer_get_language (GUPnPDIDLLiteWriter *writer)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_WRITER (writer), NULL);

        return writer->priv->language;
}

/**
 * gupnp_didl_lite_writer_filter:
 * @writer: A #GUPnPDIDLLiteWriter
 * @filter: A filter string
 *
 * Clears the DIDL-Lite XML document of the properties not specified in the
 * @filter. The passed filter string would typically come from the 'Filter'
 * argument of Browse or Search actions from a ContentDirectory control point.
 * Please refer to Section 2.3.15 of UPnP AV ContentDirectory version 3
 * specification for details on this string.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_writer_filter (GUPnPDIDLLiteWriter *writer,
                               const char          *filter)
{
        char **tokens;
        GList *allowed = NULL;
        unsigned short i;
        xmlNode *node;

        g_return_if_fail (GUPNP_IS_DIDL_LITE_WRITER (writer));
        g_return_if_fail (filter != NULL);

        if (filter[0] == '*')
                return;         /* Wildcard */

        tokens = g_strsplit (filter, ",", -1);
        g_return_if_fail (tokens != NULL);

        for (i = 0; tokens[i] != NULL; i++)
                allowed = g_list_append (allowed, tokens[i]);

        for (node = writer->priv->xml_node->children;
             node != NULL;
             node = node->next)
                filter_node (node, allowed, writer);

        g_list_free (allowed);
        g_strfreev (tokens);
}
