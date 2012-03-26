/*
 * Copyright (C) 2009 Nokia Corporation.
 * Copyright (C) 2007, 2008 OpenedHand Ltd.
 *
 * Authors: Zeeshan Ali (Khattak) <zeeshan.ali@nokia.com>
 *                                <zeeshanak@gnome.org>
 *          Jorn Baayen <jorn@openedhand.com>
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
 * SECTION:gupnp-protocol-info
 * @short_description: UPnP AV ProtocolInfo
 *
 * #GUPnPProtocolInfo provides a convenient API to deal with ProtocolInfo
 * strings used in UPnP AV specifications.
 */

#include <string.h>
#include <stdlib.h>
#include <libgupnp/gupnp.h>
#include "gupnp-protocol-info.h"
#include "gupnp-av-error.h"

G_DEFINE_TYPE (GUPnPProtocolInfo,
               gupnp_protocol_info,
               G_TYPE_OBJECT);

struct _GUPnPProtocolInfoPrivate {
        char  *protocol;
        char  *network;
        char  *mime_type;
        char  *dlna_profile;
        char **play_speeds;

        GUPnPDLNAConversion dlna_conversion;
        GUPnPDLNAOperation  dlna_operation;
        GUPnPDLNAFlags      dlna_flags;
};

enum {
        PROP_0,
        PROP_PROTOCOL,
        PROP_NETWORK,
        PROP_MIME_TYPE,
        PROP_DLNA_PROFILE,
        PROP_PLAY_SPEEDS,

        PROP_DLNA_CONVERSION,
        PROP_DLNA_OPERATION,
        PROP_DLNA_FLAGS
};

static void
parse_additional_info (const char        *additional_info,
                       GUPnPProtocolInfo *info)
{
        GUPnPProtocolInfoPrivate *priv;
        char **tokens = NULL;
        short i;

        priv = info->priv;

        if (strcmp (additional_info, "*") == 0)
                return;

        tokens = g_strsplit (additional_info, ";", -1);
        if (tokens == NULL) {
                g_warning ("Invalid additional info in DIDL-Lite info: %s",
                           additional_info);

                return;
        }

        for (i = 0; tokens[i]; i++) {
                char *p;

                p = g_strstr_len (tokens[i],
                                  strlen (tokens[i]),
                                  "DLNA.ORG_PN=");
                if (p != NULL) {
                        p += 12; /* end of "DLNA.ORG_PN=" */
                        gupnp_protocol_info_set_dlna_profile (info, p);

                        continue;
                }

                p = g_strstr_len (tokens[i],
                                  strlen (tokens[i]),
                                  "DLNA.ORG_PS=");
                if (p != NULL) {
                        char **play_speeds;

                        p += 12; /* end of "DLNA.ORG_PS=" */

                        play_speeds = g_strsplit (p, ",", -1);
                        gupnp_protocol_info_set_play_speeds
                                                (info,
                                                 (const char **) play_speeds);
                        g_strfreev (play_speeds);

                        continue;
                }

                p = g_strstr_len (tokens[i],
                                  strlen (tokens[i]),
                                  "DLNA.ORG_CI=");
                if (p != NULL) {
                        p += 12; /* end of "DLNA.ORG_CI=" */

                        gupnp_protocol_info_set_dlna_conversion (info,
                                                                 atoi (p));

                        continue;
                }

                p = g_strstr_len (tokens[i],
                                  strlen (tokens[i]),
                                  "DLNA.ORG_OP=");
                if (p != NULL) {
                        p += 12; /* end of "DLNA.ORG_OP=" */

                        gupnp_protocol_info_set_dlna_operation
                                                        (info,
                                                         strtoul (p, NULL, 16));

                        continue;
                }

                p = g_strstr_len (tokens[i],
                                  strlen (tokens[i]),
                                  "DLNA.ORG_FLAGS=");
                if (p != NULL) {
                        p += 15; /* end of "DLNA.ORG_FLAGS=" */

                        p[8] = '\0';
                        gupnp_protocol_info_set_dlna_flags
                                                        (info,
                                                         strtoul (p, NULL, 16));

                        continue;
                }
        }

        g_strfreev (tokens);
}

