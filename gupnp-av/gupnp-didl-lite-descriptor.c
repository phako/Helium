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
 * SECTION:gupnp-didl-lite-descriptor
 * @short_description: DIDL-Lite Descriptor
 *
 * #GUPnPDIDLLiteDescriptor respresent a DIDL-Lite descriptor (desc) element.
 */

#include <string.h>

#include "gupnp-didl-lite-descriptor.h"
#include "xml-util.h"

G_DEFINE_TYPE (GUPnPDIDLLiteDescriptor,
               gupnp_didl_lite_descriptor,
               G_TYPE_OBJECT);

struct _GUPnPDIDLLiteDescriptorPrivate {
        xmlNode     *xml_node;
        GUPnPXMLDoc *xml_doc;
};

enum {
        PROP_0,
        PROP_XML_NODE,
        PROP_XML_DOC,

        PROP_ID,
        PROP_METADATA_TYPE,
        PROP_NAME_SPACE,
        PROP_CONTENT
};

static void
gupnp_didl_lite_descriptor_init (GUPnPDIDLLiteDescriptor *descriptor)
{
        descriptor->priv = G_TYPE_INSTANCE_GET_PRIVATE
                                        (descriptor,
                                         GUPNP_TYPE_DIDL_LITE_DESCRIPTOR,
                                         GUPnPDIDLLiteDescriptorPrivate);
}

