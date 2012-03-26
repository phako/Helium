/*
 * Copyright (C) 2007 Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *
 * Authors: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
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
 * SECTION:gupnp-didl-lite-parser
 * @short_description: A/V DIDL-Lite XML parser
 *
 * #GUPnPDIDLLiteParser parses DIDL-Lite XML strings.
 *
 */

#include <string.h>
#include "gupnp-av.h"
#include "gupnp-didl-lite-object-private.h"
#include "xml-util.h"

G_DEFINE_TYPE (GUPnPDIDLLiteParser,
               gupnp_didl_lite_parser,
               G_TYPE_OBJECT);

enum {
        OBJECT_AVAILABLE,
        ITEM_AVAILABLE,
        CONTAINER_AVAILABLE,
        SIGNAL_LAST
};

static guint signals[SIGNAL_LAST];

static void
gupnp_didl_lite_parser_init (GUPnPDIDLLiteParser *parser)
{
}

static void
gupnp_didl_lite_parser_dispose (GObject *object)
{
        GObjectClass   *gobject_class;
        GUPnPDIDLLiteParser *parser;

        parser = GUPNP_DIDL_LITE_PARSER (object);

        gobject_class = G_OBJECT_CLASS (gupnp_didl_lite_parser_parent_class);
        gobject_class->dispose (object);
}

static void
gupnp_didl_lite_parser_class_init (GUPnPDIDLLiteParserClass *klass)
{
        GObjectClass *object_class;

        object_class = G_OBJECT_CLASS (klass);

        object_class->dispose = gupnp_didl_lite_parser_dispose;

        /**
         * GUPnPDIDLLiteParser::object-available
         * @parser: The #GUPnPDIDLLiteParser that received the signal
         * @object: The now available #GUPnPDIDLLiteObject
         *
         * The ::object-available signal is emitted each time an object is
         * found in the DIDL-Lite XML being parsed.
         **/
        signals[OBJECT_AVAILABLE] =
                g_signal_new ("object-available",
                              GUPNP_TYPE_DIDL_LITE_PARSER,
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (GUPnPDIDLLiteParserClass,
                                               object_available),
                              NULL,
                              NULL,
                              g_cclosure_marshal_VOID__OBJECT,
                              G_TYPE_NONE,
                              1,
                              GUPNP_TYPE_DIDL_LITE_OBJECT);

        /**
         * GUPnPDIDLLiteParser::item-available
         * @parser: The #GUPnPDIDLLiteParser that received the signal
         * @item: The now available #GUPnPDIDLLiteItem
         *
         * The ::item-available signal is emitted each time an item is found in
         * the DIDL-Lite XML being parsed.
         **/
        signals[ITEM_AVAILABLE] =
                g_signal_new ("item-available",
                              GUPNP_TYPE_DIDL_LITE_PARSER,
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (GUPnPDIDLLiteParserClass,
                                               item_available),
                              NULL,
                              NULL,
                              g_cclosure_marshal_VOID__OBJECT,
                              G_TYPE_NONE,
                              1,
                              GUPNP_TYPE_DIDL_LITE_ITEM);

        /**
         * GUPnPDIDLLiteParser::container-available
         * @parser: The #GUPnPDIDLLiteParser that received the signal
         * @container: The now available #GUPnPDIDLLiteContainer
         *
         * The ::container-available signal is emitted each time a container is
         * found in the DIDL-Lite XML being parsed.
         **/
        signals[CONTAINER_AVAILABLE] =
                g_signal_new ("container-available",
                              GUPNP_TYPE_DIDL_LITE_PARSER,
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (GUPnPDIDLLiteParserClass,
                                               container_available),
                              NULL,
                              NULL,
                              g_cclosure_marshal_VOID__OBJECT,
                              G_TYPE_NONE,
                              1,
                              GUPNP_TYPE_DIDL_LITE_CONTAINER);
}

/**
 * gupnp_didl_lite_parser_new
 *
 * Return value: A new #GUPnPDIDLLiteParser object.
 **/
GUPnPDIDLLiteParser *
gupnp_didl_lite_parser_new (void)
{
        return g_object_new (GUPNP_TYPE_DIDL_LITE_PARSER, NULL);
}

/**
 * gupnp_didl_lite_parser_parse_didl
 * @parser: A #GUPnPDIDLLiteParser
 * @didl: The DIDL-Lite XML string to be parsed
 * @error: The location where to store any error, or NULL
 *
 * Parses DIDL-Lite XML string @didl, emitting the ::object-available,
 * ::item-available and ::container-available signals appropriately during the
 * process.
 *
 * Return value: TRUE on success.
 **/