static gboolean
is_transport_compat (GUPnPProtocolInfo *info1,
                     GUPnPProtocolInfo *info2)
{
        const char *protocol1;
        const char *protocol2;

        protocol1 = gupnp_protocol_info_get_protocol (info1);
        protocol2 = gupnp_protocol_info_get_protocol (info2);

        if (protocol1[0] != '*' &&
            protocol2[0] != '*' &&
            g_ascii_strcasecmp (protocol1, protocol2) != 0)
                return FALSE;
        else if (g_ascii_strcasecmp ("internal", protocol1) == 0 &&
                 strcmp (gupnp_protocol_info_get_network (info1),
                         gupnp_protocol_info_get_network (info2)) != 0)
                /* Host must be the same in case of INTERNAL protocol */
                return FALSE;
        else
                return TRUE;
}

static gboolean
is_content_format_compat (GUPnPProtocolInfo *info1,
                          GUPnPProtocolInfo *info2)
{
        const char *mime_type1;
        const char *mime_type2;

        mime_type1 = gupnp_protocol_info_get_mime_type (info1);
        mime_type2 = gupnp_protocol_info_get_mime_type (info2);

        if (mime_type1 [0] != '*' &&
            mime_type2 [0] != '*' &&
            g_ascii_strcasecmp (mime_type1, mime_type2) != 0 &&
            /* Handle special case for LPCM: It is the only content type that
             * make use of mime-type parameters that we know of.
             *
             * Example: audio/L16;rate=44100;channels=2
             */
            !((g_ascii_strcasecmp (mime_type1, "audio/L16") == 0 &&
               g_ascii_strncasecmp (mime_type2, "audio/L16", 9) == 0) ||
              (g_ascii_strcasecmp (mime_type2, "audio/L16") == 0 &&
               g_ascii_strncasecmp (mime_type1, "audio/L16", 9) == 0)))
                return FALSE;
        else
                return TRUE;
}

static gboolean
is_additional_info_compat (GUPnPProtocolInfo *info1,
                           GUPnPProtocolInfo *info2)
{
        const char *profile1;
        const char *profile2;

        profile1 = gupnp_protocol_info_get_dlna_profile (info1);
        profile2 = gupnp_protocol_info_get_dlna_profile (info2);

        if (profile1 == NULL ||
            profile2 == NULL ||
            profile1 [0] == '*' ||
            profile2 [0] == '*' ||
            g_ascii_strcasecmp (profile1, profile2) == 0)
                return TRUE;
        else
                return FALSE;
}

static void
add_dlna_info (GString           *str,
               GUPnPProtocolInfo *info)
{
        const char *dlna_profile;
        const char **speeds;
        GUPnPDLNAConversion conversion;
        GUPnPDLNAOperation operation;
        GUPnPDLNAFlags flags;

        dlna_profile = gupnp_protocol_info_get_dlna_profile (info);
        if (dlna_profile == NULL) {
                g_string_append_printf (str, ":*");
                return;
        }

        g_string_append_printf (str, ":DLNA.ORG_PN=%s", dlna_profile);

        operation = gupnp_protocol_info_get_dlna_operation (info);
        if (operation != GUPNP_DLNA_OPERATION_NONE &&
            /* the OP parameter is only allowed for the "http-get"
             * and "rtsp-rtp-udp" protocols
             */
            (strcmp (gupnp_protocol_info_get_protocol (info),
                     "http-get") == 0 ||
             strcmp (gupnp_protocol_info_get_protocol (info),
                     "rtsp-rtp-udp") == 0))
                g_string_append_printf (str, ";DLNA.ORG_OP=%.2x", operation);

        /* Specify PS parameter if list of play speeds is provided */
        speeds = gupnp_protocol_info_get_play_speeds (info);
        if (speeds != NULL) {
                int i;

                g_string_append_printf (str, ";DLNA.ORG_PS=");

                for (i = 0; speeds[i]; i++) {
                        g_string_append (str, speeds[i]);

                        if (speeds[i + 1])
                                g_string_append_c (str, ',');
                }
        }

        conversion = gupnp_protocol_info_get_dlna_conversion (info);
        /* omit the CI parameter for non-converted content */
        if (conversion != GUPNP_DLNA_CONVERSION_NONE)
                g_string_append_printf (str, ";DLNA.ORG_CI=%d", conversion);

        flags = gupnp_protocol_info_get_dlna_flags (info);
        /* Omit the FLAGS parameter if no flags set */
        if (flags != GUPNP_DLNA_FLAGS_NONE) {
                g_string_append_printf (str, ";DLNA.ORG_FLAGS=%.8x", flags);
                /*  append 24 reserved hex-digits */
                g_string_append_printf (str,
                                        "0000" "0000" "0000"
                                        "0000" "0000" "0000");
        }
}

