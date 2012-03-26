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
 * SECTION:gupnp-didl-lite-object
 * @short_description: DIDL-Lite Object
 *
 * #GUPnPDIDLLiteObject respresent a DIDL-Lite object element.
 */

#include <string.h>
#include <libgupnp/gupnp.h>

#include "gupnp-didl-lite-object.h"
#include "gupnp-didl-lite-object-private.h"
#include "gupnp-didl-lite-resource-private.h"
#include "gupnp-didl-lite-descriptor-private.h"
#include "gupnp-didl-lite-container.h"
#include "gupnp-didl-lite-item.h"
#include "gupnp-didl-lite-contributor-private.h"
#include "xml-util.h"

G_DEFINE_ABSTRACT_TYPE (GUPnPDIDLLiteObject,
                        gupnp_didl_lite_object,
                        G_TYPE_OBJECT);

struct _GUPnPDIDLLiteObjectPrivate {
        xmlNode     *xml_node;
        GUPnPXMLDoc *xml_doc;

        xmlNs *upnp_ns;
        xmlNs *dc_ns;
        xmlNs *dlna_ns;
};

enum {
        PROP_0,
        PROP_XML_NODE,
        PROP_XML_DOC,
        PROP_UPNP_NAMESPACE,
        PROP_DC_NAMESPACE,
        PROP_DLNA_NAMESPACE,
        PROP_ID,
        PROP_PARENT_ID,
        PROP_RESTRICTED,
        PROP_TITLE,
        PROP_UPNP_CLASS,
        PROP_CREATOR,
        PROP_ARTIST,
        PROP_AUTHOR,
        PROP_GENRE,
        PROP_WRITE_STATUS,
        PROP_ALBUM,
        PROP_ALBUM_ART,
        PROP_DESCRIPTION,
        PROP_DATE,
        PROP_TRACK_NUMBER,
        PROP_DLNA_MANAGED,
};

static int
is_non_transcoded_resource (GUPnPDIDLLiteResource *resource)
{
        GUPnPProtocolInfo *info;

        info = gupnp_didl_lite_resource_get_protocol_info (resource);
        if (G_UNLIKELY (info == NULL))
                return -1;

        return gupnp_protocol_info_get_dlna_conversion (info) &
               GUPNP_DLNA_CONVERSION_TRANSCODED;
}

static void
gupnp_didl_lite_object_init (GUPnPDIDLLiteObject *object)
{
        object->priv = G_TYPE_INSTANCE_GET_PRIVATE
                                        (object,
                                         GUPNP_TYPE_DIDL_LITE_OBJECT,
                                         GUPnPDIDLLiteObjectPrivate);
}

static void
gupnp_didl_lite_object_set_property (GObject      *object,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)

{
        GUPnPDIDLLiteObject *didl_object;

        didl_object = GUPNP_DIDL_LITE_OBJECT (object);

        switch (property_id) {
        case PROP_XML_NODE:
                didl_object->priv->xml_node = g_value_get_pointer (value);
                break;
        case PROP_XML_DOC:
                didl_object->priv->xml_doc = g_value_dup_object (value);
                break;
        case PROP_UPNP_NAMESPACE:
                didl_object->priv->upnp_ns = g_value_get_pointer (value);
                break;
        case PROP_DC_NAMESPACE:
                didl_object->priv->dc_ns = g_value_get_pointer (value);
                break;
        case PROP_DLNA_NAMESPACE:
                didl_object->priv->dlna_ns = g_value_get_pointer (value);
                break;
        case PROP_ID:
                gupnp_didl_lite_object_set_id (didl_object,
                                               g_value_get_string (value));
                break;
        case PROP_PARENT_ID:
                gupnp_didl_lite_object_set_parent_id
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_RESTRICTED:
                gupnp_didl_lite_object_set_restricted
                                        (didl_object,
                                         g_value_get_boolean (value));
                break;
        case PROP_TITLE:
                gupnp_didl_lite_object_set_title
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_UPNP_CLASS:
                gupnp_didl_lite_object_set_upnp_class
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_CREATOR:
                gupnp_didl_lite_object_set_creator
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_ARTIST:
                gupnp_didl_lite_object_set_artist
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_AUTHOR:
                gupnp_didl_lite_object_set_author
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_GENRE:
                gupnp_didl_lite_object_set_genre
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_WRITE_STATUS:
                gupnp_didl_lite_object_set_write_status
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_ALBUM:
                gupnp_didl_lite_object_set_album
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_ALBUM_ART:
                gupnp_didl_lite_object_set_album_art
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_DESCRIPTION:
                gupnp_didl_lite_object_set_description
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_DATE:
                gupnp_didl_lite_object_set_date
                                        (didl_object,
                                         g_value_get_string (value));
                break;
        case PROP_TRACK_NUMBER:
                gupnp_didl_lite_object_set_track_number
                                        (didl_object,
                                         g_value_get_int (value));
                break;
        case PROP_DLNA_MANAGED:
                gupnp_didl_lite_object_set_dlna_managed
                                        (didl_object,
                                         g_value_get_flags (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_object_get_property (GObject    *object,
                                     guint       property_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
        GUPnPDIDLLiteObject *didl_object;

        didl_object = GUPNP_DIDL_LITE_OBJECT (object);

        switch (property_id) {
        case PROP_XML_NODE:
                g_value_set_pointer
                        (value,
                         gupnp_didl_lite_object_get_xml_node (didl_object));
                break;
        case PROP_UPNP_NAMESPACE:
                g_value_set_pointer
                        (value,
                         gupnp_didl_lite_object_get_upnp_namespace
                                (didl_object));
                break;
        case PROP_DC_NAMESPACE:
                g_value_set_pointer
                        (value,
                         gupnp_didl_lite_object_get_dc_namespace
                                (didl_object));
        case PROP_DLNA_NAMESPACE:
                g_value_set_pointer
                        (value,
                         gupnp_didl_lite_object_get_dlna_namespace
                                (didl_object));
                break;
        case PROP_ID:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_id (didl_object));
                break;
        case PROP_PARENT_ID:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_parent_id (didl_object));
                break;
        case PROP_RESTRICTED:
                g_value_set_boolean
                        (value,
                         gupnp_didl_lite_object_get_restricted (didl_object));
                break;
        case PROP_TITLE:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_title (didl_object));
                break;
        case PROP_UPNP_CLASS:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_upnp_class (didl_object));
                break;
        case PROP_CREATOR:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_creator (didl_object));
                break;
        case PROP_ARTIST:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_artist (didl_object));
                break;
        case PROP_AUTHOR:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_author (didl_object));
                break;
        case PROP_GENRE:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_genre (didl_object));
                break;
        case PROP_WRITE_STATUS:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_write_status (didl_object));
                break;
        case PROP_ALBUM:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_album (didl_object));
                break;
        case PROP_ALBUM_ART:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_album_art (didl_object));
                break;
        case PROP_DESCRIPTION:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_description (didl_object));
                break;
        case PROP_DATE:
                g_value_set_string
                        (value,
                         gupnp_didl_lite_object_get_date (didl_object));
                break;
        case PROP_TRACK_NUMBER:
                g_value_set_int
                        (value,
                         gupnp_didl_lite_object_get_track_number (didl_object));
                break;
        case PROP_DLNA_MANAGED:
                g_value_set_flags
                        (value,
                         gupnp_didl_lite_object_get_dlna_managed (didl_object));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_didl_lite_object_dispose (GObject *object)
{
        GObjectClass               *object_class;
        GUPnPDIDLLiteObjectPrivate *priv;

        priv = GUPNP_DIDL_LITE_OBJECT (object)->priv;

        if (priv->xml_doc) {
                g_object_unref (priv->xml_doc);
                priv->xml_doc = NULL;
        }

        object_class = G_OBJECT_CLASS (gupnp_didl_lite_object_parent_class);
        object_class->dispose (object);
}