gboolean
gupnp_didl_lite_parser_parse_didl (GUPnPDIDLLiteParser *parser,
                                   const char          *didl,
                                   GError             **error)
{
        xmlDoc       *doc;
        xmlNode      *element;
        xmlNs       **ns_list;
        xmlNs        *upnp_ns = NULL;
        xmlNs        *dc_ns   = NULL;
        xmlNs        *dlna_ns   = NULL;
        GUPnPXMLDoc  *xml_doc;

        doc = xmlRecoverMemory (didl, strlen (didl));
	if (doc == NULL) {
                g_set_error (error,
                             GUPNP_XML_ERROR,
                             GUPNP_XML_ERROR_PARSE,
                             "Could not parse DIDL-Lite XML:\n%s", didl);

                return FALSE;
        }

        /* Get a pointer to root element */
        element = xml_util_get_element ((xmlNode *) doc,
                                        "DIDL-Lite",
                                        NULL);
        if (element == NULL) {
                g_set_error (error,
                             GUPNP_XML_ERROR,
                             GUPNP_XML_ERROR_NO_NODE,
                             "No 'DIDL-Lite' node in the DIDL-Lite XML:\n%s",
                             didl);
                xmlFreeDoc (doc);

                return FALSE;
        }

        if (element->children == NULL) {
                g_set_error (error,
                             GUPNP_XML_ERROR,
                             GUPNP_XML_ERROR_EMPTY_NODE,
                             "Empty 'DIDL-Lite' node in the DIDL-Lite XML:\n%s",
                             didl);
                xmlFreeDoc (doc);

                return FALSE;
        }

        /* Lookup UPnP and DC namespaces */
        ns_list = xmlGetNsList (doc,
                                xmlDocGetRootElement (doc));

        if (ns_list) {
                short i;

                for (i = 0; ns_list[i] != NULL; i++) {
                        const char *prefix = (const char *) ns_list[i]->prefix;

                        if (prefix == NULL)
                                continue;

                        if (! upnp_ns &&
                            g_ascii_strcasecmp (prefix, "upnp") == 0)
                                upnp_ns = ns_list[i];
                        else if (! dc_ns &&
                                 g_ascii_strcasecmp (prefix, "dc") == 0)
                                dc_ns = ns_list[i];
                        else if (! dlna_ns &&
                                 g_ascii_strcasecmp (prefix, "dlna") == 0)
                                dlna_ns = ns_list[i];
                }

                xmlFree (ns_list);
        }

        /* Create UPnP and DC namespaces if they don't exist */
        if (! upnp_ns)
                upnp_ns = xmlNewNs (xmlDocGetRootElement (doc),
                                    (unsigned char *)
                                    "urn:schemas-upnp-org:metadata-1-0/upnp/",
                                    (unsigned char *)
                                    GUPNP_DIDL_LITE_WRITER_NAMESPACE_UPNP);
        if (! dc_ns)
                dc_ns = xmlNewNs (xmlDocGetRootElement (doc),
                                  (unsigned char *)
                                  "http://purl.org/dc/elements/1.1/",
                                  (unsigned char *)
                                  GUPNP_DIDL_LITE_WRITER_NAMESPACE_DC);
        if (! dlna_ns)
                dlna_ns = xmlNewNs (xmlDocGetRootElement (doc),
                                    (unsigned char *)
                                    "urn:schemas-dlna-org:metadata-2-0/",
                                    (unsigned char *)
                                    GUPNP_DIDL_LITE_WRITER_NAMESPACE_DLNA);

        xml_doc = gupnp_xml_doc_new (doc);

        for (element = element->children; element; element = element->next) {
                GUPnPDIDLLiteObject *object;

                object = gupnp_didl_lite_object_new_from_xml (element, xml_doc,
                                                              upnp_ns, dc_ns,
                                                              dlna_ns);

                if (object == NULL)
                        continue;

                if (GUPNP_IS_DIDL_LITE_CONTAINER (object))
                        g_signal_emit (parser,
                                        signals[CONTAINER_AVAILABLE],
                                        0,
                                        object);
                else if (GUPNP_IS_DIDL_LITE_ITEM (object))
                        g_signal_emit (parser,
                                        signals[ITEM_AVAILABLE],
                                        0,
                                        object);

                g_signal_emit (parser,
                                signals[OBJECT_AVAILABLE],
                                0,
                                object);

                g_object_unref (object);
        }

        g_object_unref (xml_doc);

        return TRUE;
}