static void
gupnp_protocol_info_init (GUPnPProtocolInfo *info)
{
        info->priv = G_TYPE_INSTANCE_GET_PRIVATE
                                        (info,
                                         GUPNP_TYPE_PROTOCOL_INFO,
                                         GUPnPProtocolInfoPrivate);

        info->priv->dlna_conversion = GUPNP_DLNA_CONVERSION_NONE;
        info->priv->dlna_operation  = GUPNP_DLNA_OPERATION_NONE;
        info->priv->dlna_flags      = GUPNP_DLNA_FLAGS_NONE;
}

static void
gupnp_protocol_info_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
        GUPnPProtocolInfo *info;

        info = GUPNP_PROTOCOL_INFO (object);

        switch (property_id) {
        case PROP_PROTOCOL:
                gupnp_protocol_info_set_protocol (info,
                                                  g_value_get_string (value));
                break;
        case PROP_NETWORK:
                gupnp_protocol_info_set_network (info,
                                                 g_value_get_string (value));
                break;
        case PROP_MIME_TYPE:
                gupnp_protocol_info_set_mime_type (info,
                                                   g_value_get_string (value));
                break;
        case PROP_DLNA_PROFILE:
                gupnp_protocol_info_set_dlna_profile
                                        (info,
                                         g_value_get_string (value));
                break;
        case PROP_PLAY_SPEEDS:
                gupnp_protocol_info_set_play_speeds (info,
                                                     g_value_get_boxed (value));
                break;
        case PROP_DLNA_CONVERSION:
                gupnp_protocol_info_set_dlna_conversion
                                        (info,
                                         g_value_get_flags (value));
                break;
        case PROP_DLNA_OPERATION:
                gupnp_protocol_info_set_dlna_operation
                                        (info,
                                         g_value_get_flags (value));
                break;
        case PROP_DLNA_FLAGS:
                gupnp_protocol_info_set_dlna_flags
                                        (info,
                                         g_value_get_flags (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_protocol_info_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
        GUPnPProtocolInfo *info;

        info = GUPNP_PROTOCOL_INFO (object);

        switch (property_id) {
        case PROP_PROTOCOL:
                g_value_set_string (value,
                                    gupnp_protocol_info_get_protocol (info));
                break;
        case PROP_NETWORK:
                g_value_set_string (value,
                                    gupnp_protocol_info_get_network (info));
                break;
        case PROP_MIME_TYPE:
                g_value_set_string (value,
                                    gupnp_protocol_info_get_mime_type (info));
                break;
        case PROP_DLNA_PROFILE:
                g_value_set_string
                              (value,
                               gupnp_protocol_info_get_dlna_profile (info));
                break;
        case PROP_PLAY_SPEEDS:
                g_value_set_boxed (value,
                                   gupnp_protocol_info_get_play_speeds (info));
                break;
        case PROP_DLNA_CONVERSION:
                g_value_set_flags
                                (value,
                                 gupnp_protocol_info_get_dlna_conversion
                                                                (info));
                break;
        case PROP_DLNA_OPERATION:
                g_value_set_flags
                                (value,
                                 gupnp_protocol_info_get_dlna_operation (info));
                break;
        case PROP_DLNA_FLAGS:
                g_value_set_flags (value,
                                   gupnp_protocol_info_get_dlna_flags (info));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
gupnp_protocol_info_finalize (GObject *object)
{
        GObjectClass                 *object_class;
        GUPnPProtocolInfoPrivate *priv;

        priv = GUPNP_PROTOCOL_INFO (object)->priv;

        if (priv->protocol)
                g_free (priv->protocol);
        if (priv->network)
                g_free (priv->network);
        if (priv->mime_type)
                g_free (priv->mime_type);
        if (priv->dlna_profile)
                g_free (priv->dlna_profile);
        if (priv->play_speeds)
                g_strfreev (priv->play_speeds);

        object_class = G_OBJECT_CLASS (gupnp_protocol_info_parent_class);
        object_class->finalize (object);
}

static void
gupnp_protocol_info_class_init (GUPnPProtocolInfoClass *klass)
{
        GObjectClass *object_class;

        object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = gupnp_protocol_info_set_property;
        object_class->get_property = gupnp_protocol_info_get_property;
        object_class->finalize = gupnp_protocol_info_finalize;

        g_type_class_add_private (klass, sizeof (GUPnPProtocolInfoPrivate));

        /**
         * GUPnPProtocolInfo:protocol
         *
         * The protocol of this info.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_PROTOCOL,
                 g_param_spec_string ("protocol",
                                      "Protocol",
                                      "The protocol of this info.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPProtocolInfo:network
         *
         * The network this info is associated with.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_NETWORK,
                 g_param_spec_string ("network",
                                      "Network",
                                      "The network this info is associated"
                                      " with.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPProtocolInfo:mime-type
         *
         * The MIME-type of this info.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_MIME_TYPE,
                 g_param_spec_string ("mime-type",
                                      "MIMEType",
                                      "The MIME-type of this info.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPProtocolInfo:dlna-profile
         *
         * The DLNA profile of this info.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DLNA_PROFILE,
                 g_param_spec_string ("dlna-profile",
                                      "DLNAProfile",
                                      "The DLNA profile of this info.",
                                      NULL,
                                      G_PARAM_READWRITE |
                                      G_PARAM_STATIC_NAME |
                                      G_PARAM_STATIC_NICK |
                                      G_PARAM_STATIC_BLURB));

        /**
         * GUPnPProtocolInfo:play-speeds
         *
         * The allowed play speeds on this info in the form of array of
         * strings.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_PLAY_SPEEDS,
                 g_param_spec_boxed ("play-speeds",
                                     "PlaySpeeds",
                                     "The allowed play speeds on this"
                                     " info in the form of array of"
                                     " strings.",
                                     G_TYPE_STRV,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_NAME |
                                     G_PARAM_STATIC_NICK |
                                     G_PARAM_STATIC_BLURB));

        /**
         * GUPnPProtocolInfo:dlna-conversion
         *
         * The DLNA conversion flags.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DLNA_CONVERSION,
                 g_param_spec_flags ("dlna-conversion",
                                     "DLNAConversion",
                                     "The DLNA conversion flags.",
                                     GUPNP_TYPE_DLNA_CONVERSION,
                                     GUPNP_DLNA_CONVERSION_NONE,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_NAME |
                                     G_PARAM_STATIC_NICK |
                                     G_PARAM_STATIC_BLURB));

        /**
         * GUPnPProtocolInfo:dlna-operation
         *
         * The DLNA operation flags.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DLNA_OPERATION,
                 g_param_spec_flags ("dlna-operation",
                                     "DLNAOperation",
                                     "The DLNA operation flags.",
                                     GUPNP_TYPE_DLNA_OPERATION,
                                     GUPNP_DLNA_OPERATION_NONE,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_NAME |
                                     G_PARAM_STATIC_NICK |
                                     G_PARAM_STATIC_BLURB));

        /**
         * GUPnPProtocolInfo:dlna-flags
         *
         * Various generic DLNA flags.
         **/
        g_object_class_install_property
                (object_class,
                 PROP_DLNA_FLAGS,
                 g_param_spec_flags ("dlna-flags",
                                     "DLNAFlags",
                                     "Various generic DLNA flags.",
                                     GUPNP_TYPE_DLNA_FLAGS,
                                     GUPNP_DLNA_FLAGS_NONE,
                                     G_PARAM_READWRITE |
                                     G_PARAM_STATIC_NAME |
                                     G_PARAM_STATIC_NICK |
                                     G_PARAM_STATIC_BLURB));
}

/**
 * gupnp_protocol_info_new
 *
 * Return value: A new #GUPnPProtocolInfo object. Unref after usage.
 **/
GUPnPProtocolInfo *
gupnp_protocol_info_new ()
{
        return g_object_new (GUPNP_TYPE_PROTOCOL_INFO,
                             NULL);
}

/**
 * gupnp_protocol_info_new_from_string
 * @protocol_info: The protocol info string
 * @error: The location where to store any error, or NULL
 *
 * Parses the @protocol_info string and creates a new #GUPnPProtocolInfo object
 * as a result.
 *
 * Return value: A new #GUPnPProtocolInfo object. Unref after usage.
 **/
GUPnPProtocolInfo *
gupnp_protocol_info_new_from_string (const char *protocol_info,
                                     GError    **error)
{
        GUPnPProtocolInfo *info;
        char **tokens;

        g_return_val_if_fail (protocol_info != NULL, NULL);

        tokens = g_strsplit (protocol_info, ":", 4);
        if (tokens[0] == NULL ||
            tokens[1] == NULL ||
            tokens[2] == NULL ||
            tokens[3] == NULL) {
                g_set_error (error,
                             GUPNP_PROTOCOL_ERROR,
                             GUPNP_PROTOCOL_ERROR_INVALID_SYNTAX,
                             "Failed to parse protocolInfo string: \n%s",
                             protocol_info);

                g_strfreev (tokens);

                return NULL;
        }

        info = gupnp_protocol_info_new ();

        gupnp_protocol_info_set_protocol (info, tokens[0]);
        gupnp_protocol_info_set_network (info, tokens[1]);
        gupnp_protocol_info_set_mime_type (info, tokens[2]);

        parse_additional_info (tokens[3], info);

        g_strfreev (tokens);

        return info;
}

/**
 * gupnp_protocol_info_to_string
 * @info: The #GUPnPProtocolInfo
 *
 * Provides the string representation of @info.
 *
 * Return value: String representation of @info. #g_free after usage.
 **/
char *
gupnp_protocol_info_to_string (GUPnPProtocolInfo *info)
{
        GString *str;
        const char *protocol;
        const char *mime_type;
        const char *network;

        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info), NULL);

        protocol = gupnp_protocol_info_get_protocol (info);
        mime_type = gupnp_protocol_info_get_mime_type (info);
        network = gupnp_protocol_info_get_network (info);

        g_return_val_if_fail (protocol != NULL, NULL);
        g_return_val_if_fail (mime_type != NULL, NULL);

        str = g_string_new ("");

        g_string_append (str, protocol);
        g_string_append_c (str, ':');
        if (network != NULL)
                g_string_append (str, network);
        else
                g_string_append_c (str, '*');
        g_string_append_c (str, ':');
        g_string_append (str, mime_type);

        add_dlna_info (str, info);

        return g_string_free (str, FALSE);
}

/**
 * gupnp_protocol_info_get_protocol
 * @info: A #GUPnPProtocolInfo
 *
 * Get the protocol of this info.
 *
 * Return value: The protocol of this info or %NULL. This string should not
 * be freed.
 **/
const char *
gupnp_protocol_info_get_protocol (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info), NULL);

        return info->priv->protocol;
}