static void
gupnp_didl_lite_object_class_init (GUPnPDIDLLiteObjectClass *klass)
{
        GObjectClass *object_class;

        object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gupnp_didl_lite_object_set_property;
        object_class->get_property = gupnp_didl_lite_object_get_property;
        object_class->dispose = gupnp_didl_lite_object_dispose;

        g_type_class_add_private (klass, sizeof (GUPnPDIDLLiteObjectPrivate));

        /**
         * GUPnPDIDLLiteObject:xml-node
         *
         * The pointer to object node in XML document.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_XML_NODE,
                 g_param_spec_pointer ("xml-node",
                                       "XMLNode",
                                       "The pointer to object node in XML"
                                       " document.",
                                       G_PARAM_READWRITE |
                                       G_PARAM_CONSTRUCT_ONLY |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:xml-doc
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
         * GUPnPDIDLLiteObject:upnp-namespace
         *
         * Pointer to the UPnP namespace registered with the XML document
         * containing this object.
         *
         **/
        g_object_class_install_property
                (object_class,
                 PROP_UPNP_NAMESPACE,
                 g_param_spec_pointer ("upnp-namespace",
                                       "XML namespace",
                                       "Pointer to the UPnP XML namespace "
                                       "registered with the XML document "
                                       "containing this object.",
                                       G_PARAM_READWRITE |
                                       G_PARAM_CONSTRUCT_ONLY |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:dc-namespace
         *
         * Pointer to the DublinCore namespace registered with the XML document
         * containing this object.
         *
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DC_NAMESPACE,
                 g_param_spec_pointer ("dc-namespace",
                                       "XML namespace",
                                       "Pointer to the Dublin Core XML "
                                       "namespace registered with the XML "
                                       "document containing this object.",
                                       G_PARAM_READWRITE |
                                       G_PARAM_CONSTRUCT_ONLY |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:dlna-namespace
         *
         * Pointer to the DLNA metadata namespace registered with the XML
         * document containing this object.
         *
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DLNA_NAMESPACE,
                 g_param_spec_pointer ("dlna-namespace",
                                       "XML namespace",
                                       "Pointer to the DLNA metadata namespace "
                                       "registered with the XML document "
                                       "containing this object.",
                                       G_PARAM_READWRITE |
                                       G_PARAM_CONSTRUCT_ONLY |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:id
         *
         * The ID of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_ID,
                 g_param_spec_string ("id",
                                      "ID",
                                      "The ID of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:parent-id
         *
         * The ID of the parent container of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_PARENT_ID,
                 g_param_spec_string ("parent-id",
                                      "ParentID",
                                      "The ID of the parent container of"
                                      " this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:restricted
         *
         * Whether this object is restricted.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_RESTRICTED,
                 g_param_spec_boolean ("restricted",
                                       "Restricted",
                                       "Whether this object is restricted.",
                                       FALSE,
                                       G_PARAM_READWRITE |
                                       G_PARAM_STATIC_NAME |
                                       G_PARAM_STATIC_NICK |
                                       G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:title
         *
         * The title of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_TITLE,
                 g_param_spec_string ("title",
                                      "Title",
                                      "The title of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:upnp-class
         *
         * The UPnP class of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_UPNP_CLASS,
                 g_param_spec_string ("upnp-class",
                                      "UPnPClassName",
                                      "The UPnP class of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:creator
         *
         * The creator of this object.
         *
         * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_get_creators and
         * #gupnp_didl_lite_object_add_creator instead since unlike this
         * property, they are capable of dealing with multiple creator nodes.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_CREATOR,
                 g_param_spec_string ("creator",
                                      "Creator",
                                      "The creator of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:artist
         *
         * The artist of this object.
         *
         * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_get_artists and
         * #gupnp_didl_lite_object_add_artist instead since unlike this
         * property, they are capable of dealing with multiple artist nodes.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_ARTIST,
                 g_param_spec_string ("artist",
                                      "Artist",
                                      "The artist of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:author
         *
         * The author of this object.
         *
         * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_get_authors and
         * #gupnp_didl_lite_object_add_author instead since unlike this
         * property, they are capable of dealing with multiple author nodes.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_AUTHOR,
                 g_param_spec_string ("author",
                                      "Author",
                                      "The author of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:genre
         *
         * The genre of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_GENRE,
                 g_param_spec_string ("genre",
                                      "Genre",
                                      "The genre of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:write-status
         *
         * The write status of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_WRITE_STATUS,
                 g_param_spec_string ("write-status",
                                      "WriteStatus",
                                      "The write status of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:album
         *
         * The album of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_ALBUM,
                 g_param_spec_string ("album",
                                      "Album",
                                      "The album of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:album-art
         *
         * The URI to album art of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_ALBUM_ART,
                 g_param_spec_string ("album-art",
                                      "AlbumArt",
                                      "The URI to album art of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:description
         *
         * The description of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DESCRIPTION,
                 g_param_spec_string ("description",
                                      "Description",
                                      "The description of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:date
         *
         * The date of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DATE,
                 g_param_spec_string ("date",
                                      "Date",
                                      "The date of this object.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:track-number
         *
         * The original track number of this object.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_TRACK_NUMBER,
                 g_param_spec_int ("track-number",
                                   "TrackNumber",
                                   "The original track number of this  object.",
                                   -1, G_MAXINT, -1,
                                   G_PARAM_READWRITE |
                                   G_PARAM_STATIC_NAME |
                                   G_PARAM_STATIC_NICK |
                                   G_PARAM_STATIC_BLURB));

        /**
         * GUPnPDIDLLiteObject:dlna-managed
         *
         * The 'dlna:dlnaManaged' attribute.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DLNA_MANAGED,
                 g_param_spec_flags ("dlna-managed",
                                     "DLNAManaged",
                                     "The 'dlna:dlnaManaged' attribute",
                                     GUPNP_TYPE_OCM_FLAGS,
                                     GUPNP_OCM_FLAGS_NONE,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_NAME |
                                     G_PARAM_STATIC_NICK |
                                     G_PARAM_STATIC_BLURB));
}

static gboolean
is_resource_compatible (GUPnPDIDLLiteResource *resource,
                        const char            *sink_protocol_info)
{
        gboolean ret = FALSE;
        char **protocols;
        guint8 i;

        protocols = g_strsplit (sink_protocol_info, ",", 0);

        for (i = 0; protocols[i] && !ret; i++) {
                GUPnPProtocolInfo *info;
                GUPnPProtocolInfo *res_info;

                info = gupnp_protocol_info_new_from_string (protocols[i], NULL);
                if (info == NULL)
                        continue;

                res_info = gupnp_didl_lite_resource_get_protocol_info
                                                        (resource);
                ret = gupnp_protocol_info_is_compatible (info, res_info);

                g_object_unref (info);
        }

        g_strfreev (protocols);

        return ret;
}

static GList *
get_contributor_list_by_name (GUPnPDIDLLiteObject *object,
                              const char          *name)
{
        GList *contributors = NULL;
        GList *ret = NULL;
        GList *l;

        contributors = gupnp_didl_lite_object_get_properties (object, name);

        for (l = contributors; l; l = l->next) {
                GUPnPDIDLLiteContributor *contributor;
                xmlNode *contributor_node;

                contributor_node = (xmlNode *) l->data;

                contributor = gupnp_didl_lite_contributor_new_from_xml
                                        (contributor_node,
                                         object->priv->xml_doc);

                ret = g_list_append (ret, contributor);
        }

        g_list_free (contributors);

        return ret;
}

/**
 * gupnp_didl_lite_object_new_from_xml:
 * @xml_node: The pointer to 'res' node in XML document
 * @xml_doc: The reference to XML document containing this object
 * @upnp_ns: The pointer to 'upnp' namespace in XML document
 * @dc_ns: The pointer to 'dc' namespace in XML document
 * @dlna_ns: The pointer to 'dlna' namespace in XML document
 *
 * Creates a new #GUPnPDIDLLiteObject for the @xml_node.
 *
 * Return value: A new #GUPnPDIDLLiteObject object. Unref after usage.
 **/
GUPnPDIDLLiteObject *
gupnp_didl_lite_object_new_from_xml (xmlNode     *xml_node,
                                     GUPnPXMLDoc *xml_doc,
                                     xmlNs       *upnp_ns,
                                     xmlNs       *dc_ns,
                                     xmlNs       *dlna_ns)
{
        g_return_val_if_fail (xml_node != NULL, NULL);
        g_return_val_if_fail (xml_node->name != NULL, NULL);
        g_return_val_if_fail (upnp_ns != NULL, NULL);
        g_return_val_if_fail (dc_ns != NULL, NULL);
        g_return_val_if_fail (dlna_ns != NULL, NULL);

        if (g_ascii_strcasecmp ((char *) xml_node->name, "container") == 0)
                return g_object_new (GUPNP_TYPE_DIDL_LITE_CONTAINER,
                                     "xml-node", xml_node,
                                     "xml-doc", xml_doc,
                                     "upnp-namespace", upnp_ns,
                                     "dc-namespace", dc_ns,
                                     "dlna-namespace", dlna_ns,
                                     NULL);
        else if (g_ascii_strcasecmp ((char *) xml_node->name, "item") == 0)
                return g_object_new (GUPNP_TYPE_DIDL_LITE_ITEM,
                                     "xml-node", xml_node,
                                     "xml-doc", xml_doc,
                                     "upnp-namespace", upnp_ns,
                                     "dc-namespace", dc_ns,
                                     "dlna-namespace", dlna_ns,
                                     NULL);
        else
                return NULL;
}

/**
 * gupnp_didl_lite_object_get_xml_node:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Get the pointer to object node in XML document.
 *
 * Return value: The pointer to object node in XML document.
 **/
xmlNode *
gupnp_didl_lite_object_get_xml_node (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return object->priv->xml_node;
}

/**
 * gupnp_didl_lite_object_get_upnp_namespace:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Get the pointer to the UPnP namespace registered with the XML document.
 *
 * Return value: The pointer to UPnP namespace in XML document.
 **/
xmlNsPtr
gupnp_didl_lite_object_get_upnp_namespace (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return object->priv->upnp_ns;
}

/**
 * gupnp_didl_lite_object_get_dc_namespace:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Get the pointer to the DublinCore namespace registered with the XML document
 * containing this object.
 *
 * Return value: The pointer to DublinCore namespace in XML document.
 **/
xmlNsPtr
gupnp_didl_lite_object_get_dc_namespace (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return object->priv->dc_ns;
}

/**
 * gupnp_didl_lite_object_get_upnp_class:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Get the UPnP class of the @object.
 *
 * Return value: The class of @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_upnp_class (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "class");
}

/**
 * gupnp_didl_lite_object_get_dlna_namespace:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Get the pointer to the DLNA metadata namespace registered with the XML
 * document containing this object.
 *
 * Return value: The pointer to DLNA namespace in XML document.
 **/
xmlNsPtr
gupnp_didl_lite_object_get_dlna_namespace (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return object->priv->dlna_ns;
}

/**
 * gupnp_didl_lite_object_get_id
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the ID of the @object.
 *
 * Return value: The ID of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_id (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_attribute_content (object->priv->xml_node, "id");
}

/**
 * gupnp_didl_lite_object_get_parent_id:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the ID of the parent of the @object.
 *
 * Return value: The ID of parent of the @object, or %NULL. #g_free after
 * usage.
 **/
const char *
gupnp_didl_lite_object_get_parent_id (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_attribute_content (object->priv->xml_node,
                                               "parentID");
}

/**
 * gupnp_didl_lite_object_get_properties:
 * @object: #GUPnPDIDLLiteObject
 * @name: name of the properties
 *
 * Use this function to retreive property nodes by name.
 *
 * Return value: (element-type utf8) (transfer none): The list of property nodes
 * by the name @property_name belonging to @object, or %NULL. #g_list_free the
 * returned list after usage but do not modify the contents.
 **/
GList *
gupnp_didl_lite_object_get_properties (GUPnPDIDLLiteObject *object,
                                       const char          *name)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);
        g_return_val_if_fail (name != NULL, NULL);

        return xml_util_get_child_elements_by_name (object->priv->xml_node,
                                                    name);
}

/**
 * gupnp_didl_lite_object_get_restricted:
 * @object: #GUPnPDIDLLiteObject
 *
 * Whether the @object is restricted or not.
 *
 * Return value: #TRUE if @object is restricted.
 **/
gboolean
gupnp_didl_lite_object_get_restricted (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), FALSE);

        return xml_util_get_boolean_attribute (object->priv->xml_node,
                                               "restricted");
}

/**
 * gupnp_didl_lite_object_get_title:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the title of the @object.
 *
 * Return value: The title of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_title (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "title");
}

/**
 * gupnp_didl_lite_object_get_creator:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the creator of the @object.
 *
 * Return value: The creator of the @object, or %NULL.
 *
 * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_get_creators instead.
 **/
const char *
gupnp_didl_lite_object_get_creator (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "creator");
}

/**
 * gupnp_didl_lite_object_get_creators:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the creators of the @object.
 *
 * Return value: The list of creators belonging to @object, or %NULL.
 * #g_list_free the returned list after usage and unref each object in it.
 **/
GList *
gupnp_didl_lite_object_get_creators (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return get_contributor_list_by_name (object, "creator");
}

/**
 * gupnp_didl_lite_object_get_artist:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the artist of the @object. If role is not %NULL, it is set to the role
 * of the artist if available.
 *
 * Return value: The artist of the @object, or %NULL.
 *
 * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_get_artists instead.
 **/
const char *
gupnp_didl_lite_object_get_artist (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "artist");
}

/**
 * gupnp_didl_lite_object_get_artists:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the artists of the @object.
 *
 * Return value: The list of artists belonging to @object, or %NULL.
 * #g_list_free the returned list after usage and unref each object in it.
 **/
GList *
gupnp_didl_lite_object_get_artists (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return get_contributor_list_by_name (object, "artist");
}

/**
 * gupnp_didl_lite_object_get_author:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the author of the @object.
 *
 * Return value: The author of the @object, or %NULL.
 *
 * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_get_authors instead.
 **/
const char *
gupnp_didl_lite_object_get_author (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "author");
}

/**
 * gupnp_didl_lite_object_get_authors:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the authors of the @object.
 *
 * Return value: The list of authors belonging to @object, or %NULL.
 * #g_list_free the returned list after usage and unref each object in it.
 **/
GList *
gupnp_didl_lite_object_get_authors (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return get_contributor_list_by_name (object, "author");
}

/**
 * gupnp_didl_lite_object_get_descriptors:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the descriptors of the @object.
 *
 * Return value: The list of descriptors belonging to @object, or %NULL.
 * #g_list_free the returned list after usage and unref each object in it.
 **/
GList *
gupnp_didl_lite_object_get_descriptors (GUPnPDIDLLiteObject *object)
{
        GList *descriptors = NULL;
        GList *ret = NULL;
        GList *l;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        descriptors = gupnp_didl_lite_object_get_properties (object, "desc");

        for (l = descriptors; l; l = l->next) {
                GUPnPDIDLLiteDescriptor *descriptor;
                xmlNode *descriptor_node;

                descriptor_node = (xmlNode *) l->data;

                descriptor = gupnp_didl_lite_descriptor_new_from_xml
                                        (descriptor_node,
                                         object->priv->xml_doc);

                ret = g_list_append (ret, descriptor);
        }

        g_list_free (descriptors);

        return ret;
}

/**
 * gupnp_didl_lite_object_get_genre:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the genre of the @object.
 *
 * Return value: The genre of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_genre (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "genre");
}

/**
 * gupnp_didl_lite_object_get_write_status:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the write status of the @object.
 *
 * Return value: The write status of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_write_status (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "writeStatus");
}

/**
 * gupnp_didl_lite_object_get_album:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the album of the @object.
 *
 * Return value: The album of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_album (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "album");
}

/**
 * gupnp_didl_lite_object_get_album_art:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the URI to album art of the @object.
 *
 * Return value: The URI to album art of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_album_art (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "albumArtURI");
}

/**
 * gupnp_didl_lite_object_get_description:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the description of the @object.
 *
 * Return value: The description of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_description (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "description");
}

/**
 * gupnp_didl_lite_object_get_date:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the date of the @object.
 *
 * Return value: The date of the @object, or %NULL.
 **/
const char *
gupnp_didl_lite_object_get_date (GUPnPDIDLLiteObject *object)
{
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        return xml_util_get_child_element_content (object->priv->xml_node,
                                                   "date");
}

/**
 * gupnp_didl_lite_object_get_track_number:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the original track number of the @object.
 *
 * Return value: The original track number of the @object, or -1.
 **/
int
gupnp_didl_lite_object_get_track_number (GUPnPDIDLLiteObject *object)
{
        const char *str;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), -1);

        str = xml_util_get_child_element_content (object->priv->xml_node,
                                                  "originalTrackNumber");
        if (str == NULL)
                return -1;

        return atoi (str);
}

/**
 * gupnp_didl_lite_object_get_dlna_managed:
 * @object: #GUPnPDIDLLiteObject
 *
 * Get the 'dlna:dlnaManaged' attribute of the @object.
 *
 * Return value: The 'dlna:dlnaManaged' attribute of the @object.
 **/
GUPnPOCMFlags
gupnp_didl_lite_object_get_dlna_managed (GUPnPDIDLLiteObject *object)
{
        const char *str;
        GUPnPOCMFlags dlna_managed;

        g_return_val_if_fail (object != NULL, GUPNP_OCM_FLAGS_NONE);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object),
                              GUPNP_OCM_FLAGS_NONE);

        str = xml_util_get_attribute_content (object->priv->xml_node,
                                              "dlnaManaged");
        if (str == NULL)
                return GUPNP_OCM_FLAGS_NONE;

        sscanf (str, "%08x", &dlna_managed);

        return dlna_managed;
}

/**
 * gupnp_didl_lite_object_get_resources:
 * @object: #GUPnPDIDLLiteObject
 *
 * Use this function to retreive resources from the @object.
 *
 * Return value: (element-type GUPnPDIDLLiteResource) (transfer full): The list
 *               of resources belonging to  @object, or %NULL. #g_list_free the
 *               returned list after usage and unref each resource in it.
 **/
GList *
gupnp_didl_lite_object_get_resources (GUPnPDIDLLiteObject *object)
{
        GList *resources = NULL;
        GList *res = NULL;
        GList *ret = NULL;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        resources = gupnp_didl_lite_object_get_properties (object, "res");

        for (res = resources; res; res = res->next) {
                GUPnPDIDLLiteResource *resource;
                xmlNode *res_node;

                res_node = (xmlNode *) res->data;

                /* Create a resource struct out of DIDLLite XML */
                resource = gupnp_didl_lite_resource_new_from_xml
                                        (res_node,
                                         object->priv->xml_doc);

                ret = g_list_append (ret, resource);
        }

        g_list_free (resources);

        return ret;
}

/**
 * gupnp_didl_lite_object_get_compat_resource:
 * @object: #GUPnPDIDLLiteObject
 * @sink_protocol_info: The SinkProtocolInfo string from MediaRenderer
 * @lenient: Enable lenient mode
 *
 * Use this function to get a resource from the @object that is compatible with
 * any of the protocols specified in the @sink_protocol_info. The value of
 * @sink_protocol_info will typically be acquired from 'Sink' argument of
 * 'GetProtocolInfo' action or 'SinkProtocolInfo' state-variable of a
 * ConnectionManager service.
 *
 * If @lenient is #TRUE, the first resource in the list is returned instead of
 * %NULL if none of resources and protocols are found to be compatible.
 *
 * Return value: The resource belonging to @object that is comaptible with
 * any of the protocols specified in @sink_protocol_info, or %NULL. Unref after
 * usage.
 **/
GUPnPDIDLLiteResource *
gupnp_didl_lite_object_get_compat_resource
                                (GUPnPDIDLLiteObject *object,
                                 const char          *sink_protocol_info,
                                 gboolean             lenient)
{
        GUPnPDIDLLiteResource *resource = NULL;
        GList  *resources = NULL;
        GList  *compat_resources = NULL;
        GList  *res;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);
        g_return_val_if_fail (sink_protocol_info != NULL, NULL);

        resources = gupnp_didl_lite_object_get_resources (object);
        if (resources == NULL)
                return NULL;

        for (res = resources;
             res != NULL;
             res = res->next) {
                resource = (GUPnPDIDLLiteResource *) res->data;

                if (is_resource_compatible (resource, sink_protocol_info))
                        compat_resources = g_list_append (compat_resources,
                                                          resource);
        }

        resource = NULL;

        if (compat_resources != NULL) {
                /* Try to find non-transcoded resource */
                res = g_list_find_custom (compat_resources,
                                          NULL,
                                          (GCompareFunc)
                                          is_non_transcoded_resource);

                if (res != NULL)
                        resource = (GUPnPDIDLLiteResource *) res->data;
                else
                        /* Just use the first compatible resource */
                        resource = (GUPnPDIDLLiteResource *)
                                   compat_resources->data;

        } else if (lenient)
                /* Just use the first resource */
                resource = (GUPnPDIDLLiteResource *) resources->data;

        /* Unref all resources except for the one we just took */
        for (res = resources; res; res = res->next)
                if (res->data != resource)
                        g_object_unref (res->data);
        g_list_free (resources);
        g_list_free (compat_resources);

        return resource;
}

/**
 * gupnp_didl_lite_object_set_upnp_class:
 * @object: The #GUPnPDIDLLiteObject
 * @upnp_class: The UPnP class as string.
 *
 * Set the UPnP class of the @object to @upnp_class.
 **/
void
gupnp_didl_lite_object_set_upnp_class (GUPnPDIDLLiteObject *object,
                                       const char          *upnp_class)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->upnp_ns,
                            object->priv->xml_doc->doc,
                            "class",
                            upnp_class);

        g_object_notify (G_OBJECT (object), "upnp-class");
}

/**
 * gupnp_didl_lite_object_set_id
 * @object: #GUPnPDIDLLiteObject
 * @id: The ID
 *
 * Set the ID of the @object to @id.
 **/
void
gupnp_didl_lite_object_set_id (GUPnPDIDLLiteObject *object,
                               const char          *id)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xmlSetProp (object->priv->xml_node,
                    (unsigned char *) "id",
                    (unsigned char *) id);

        g_object_notify (G_OBJECT (object), "id");
}

/**
 * gupnp_didl_lite_object_set_parent_id:
 * @object: #GUPnPDIDLLiteObject
 * @parent_id: The parent ID
 *
 * Set the ID of the parent of the @object to @parent_id.
 **/
void
gupnp_didl_lite_object_set_parent_id (GUPnPDIDLLiteObject *object,
                                      const char          *parent_id)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xmlSetProp (object->priv->xml_node,
                    (unsigned char *) "parentID",
                    (unsigned char *) parent_id);

        g_object_notify (G_OBJECT (object), "parent-id");
}

/**
 * gupnp_didl_lite_object_set_restricted:
 * @object: #GUPnPDIDLLiteObject
 * @restricted: The restricted status
 *
 * Set the restricted status of @object to @restricted.
 **/
void
gupnp_didl_lite_object_set_restricted (GUPnPDIDLLiteObject *object,
                                       gboolean             restricted)
{
        const char *str;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        if (restricted)
                str = "1";
        else
                str = "0";
        xmlSetProp (object->priv->xml_node,
                    (unsigned char *) "restricted",
                    (unsigned char *) str);

        g_object_notify (G_OBJECT (object), "restricted");
}

/**
 * gupnp_didl_lite_object_set_title:
 * @object: #GUPnPDIDLLiteObject
 * @title: The title
 *
 * Set the title of the @object to @title.
 **/
void
gupnp_didl_lite_object_set_title (GUPnPDIDLLiteObject *object,
                                  const char          *title)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->dc_ns,
                            object->priv->xml_doc->doc,
                            "title",
                            title);

        g_object_notify (G_OBJECT (object), "title");
}

