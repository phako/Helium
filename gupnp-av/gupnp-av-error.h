/*
 * Copyright (C) 2009, Nokia Corporation.
 * Copyright (C) 2006, 2007 OpenedHand Ltd.
 *
 * Authors: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *                                <zeeshan.ali@nokia.com>
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

#ifndef __GUPNP_AV_ERROR_H__
#define __GUPNP_AV_ERROR_H__

#include <glib.h>

G_BEGIN_DECLS

GQuark
gupnp_protocol_error_quark (void) G_GNUC_CONST;

#define GUPNP_PROTOCOL_ERROR (gupnp_protocol_error_quark ())

/**
 * GUPnPProtocolError:
 * @GUPNP_PROTOCOL_ERROR_INVALID_SYNTAX: Invalid syntax.
 * @GUPNP_PROTOCOL_ERROR_OTHER: Unknown/unhandled protocol related errors.
 *
 * #GError codes used for errors in the #GUPNP_PROTOCOL_ERROR domain, upon any
 * protocol related errors.
 */
typedef enum {
        GUPNP_PROTOCOL_ERROR_INVALID_SYNTAX,
        GUPNP_PROTOCOL_ERROR_OTHER
} GUPnPProtocolError;

G_END_DECLS

#endif /* __GUPNP_AV_ERROR_H__ */
