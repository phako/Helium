/*
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Authors: Zeeshan Ali <zeeshanak@gnome.org>
 *                      <zeeshan.ali@nokia.com>
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

#include "gupnp-dlna.h"

GType
gupnp_dlna_conversion_get_type (void)
{
        static GType type = 0;

        if (type == 0) {
                static const GFlagsValue values[] = {
                        { GUPNP_DLNA_CONVERSION_NONE,
                          "GUPNP_DLNA_CONVERSION_NONE",
                          "none" },
                        { GUPNP_DLNA_CONVERSION_TRANSCODED,
                          "GUPNP_DLNA_CONVERSION_TRANSCODED",
                          "transcoded" },
                        { 0, NULL, NULL }
                };

                type = g_flags_register_static
                                (g_intern_static_string ("GUPnPDLNAConversion"),
                                 values);
        }

        return type;
}

GType
gupnp_dlna_operation_get_type (void)
{
        static GType type = 0;

        if (type == 0) {
                static const GFlagsValue values[] = {
                        { GUPNP_DLNA_OPERATION_NONE,
                          "GUPNP_DLNA_OPERATION_NONE",
                          "none" },
                        { GUPNP_DLNA_OPERATION_RANGE,
                          "GUPNP_DLNA_OPERATION_RANGE",
                          "range" },
                        { GUPNP_DLNA_OPERATION_TIMESEEK,
                          "GUPNP_DLNA_OPERATION_TIMESEEK",
                          "timeseek" },
                        { 0, NULL, NULL }
                };

                type = g_flags_register_static
                                (g_intern_static_string ("GUPnPDLNAOperation"),
                                 values);
        }

        return type;
}

GType
gupnp_dlna_flags_get_type (void)
{
        static GType type = 0;

        if (type == 0) {
                static const GFlagsValue values[] = {
                        { GUPNP_DLNA_FLAGS_NONE,
                          "GUPNP_DLNA_FLAGS_NONE",
                          "none" },
                        { GUPNP_DLNA_FLAGS_SENDER_PACED,
                          "GUPNP_DLNA_FLAGS_SENDER_PACED",
                          "sender-paced" },
                        { GUPNP_DLNA_FLAGS_TIME_BASED_SEEK,
                          "GUPNP_DLNA_FLAGS_TIME_BASED_SEEK",
                          "time-based-seek" },
                        { GUPNP_DLNA_FLAGS_BYTE_BASED_SEEK,
                          "GUPNP_DLNA_FLAGS_BYTE_BASED_SEEK",
                          "byte-based-seek" },
                        { GUPNP_DLNA_FLAGS_PLAY_CONTAINER,
                          "GUPNP_DLNA_FLAGS_PLAY_CONTAINER",
                          "play-container" },
                        { GUPNP_DLNA_FLAGS_S0_INCREASE,
                          "GUPNP_DLNA_FLAGS_S0_INCREASE",
                          "s0-increase" },
                        { GUPNP_DLNA_FLAGS_SN_INCREASE,
                          "GUPNP_DLNA_FLAGS_SN_INCREASE",
                          "sn-increase" },
                        { GUPNP_DLNA_FLAGS_RTSP_PAUSE,
                          "GUPNP_DLNA_FLAGS_RTSP_PAUSE",
                          "rtsp-pause" },
                        { GUPNP_DLNA_FLAGS_STREAMING_TRANSFER_MODE,
                          "GUPNP_DLNA_FLAGS_STREAMING_TRANSFER_MODE",
                          "streaming-transfer-mode" },
                        { GUPNP_DLNA_FLAGS_INTERACTIVE_TRANSFER_MODE,
                          "GUPNP_DLNA_FLAGS_INTERACTIVE_TRANSFER_MODE",
                          "interactive-transfer-mode" },
                        { GUPNP_DLNA_FLAGS_BACKGROUND_TRANSFER_MODE,
                          "GUPNP_DLNA_FLAGS_BACKGROUND_TRANSFER_MODE",
                          "background-transfer-mode" },
                        { GUPNP_DLNA_FLAGS_CONNECTION_STALL,
                          "GUPNP_DLNA_FLAGS_CONNECTION_STALL",
                          "connection-stall" },
                        { GUPNP_DLNA_FLAGS_DLNA_V15,
                          "GUPNP_DLNA_FLAGS_DLNA_V15",
                          "dlna-v15" },
                        { 0, NULL, NULL }
                };

                type = g_flags_register_static
                                (g_intern_static_string ("GUPnPDLNAFlags"),
                                 values);
        }

        return type;
}

GType
gupnp_ocm_flags_get_type (void)
{
        static GType type = 0;

        if (type == 0) {
                static const GFlagsValue values[] = {
                        { GUPNP_OCM_FLAGS_NONE,
                          "GUPNP_OCM_FLAGS_NONE",
                          "none" },
                        { GUPNP_OCM_FLAGS_UPLOAD,
                          "GUPNP_OCM_FLAGS_UPLOAD",
                          "upload" },
                        { GUPNP_OCM_FLAGS_CREATE_CONTAINER,
                          "GUPNP_OCM_FLAGS_CREATE_CONTAINER",
                          "create-container" },
                        { GUPNP_OCM_FLAGS_DESTROYABLE,
                          "GUPNP_OCM_FLAGS_DESTROYABLE",
                          "destroyable" },
                        { GUPNP_OCM_FLAGS_UPLOAD_DESTROYABLE,
                          "GUPNP_OCM_FLAGS_UPLOAD_DESTROYABLE",
                          "upload-destroyable" },
                        { GUPNP_OCM_FLAGS_CHANGE_METADATA,
                          "GUPNP_OCM_FLAGS_CHANGE_METADATA",
                          "change-metadata" },
                        { 0, NULL, NULL }
                };

                type = g_flags_register_static
                                (g_intern_static_string ("GUPnPOCMFlags"),
                                 values);
        }

        return type;
}