/**
 * gupnp_didl_lite_object_set_creator:
 * @object: #GUPnPDIDLLiteObject
 * @creator: The creator
 *
 * Set the creator of the @object to @creator.
 *
 * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_add_creator instead.
 **/
void
gupnp_didl_lite_object_set_creator (GUPnPDIDLLiteObject *object,
                                    const char          *creator)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->dc_ns,
                            object->priv->xml_doc->doc,
                            "creator",
                            creator);

        g_object_notify (G_OBJECT (object), "creator");
}

/**
 * gupnp_didl_lite_object_add_creator:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Add a new creator node to the @object and return the associated
 * #GUPnPDIDLLiteContributor object.
 *
 * Return value: A new #GUPnPDIDLLiteContributor object. Unref after usage.
 **/
GUPnPDIDLLiteContributor *
gupnp_didl_lite_object_add_creator (GUPnPDIDLLiteObject *object)
{
        xmlNode *res_node;

        g_return_val_if_fail (object != NULL, NULL);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        res_node = xmlNewChild (object->priv->xml_node,
                                object->priv->dc_ns,
                                (unsigned char *) "creator",
                                NULL);

        return gupnp_didl_lite_contributor_new_from_xml (res_node,
                                                         object->priv->xml_doc);
}


/**
 * gupnp_didl_lite_object_set_artist:
 * @object: The #GUPnPDIDLLiteObject
 * @artist: The Artist
 *
 * Set the Artist of the @object to @artist.
 *
 * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_add_artist instead.
 **/