/**
 * gupnp_protocol_info_get_network
 * @info: A #GUPnPProtocolInfo
 *
 * Get the network this info is associated with.
 *
 * Return value: The network string or %NULL. This string should not be freed.
 **/
const char *
gupnp_protocol_info_get_network (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info), NULL);

        return info->priv->network;
}

/**
 * gupnp_protocol_info_get_mime_type
 * @info: A #GUPnPProtocolInfo
 *
 * Get the MIME-type of this info.
 *
 * Return value: The MIME-type of this info or %NULL. This string should not
 * be freed.
 **/
const char *
gupnp_protocol_info_get_mime_type (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info), NULL);

        return info->priv->mime_type;
}

/**
 * gupnp_protocol_info_get_dlna_profile
 * @info: A #GUPnPProtocolInfo
 *
 * Get the DLNA profile of this info.
 *
 * Return value: The DLNA profile of this info or %NULL. This string should
 * not be freed.
 **/
const char *
gupnp_protocol_info_get_dlna_profile (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info), NULL);

        return info->priv->dlna_profile;
}

/**
 * gupnp_protocol_info_get_play_speeds
 * @info: A #GUPnPProtocolInfo
 *
 * Get the allowed play speeds on this info in the form of array of strings.
 *
 * Return value: The allowed play speeds as array of strings or %NULL. This
 * return array and it's content must not be modified or freed.
 **/