static void
gupnp_didl_lite_descriptor_set_property (GObject      *object,
                                         guint         property_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
        GUPnPDIDLLiteDescriptor *descriptor;

        descriptor = GUPNP_DIDL_LITE_DESCRIPTOR (object);

        switch (property_id) {
        case PROP_XML_NODE:
                descriptor->priv->xml_node = g_value_get_pointer (value);
                break;
        case PROP_XML_DOC:
                descriptor->priv->xml_doc = g_value_dup_object (value);
                break;
        case PROP_ID:
                gupnp_didl_lite_descriptor_set_id
                                        (descriptor,
                                         g_value_get_string (value));
                break;
        case PROP_METADATA_TYPE:
                gupnp_didl_lite_descriptor_set_metadata_type
                                        (descriptor,
                                         g_value_get_string (value));
                break;
        case PROP_NAME_SPACE:
                gupnp_didl_lite_descriptor_set_name_space
                                        (descriptor,
                                         g_value_get_string (value));
                break;
        case PROP_CONTENT:
                gupnp_didl_lite_descriptor_set_content
                                        (descriptor,
                                         g_value_get_string (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_descriptor_get_property (GObject    *object,
                                         guint       property_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
        GUPnPDIDLLiteDescriptor *descriptor;

        descriptor = GUPNP_DIDL_LITE_DESCRIPTOR (object);

        switch (property_id) {
        case PROP_XML_NODE:
                g_value_set_pointer
                        (value,
                         gupnp_didl_lite_descriptor_get_xml_node (descriptor));
                break;
        case PROP_ID:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_descriptor_get_id (descriptor));
                break;
        case PROP_METADATA_TYPE:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_descriptor_get_metadata_type
                                                                (descriptor));
                break;
        case PROP_NAME_SPACE:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_descriptor_get_name_space
                                                                (descriptor));
                break;
        case PROP_CONTENT:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_descriptor_get_content (descriptor));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_descriptor_dispose (GObject *object)
{
        GObjectClass                 *object_class;
        GUPnPDIDLLiteDescriptorPrivate *priv;

        priv = GUPNP_DIDL_LITE_DESCRIPTOR (object)->priv;

        if (priv->xml_doc) {
                g_object_unref (priv->xml_doc);
                priv->xml_doc = NULL;
        }

        object_class = G_OBJECT_CLASS (gupnp_didl_lite_descriptor_parent_class);
        object_class->dispose (object);
}

static void
gupnp_didl_lite_descriptor_class_init (GUPnPDIDLLiteDescriptorClass *klass)
{
        GObjectClass *object_class;

        object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gupnp_didl_lite_descriptor_set_property;
        object_class->get_property = gupnp_didl_lite_descriptor_get_property;
        object_class->dispose = gupnp_didl_lite_descriptor_dispose;

        g_type_class_add_private (klass,
                                  sizeof (GUPnPDIDLLiteDescriptorPrivate));

        /**
         * GUPnPDIDLLiteDescriptor:xml-node
         *
         * The pointer to desc node in XML document.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_XML_NODE,
                 g_param_spec_pointer ("xml-node",
                                       "XMLNode",
                                       "The pointer to desc node in XML"
                                       " document.",
                                       G_PARAM_READWRITE |
                                       G_PARAM_CONSTRUCT_ONLY |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteDescriptor:xml-doc
         *
         * The reference to XML document containing this object.
         *
         * Internal property.
         *
         * Stability: Private
         **/
        g_object_class_install_property
                (object_class,
                 PROP_XML_DOC,
                 g_param_spec_object ("xml-doc",
                                      "XMLDoc",
                                      "The reference to XML document"
                                      " containing this object.",
                                      GUPNP_TYPE_XML_DOC,
                                      G_PARAM_WRITABLE |
                                      G_PARAM_CONSTRUCT_ONLY |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteDescriptor:name-space
         *
         * The name space associated with this descriptor.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_NAME_SPACE,
                 g_param_spec_string ("name-space",
                                      "NameSpace",
                                      "The name space associated with this"
                                      " descriptor",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteDescriptor:id
         *
         * The ID of this descriptor.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_ID,
                 g_param_spec_string ("id",
                                      "ID",
                                      "The ID of this descriptor",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteDescriptor:metadata-type
         *
         * The type of this descriptor.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_METADATA_TYPE,
                 g_param_spec_string ("metadata-type",
                                      "MetadataType",
                                      "The metadata type of this descriptor",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteDescriptor:content
         *
         * The content of this descriptor.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_CONTENT,
                 g_param_spec_string ("content",
                                      "Content",
                                      "The content of this descriptor",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));
}

/**
 * gupnp_didl_lite_descriptor_new:
 *
 * Return value: A new #GUPnPDIDLLiteDescriptor object. Unref after usage.
 **/
GUPnPDIDLLiteDescriptor *
gupnp_didl_lite_descriptor_new (void)
{
        return g_object_new (GUPNP_TYPE_DIDL_LITE_DESCRIPTOR,
                             NULL);
}

/**
 * gupnp_didl_lite_descriptor_new_from_xml:
 * @xml_node: The pointer to 'desc' node in XML document
 * @xml_doc: The reference to XML document containing this descriptor
 *
 * Creates a new #GUPnPDIDLLiteDescriptor for the @xml_node.
 *
 * Return value: A new #GUPnPDIDLLiteDescriptor object. Unref after usage.
 **/
GUPnPDIDLLiteDescriptor *
gupnp_didl_lite_descriptor_new_from_xml (xmlNode     *xml_node,
                                         GUPnPXMLDoc *xml_doc)
{
        GUPnPDIDLLiteDescriptor *descriptor;

        return g_object_new (GUPNP_TYPE_DIDL_LITE_DESCRIPTOR,
                             "xml-node", xml_node,
                             "xml-doc", xml_doc,
                             NULL);

        return descriptor;
}

/**
 * gupnp_didl_lite_descriptor_get_xml_node:
 * @descriptor: The #GUPnPDIDLLiteDescriptor
 *
 * Get the pointer to desc node in XML document.
 *
 * Return value: The pointer to desc node in XML document.
 **/
xmlNode *
gupnp_didl_lite_descriptor_get_xml_node (GUPnPDIDLLiteDescriptor *descriptor)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor), NULL);

        return descriptor->priv->xml_node;
}

/**
 * gupnp_didl_lite_descriptor_get_content:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 *
 * Get the content of the @descriptor.
 *
 * Return value: The content of the @descriptor or %NULL.
 **/
const char *
gupnp_didl_lite_descriptor_get_content (GUPnPDIDLLiteDescriptor *descriptor)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor), NULL);

        return (const char *) descriptor->priv->xml_node->children;
}