void
gupnp_didl_lite_object_set_artist (GUPnPDIDLLiteObject *object,
                                   const char          *artist)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->upnp_ns,
                            object->priv->xml_doc->doc,
                            "artist",
                            artist);

        g_object_notify (G_OBJECT (object), "artist");
}

/**
 * gupnp_didl_lite_object_add_artist:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Add a new Artist node to the @object and return the associated
 * #GUPnPDIDLLiteContributor object.
 *
 * Return value: A new #GUPnPDIDLLiteContributor object. Unref after usage.
 **/
GUPnPDIDLLiteContributor *
gupnp_didl_lite_object_add_artist (GUPnPDIDLLiteObject *object)
{
        xmlNode *res_node;

        g_return_val_if_fail (object != NULL, NULL);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        res_node = xmlNewChild (object->priv->xml_node,
                                object->priv->upnp_ns,
                                (unsigned char *) "artist",
                                NULL);

        return gupnp_didl_lite_contributor_new_from_xml (res_node,
                                                         object->priv->xml_doc);
}

/**
 * gupnp_didl_lite_object_set_author:
 * @object: The #GUPnPDIDLLiteObject
 * @author: The Author
 *
 * Set the Author of the @object to @author.
 *
 * Deprecated: 0.5.3: Use #gupnp_didl_lite_object_add_author instead.
 **/