const char **
gupnp_protocol_info_get_play_speeds (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info), NULL);

        return (const char **) info->priv->play_speeds;
}

/**
 * gupnp_protocol_info_get_dlna_conversion
 * @info: A #GUPnPProtocolInfo
 *
 * Get the DLNA conversion flags.
 *
 * Return value: The DLNA conversion flags.
 **/
GUPnPDLNAConversion
gupnp_protocol_info_get_dlna_conversion (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info),
                              GUPNP_DLNA_CONVERSION_NONE);

        return info->priv->dlna_conversion;
}

/**
 * gupnp_protocol_info_get_dlna_operation
 * @info: A #GUPnPProtocolInfo
 *
 * Get the DLNA operation flags.
 *
 * Return value: The DLNA operation flags.
 **/
GUPnPDLNAOperation
gupnp_protocol_info_get_dlna_operation (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info),
                              GUPNP_DLNA_OPERATION_NONE);

        return info->priv->dlna_operation;
}

/**
 * gupnp_protocol_info_get_dlna_flags
 * @info: A #GUPnPProtocolInfo
 *
 * Get the gereric DLNA flags.
 *
 * Return value: The generic DLNA flags.
 **/
GUPnPDLNAFlags
gupnp_protocol_info_get_dlna_flags (GUPnPProtocolInfo *info)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info),
                              GUPNP_DLNA_FLAGS_NONE);

        return info->priv->dlna_flags;
}