/**
 * gupnp_didl_lite_descriptor_get_id:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 *
 * Get the ID of the @descriptor.
 *
 * Return value: The ID string or %NULL.
 **/
const char *
gupnp_didl_lite_descriptor_get_id (GUPnPDIDLLiteDescriptor *descriptor)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor), NULL);

        return xml_util_get_attribute_content (descriptor->priv->xml_node,
                                               "id");
}

/**
 * gupnp_didl_lite_descriptor_get_metadata_type:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 *
 * Get the metadata type of the @descriptor.
 *
 * Return value: The type as string or %NULL.
 **/
const char *
gupnp_didl_lite_descriptor_get_metadata_type
                                        (GUPnPDIDLLiteDescriptor *descriptor)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor), NULL);

        return xml_util_get_attribute_content (descriptor->priv->xml_node,
                                               "type");
}

/**
 * gupnp_didl_lite_descriptor_get_name_space:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 *
 * Get the name space associated with the @descriptor.
 *
 * Return value: The name space or %NULL.
 **/
const char *
gupnp_didl_lite_descriptor_get_name_space (GUPnPDIDLLiteDescriptor *descriptor)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor), NULL);

        return xml_util_get_attribute_content (descriptor->priv->xml_node,
                                               "nameSpace");
}

/**
 * gupnp_didl_lite_descriptor_set_content:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 * @content: The content as string
 *
 * Set the content of the @descriptor.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_descriptor_set_content (GUPnPDIDLLiteDescriptor *descriptor,
                                        const char              *content)
{
        xmlChar *escaped;

        g_return_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor));
        g_return_if_fail (content != NULL);

        escaped = xmlEncodeSpecialChars (descriptor->priv->xml_doc->doc,
                                         (const unsigned char *) content);
        xmlNodeSetContent (descriptor->priv->xml_node, escaped);
        xmlFree (escaped);

        g_object_notify (G_OBJECT (descriptor), "content");
}

/**
 * gupnp_didl_lite_descriptor_set_id:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 * @id: The ID as string
 *
 * Set the ID of the @descriptor.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_descriptor_set_id (GUPnPDIDLLiteDescriptor *descriptor,
                                   const char              *id)
{
        g_return_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor));
        g_return_if_fail (id != NULL);

        xmlSetProp (descriptor->priv->xml_node,
                    (unsigned char *) "id",
                    (const unsigned char *) id);

        g_object_notify (G_OBJECT (descriptor), "id");
}

/**
 * gupnp_didl_lite_descriptor_set_metadata_type:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 * @type: The metadata type as string
 *
 * Set the metadata type of the @descriptor.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_descriptor_set_metadata_type
                                        (GUPnPDIDLLiteDescriptor *descriptor,
                                         const char              *type)
{
        g_return_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor));
        g_return_if_fail (type != NULL);

        xmlSetProp (descriptor->priv->xml_node,
                    (unsigned char *) "type",
                    (const unsigned char *) type);

        g_object_notify (G_OBJECT (descriptor), "metadata-type");
}

/**
 * gupnp_didl_lite_descriptor_set_name_space:
 * @descriptor: A #GUPnPDIDLLiteDescriptor
 * @name_space: The name space URI as string
 *
 * Set the name space associated with the @descriptor.
 *
 * Return value: None.
 **/
void
gupnp_didl_lite_descriptor_set_name_space (GUPnPDIDLLiteDescriptor *descriptor,
                                           const char              *name_space)
{
        g_return_if_fail (GUPNP_IS_DIDL_LITE_DESCRIPTOR (descriptor));
        g_return_if_fail (name_space != NULL);

        xmlSetProp (descriptor->priv->xml_node,
                    (unsigned char *) "nameSpace",
                    (const unsigned char *) name_space);

        g_object_notify (G_OBJECT (descriptor), "name-space");
}