void
gupnp_didl_lite_object_set_author (GUPnPDIDLLiteObject *object,
                                   const char          *author)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->upnp_ns,
                            object->priv->xml_doc->doc,
                            "author",
                            author);

        g_object_notify (G_OBJECT (object), "author");
}

/**
 * gupnp_didl_lite_object_add_author:
 * @object: The #GUPnPDIDLLiteObject
 *
 * Add a new author node to the @object and return the associated
 * #GUPnPDIDLLiteContributor object.
 *
 * Return value: A new #GUPnPDIDLLiteContributor object. Unref after usage.
 **/
GUPnPDIDLLiteContributor *
gupnp_didl_lite_object_add_author (GUPnPDIDLLiteObject *object)
{
        xmlNode *res_node;

        g_return_val_if_fail (object != NULL, NULL);
        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        res_node = xmlNewChild (object->priv->xml_node,
                                object->priv->upnp_ns,
                                (unsigned char *) "author",
                                NULL);

        return gupnp_didl_lite_contributor_new_from_xml (res_node,
                                                         object->priv->xml_doc);
}

/**
 * gupnp_didl_lite_object_set_genre:
 * @object: The #GUPnPDIDLLiteObject
 * @genre: The Genre
 *
 * Set the genre of the @object to @genre.
 **/