/**
 * gupnp_protocol_info_set_protocol
 * @info: A #GUPnPProtocolInfo
 * @protocol: The protocol string
 *
 * Set the protocol of this info.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_protocol (GUPnPProtocolInfo *info,
                                  const char        *protocol)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        if (info->priv->protocol)
                g_free (info->priv->protocol);
        info->priv->protocol = g_strdup (protocol);

        g_object_notify (G_OBJECT (info), "protocol");
}

/**
 * gupnp_protocol_info_set_network
 * @info: A #GUPnPProtocolInfo
 * @network: The network string
 *
 * Set the network this info is associated with.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_network (GUPnPProtocolInfo *info,
                                 const char        *network)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        if (info->priv->network)
                g_free (info->priv->network);
        info->priv->network = g_strdup (network);

        g_object_notify (G_OBJECT (info), "network");
}

/**
 * gupnp_protocol_info_set_mime_type
 * @info: A #GUPnPProtocolInfo
 * @mime_type: The MIME-type string
 *
 * Set the MIME-type of this info.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_mime_type (GUPnPProtocolInfo *info,
                                   const char        *mime_type)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        if (info->priv->mime_type)
                g_free (info->priv->mime_type);
        info->priv->mime_type = g_strdup (mime_type);

        g_object_notify (G_OBJECT (info), "mime-type");
}

/**
 * gupnp_protocol_info_set_dlna_profile
 * @info: A #GUPnPProtocolInfo
 * @profile: The DLNA profile string
 *
 * Set the DLNA profile of this info.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_dlna_profile (GUPnPProtocolInfo *info,
                                      const char        *profile)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        if (info->priv->dlna_profile)
                g_free (info->priv->dlna_profile);
        info->priv->dlna_profile = g_strdup (profile);

        g_object_notify (G_OBJECT (info), "dlna-profile");
}

/**
 * gupnp_protocol_info_set_play_speeds
 * @info: A #GUPnPProtocolInfo
 * @speeds: The allowed play speeds
 *
 * Set the allowed play speeds on this info in the form of array of strings.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_play_speeds (GUPnPProtocolInfo *info,
                                     const char       **speeds)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        if (info->priv->play_speeds)
                g_strfreev (info->priv->play_speeds);
        info->priv->play_speeds = (char **) g_boxed_copy (G_TYPE_STRV, speeds);

        g_object_notify (G_OBJECT (info), "play-speeds");
}

/**
 * gupnp_protocol_info_set_dlna_conversion
 * @info: A #GUPnPProtocolInfo
 * @conversion: The bitwise OR of one or more DLNA conversion flags
 *
 * Set the DLNA conversion flags.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_dlna_conversion (GUPnPProtocolInfo  *info,
                                         GUPnPDLNAConversion conversion)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        info->priv->dlna_conversion = conversion;

        g_object_notify (G_OBJECT (info), "dlna-conversion");
}

/**
 * gupnp_protocol_info_set_dlna_operation
 * @info: A #GUPnPProtocolInfo
 * @operation: The bitwise OR of one or more DLNA operation flags
 *
 * Set the DLNA operation flags.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_dlna_operation (GUPnPProtocolInfo *info,
                                        GUPnPDLNAOperation operation)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        info->priv->dlna_operation = operation;

        g_object_notify (G_OBJECT (info), "dlna-operation");
}

/**
 * gupnp_protocol_info_set_dlna_flags
 * @info: A #GUPnPProtocolInfo
 * @flags: The bitwise OR of one or more generic DLNA flags
 *
 * Set the gereric DLNA flags.
 *
 * Return value: None.
 **/
void
gupnp_protocol_info_set_dlna_flags (GUPnPProtocolInfo  *info,
                                    GUPnPDLNAFlags      flags)
{
        g_return_if_fail (GUPNP_IS_PROTOCOL_INFO (info));

        info->priv->dlna_flags = flags;

        g_object_notify (G_OBJECT (info), "dlna-flags");
}

/**
 * gupnp_protocol_info_is_compatible
 * @info1: The first #GUPnPProtocolInfo
 * @info2: The second #GUPnPProtocolInfo
 *
 * Checks if the given protocolInfo string is compatible with @info.
 *
 * Return value: #TRUE if @protocol_info is compatible with @info, otherwise
 * #FALSE.
 **/
gboolean
gupnp_protocol_info_is_compatible (GUPnPProtocolInfo *info1,
                                   GUPnPProtocolInfo *info2)
{
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info1), FALSE);
        g_return_val_if_fail (GUPNP_IS_PROTOCOL_INFO (info2), FALSE);

        return is_transport_compat (info1, info2) &&
               is_content_format_compat (info1, info2) &&
               is_additional_info_compat (info1, info2);
}

