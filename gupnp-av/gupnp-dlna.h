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

#ifndef __GUPNP_DLNA_H__
#define __GUPNP_DLNA_H__

#include <glib-object.h>

G_BEGIN_DECLS

GType
gupnp_dlna_conversion_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DLNA_CONVERSION (gupnp_dlna_conversion_get_type ())

/**
 * GUPnPDLNAConversion:
 * @GUPNP_DLNA_CONVERSION_NONE: Content is in original source format
 * @GUPNP_DLNA_CONVERSION_TRANSCODED: Content is transcoded
 *
 * The DLNA conversion flags for a resource.
 *
 **/
typedef enum {
        GUPNP_DLNA_CONVERSION_NONE       = 0,
        GUPNP_DLNA_CONVERSION_TRANSCODED = 1
} GUPnPDLNAConversion;

GType
gupnp_dlna_operation_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DLNA_OPERATION (gupnp_dlna_operation_get_type ())

/**
 * GUPnPDLNAOperation:
 * @GUPNP_DLNA_OPERATION_NONE: Resource does not support seeking of any type
 * @GUPNP_DLNA_OPERATION_RANGE: Resource supports byte-seek
 * @GUPNP_DLNA_OPERATION_TIMESEEK: Resource supports time-seek
 *
 * The seek operations supported by a resource.
 *
 **/
typedef enum {
        GUPNP_DLNA_OPERATION_NONE     = 0x00,
        GUPNP_DLNA_OPERATION_RANGE    = 0x01,
        GUPNP_DLNA_OPERATION_TIMESEEK = 0x10
} GUPnPDLNAOperation;

GType
gupnp_dlna_flags_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_DLNA_FLAGS (gupnp_dlna_flags_get_type ())

/**
 * GUPnPDLNAFlags:
 * @GUPNP_DLNA_FLAGS_NONE: No flags
 * @GUPNP_DLNA_FLAGS_SENDER_PACED: Content source is the clock source during
 *                                 transport
 * @GUPNP_DLNA_FLAGS_TIME_BASED_SEEK: Limited Operation: time-seek supported
 * @GUPNP_DLNA_FLAGS_BYTE_BASED_SEEK: Limited Operation: byte-seek supported
 * @GUPNP_DLNA_FLAGS_PLAY_CONTAINER: Resource supports 'Container Playback'
 * @GUPNP_DLNA_FLAGS_S0_INCREASE: Content does not have a fixed beginning
 * @GUPNP_DLNA_FLAGS_SN_INCREASE: Content does not have a fixed end
 * @GUPNP_DLNA_FLAGS_RTSP_PAUSE: RTSP resource supports pausing of media
 *                               transfer
 * @GUPNP_DLNA_FLAGS_STREAMING_TRANSFER_MODE: Streaming transfer mode supported
 * @GUPNP_DLNA_FLAGS_INTERACTIVE_TRANSFER_MODE: Interactive transfer mode
 *                                              supported
 * @GUPNP_DLNA_FLAGS_BACKGROUND_TRANSFER_MODE: Background transfer mode
 *                                             supported
 * @GUPNP_DLNA_FLAGS_CONNECTION_STALL: No content transfer when paused.
 * @GUPNP_DLNA_FLAGS_DLNA_V15: DLNAv1.5 version flag
 *
 * The miscellaneous operations supported by a resource. For details on these
 * flags please refer to section 7.3.37.2 of DLNA Networked Device
 * Interoperability Guidelines Volume 1, October 2006.
 *
 **/
typedef enum {
        GUPNP_DLNA_FLAGS_NONE                      = 0,
        GUPNP_DLNA_FLAGS_SENDER_PACED              = (1 << 31),
        GUPNP_DLNA_FLAGS_TIME_BASED_SEEK           = (1 << 30),
        GUPNP_DLNA_FLAGS_BYTE_BASED_SEEK           = (1 << 29),
        GUPNP_DLNA_FLAGS_PLAY_CONTAINER            = (1 << 28),
        GUPNP_DLNA_FLAGS_S0_INCREASE               = (1 << 27),
        GUPNP_DLNA_FLAGS_SN_INCREASE               = (1 << 26),
        GUPNP_DLNA_FLAGS_RTSP_PAUSE                = (1 << 25),
        GUPNP_DLNA_FLAGS_STREAMING_TRANSFER_MODE   = (1 << 24),
        GUPNP_DLNA_FLAGS_INTERACTIVE_TRANSFER_MODE = (1 << 23),
        GUPNP_DLNA_FLAGS_BACKGROUND_TRANSFER_MODE  = (1 << 22),
        GUPNP_DLNA_FLAGS_CONNECTION_STALL          = (1 << 21),
        GUPNP_DLNA_FLAGS_DLNA_V15                  = (1 << 20)
} GUPnPDLNAFlags;

/**
 * GUPnPOCMFlags:
 * @GUPNP_OCM_FLAGS_NONE: No flags
 * @GUPNP_OCM_FLAGS_UPLOAD: Indicates support for content upload.
 * @GUPNP_OCM_FLAGS_CREATE_CONTAINER: Indicates support for creation of child
 *                                    container.
 * @GUPNP_OCM_FLAGS_DESTROYABLE: This object is destroyable.
 * @GUPNP_OCM_FLAGS_UPLOAD_DESTROYABLE: Indicates support for upload of
 *                                      destroyable content.
 * @GUPNP_OCM_FLAGS_CHANGE_METADATA: Indicates support for changing metadata.
 *
 * The DLNA OCM flags supported by a DIDL-Lite Object. For details on these
 * flags please refer to section 7.3.118.4 of DLNA Networked Device
 * Interoperability Guidelines Volume 1, October 2006.
 *
 **/
typedef enum {
        GUPNP_OCM_FLAGS_NONE               = 0x0,
        GUPNP_OCM_FLAGS_UPLOAD             = 0x01,
        GUPNP_OCM_FLAGS_CREATE_CONTAINER   = 0x02,
        GUPNP_OCM_FLAGS_DESTROYABLE        = 0x04,
        GUPNP_OCM_FLAGS_UPLOAD_DESTROYABLE = 0x08,
        GUPNP_OCM_FLAGS_CHANGE_METADATA    = 0x10
} GUPnPOCMFlags;

GType
gupnp_ocm_flags_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_OCM_FLAGS (gupnp_ocm_flags_get_type ())

G_END_DECLS

#endif /* __GUPNP_DLNA_H__ */