void
gupnp_didl_lite_object_set_genre (GUPnPDIDLLiteObject *object,
                                  const char          *genre)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->upnp_ns,
                            object->priv->xml_doc->doc,
                            "genre",
                            genre);

        g_object_notify (G_OBJECT (object), "genre");
}

/**
 * gupnp_didl_lite_object_set_write_status:
 * @object: #GUPnPDIDLLiteObject
 * @write_status: The write status string
 *
 * Set the write status of the @object to @write_status.
 **/
void
gupnp_didl_lite_object_set_write_status (GUPnPDIDLLiteObject *object,
                                         const char          *write_status)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->dc_ns,
                            object->priv->xml_doc->doc,
                            "writeStatus",
                            write_status);

        g_object_notify (G_OBJECT (object), "write-status");
}

/**
 * gupnp_didl_lite_object_set_album:
 * @object: #GUPnPDIDLLiteObject
 * @album: The album string
 *
 * Set the album of the @object to @album.
 **/
void
gupnp_didl_lite_object_set_album (GUPnPDIDLLiteObject *object,
                                  const char          *album)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->upnp_ns,
                            object->priv->xml_doc->doc,
                            "album",
                            album);

        g_object_notify (G_OBJECT (object), "album");
}

/**
 * gupnp_didl_lite_object_set_album_art:
 * @object: #GUPnPDIDLLiteObject
 * @album_art: The URI of album art
 *
 * Set the URI to album art of the @object to @album_art.
 **/
void
gupnp_didl_lite_object_set_album_art (GUPnPDIDLLiteObject *object,
                                      const char          *album_art)
{
        xmlNode *node;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        node = xml_util_set_child (object->priv->xml_node,
                                   object->priv->upnp_ns,
                                   object->priv->xml_doc->doc,
                                   "albumArtURI",
                                   album_art);
        xmlSetNsProp (node,
                      object->priv->dlna_ns,
                      (const unsigned char *) "profileID",
                      (const unsigned char *) "JPEG_TN");

        g_object_notify (G_OBJECT (object), "album-art");
}

/**
 * gupnp_didl_lite_object_set_description:
 * @object: #GUPnPDIDLLiteObject
 * @description: The description string
 *
 * Set the description of the @object to @description.
 **/
void
gupnp_didl_lite_object_set_description (GUPnPDIDLLiteObject *object,
                                        const char          *description)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->dc_ns,
                            object->priv->xml_doc->doc,
                            "description",
                            description);

        g_object_notify (G_OBJECT (object), "description");
}

/**
 * gupnp_didl_lite_object_set_date:
 * @object: #GUPnPDIDLLiteObject
 * @date: The date string
 *
 * Set the date of the @object to @date.
 **/
void
gupnp_didl_lite_object_set_date (GUPnPDIDLLiteObject *object,
                                 const char          *date)
{
        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        xml_util_set_child (object->priv->xml_node,
                            object->priv->dc_ns,
                            object->priv->xml_doc->doc,
                            "date",
                            date);

        g_object_notify (G_OBJECT (object), "date");
}

/**
 * gupnp_didl_lite_object_set_track_number:
 * @object: #GUPnPDIDLLiteObject
 * @track_number: The original track number
 *
 * Set the original track number of the @object to @track_number.
 **/
void
gupnp_didl_lite_object_set_track_number (GUPnPDIDLLiteObject *object,
                                         int                  track_number)
{
        char *str;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        str = g_strdup_printf ("%d", track_number);
        xml_util_set_child (object->priv->xml_node,
                            object->priv->upnp_ns,
                            object->priv->xml_doc->doc,
                            "originalTrackNumber",
                            str);
        g_free (str);

        g_object_notify (G_OBJECT (object), "track-number");
}

/**
 * gupnp_didl_lite_object_set_dlna_managed:
 * @object: #GUPnPDIDLLiteObject
 * @dlna_managed: The #GUPnPOCMFlags.
 *
 * Set the 'dlna:dlnaManaged' attribute of the @object to @dlna_managed.
 **/
void
gupnp_didl_lite_object_set_dlna_managed (GUPnPDIDLLiteObject *object,
                                         GUPnPOCMFlags        dlna_managed)
{
        char *str;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object));

        str = g_strdup_printf ("%08x", dlna_managed);
        xmlSetNsProp (object->priv->xml_node,
                      object->priv->dlna_ns,
                      (const unsigned char *) "dlnaManaged",
                      (const unsigned char *) str);
        g_free (str);

        g_object_notify (G_OBJECT (object), "dlna-managed");
}

/**
 * gupnp_didl_lite_object_add_resource:
 * @object: A #GUPnPDIDLLiteObject
 *
 * Creates a new resource, attaches it to @object and returns it.
 *
 * Return value: A new #GUPnPDIDLLiteResource object. Unref after usage.
 **/
GUPnPDIDLLiteResource *
gupnp_didl_lite_object_add_resource (GUPnPDIDLLiteObject *object)
{
        xmlNode *res_node;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        res_node = xmlNewChild (object->priv->xml_node,
                                NULL,
                                (unsigned char *) "res",
                                NULL);

        return gupnp_didl_lite_resource_new_from_xml (res_node,
                                                      object->priv->xml_doc);
}

/**
 * gupnp_didl_lite_object_add_descriptor:
 * @object: A #GUPnPDIDLLiteObject
 *
 * Creates a new descriptor, attaches it to @object and returns it.
 *
 * Return value: A new #GUPnPDIDLLiteDescriptor object. Unref after usage.
 **/
GUPnPDIDLLiteDescriptor *
gupnp_didl_lite_object_add_descriptor (GUPnPDIDLLiteObject *object)
{
        xmlNode *desc_node;

        g_return_val_if_fail (GUPNP_IS_DIDL_LITE_OBJECT (object), NULL);

        desc_node = xmlNewChild (object->priv->xml_node,
                                NULL,
                                (unsigned char *) "desc",
                                NULL);

        return gupnp_didl_lite_descriptor_new_from_xml (desc_node,
                                                        object->priv->xml_doc);
}

