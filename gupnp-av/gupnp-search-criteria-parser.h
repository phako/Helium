/*
 * Copyright (C) 2008 OpenedHand Ltd.
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

#ifndef __GUPNP_SEARCH_CRITERIA_PARSER_H__
#define __GUPNP_SEARCH_CRITERIA_PARSER_H__

#include <glib-object.h>

G_BEGIN_DECLS

GType
gupnp_search_criteria_parser_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_SEARCH_CRITERIA_PARSER \
                (gupnp_search_criteria_parser_get_type ())
#define GUPNP_SEARCH_CRITERIA_PARSER(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 GUPNP_TYPE_SEARCH_CRITERIA_PARSER, \
                 GUPnPSearchCriteriaParser))
#define GUPNP_SEARCH_CRITERIA_PARSER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_CAST ((obj), \
                 GUPNP_TYPE_SEARCH_CRITERIA_PARSER, \
                 GUPnPSearchCriteriaParserClass))
#define GUPNP_IS_SEARCH_CRITERIA_PARSER(obj) \
                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                 GUPNP_TYPE_SEARCH_CRITERIA_PARSER))
#define GUPNP_IS_SEARCH_CRITERIA_PARSER_CLASS(obj) \
                (G_TYPE_CHECK_CLASS_TYPE ((obj), \
                 GUPNP_TYPE_SEARCH_CRITERIA_PARSER))
#define GUPNP_SEARCH_CRITERIA_PARSER_GET_CLASS(obj) \
                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                 GUPNP_TYPE_SEARCH_CRITERIA_PARSER, \
                 GUPnPSearchCriteriaParserClass))

/**
 * GUPnPSearchCriteriaOp:
 * @GUPNP_SEARCH_CRITERIA_OP_EQ: '='
 * @GUPNP_SEARCH_CRITERIA_OP_NEQ: '!='
 * @GUPNP_SEARCH_CRITERIA_OP_LESS: '<'
 * @GUPNP_SEARCH_CRITERIA_OP_LEQ: '<='
 * @GUPNP_SEARCH_CRITERIA_OP_GREATER: '>'
 * @GUPNP_SEARCH_CRITERIA_OP_GEQ: '>='
 * @GUPNP_SEARCH_CRITERIA_OP_CONTAINS: 'contains'
 * @GUPNP_SEARCH_CRITERIA_OP_DOES_NOT_CONTAIN: 'doesNotContain'
 * @GUPNP_SEARCH_CRITERIA_OP_DERIVED_FROM: 'derivedFrom'
 * @GUPNP_SEARCH_CRITERIA_OP_EXISTS: 'exists'
 *
 * The possible operators in SearchCriteria strings.
 *
 **/
typedef enum {
        /* G_TYPE_STRING */
        GUPNP_SEARCH_CRITERIA_OP_EQ               = G_TOKEN_LAST + 1,
        GUPNP_SEARCH_CRITERIA_OP_NEQ              = G_TOKEN_LAST + 2,
        GUPNP_SEARCH_CRITERIA_OP_LESS             = G_TOKEN_LAST + 3,
        GUPNP_SEARCH_CRITERIA_OP_LEQ              = G_TOKEN_LAST + 4,
        GUPNP_SEARCH_CRITERIA_OP_GREATER          = G_TOKEN_LAST + 5,
        GUPNP_SEARCH_CRITERIA_OP_GEQ              = G_TOKEN_LAST + 6,
        GUPNP_SEARCH_CRITERIA_OP_CONTAINS         = G_TOKEN_LAST + 7,
        GUPNP_SEARCH_CRITERIA_OP_DOES_NOT_CONTAIN = G_TOKEN_LAST + 8,
        GUPNP_SEARCH_CRITERIA_OP_DERIVED_FROM     = G_TOKEN_LAST + 9,

        /* G_TYPE_BOOLEAN */
        GUPNP_SEARCH_CRITERIA_OP_EXISTS           = G_TOKEN_LAST + 10
} GUPnPSearchCriteriaOp;

GType
gupnp_search_criteria_op_get_type (void) G_GNUC_CONST;

#define GUPNP_TYPE_SEARCH_CRITERIA_OP (gupnp_search_criteria_op_get_type ())

#define GUPNP_SEARCH_CRITERIA_PARSER_ERROR \
                (gupnp_search_criteria_parser_error_quark ())

GQuark
gupnp_search_criteria_parser_error_quark (void);

typedef enum {
        GUPNP_SEARCH_CRITERIA_PARSER_ERROR_FAILED
} GUPnPSearchCriteriaParserError;

typedef struct _GUPnPSearchCriteriaParserPrivate
                GUPnPSearchCriteriaParserPrivate;

typedef struct {
        GObject parent;

        GUPnPSearchCriteriaParserPrivate *priv;
} GUPnPSearchCriteriaParser;

typedef struct {
        GObjectClass parent_class;

        /* signals */
        void     (* begin_parens) (GUPnPSearchCriteriaParser  *parser);
        void     (* end_parens)   (GUPnPSearchCriteriaParser  *parser);
        void     (* conjunction)  (GUPnPSearchCriteriaParser  *parser);
        void     (* disjunction)  (GUPnPSearchCriteriaParser  *parser);
        gboolean (* expression)   (GUPnPSearchCriteriaParser  *parser,
                                   const char                 *property,
                                   GUPnPSearchCriteriaOp       op,
                                   const char                 *value,
                                   GError                    **error);

        /* future padding */
        void (* _gupnp_reserved1) (void);
        void (* _gupnp_reserved2) (void);
        void (* _gupnp_reserved3) (void);
        void (* _gupnp_reserved4) (void);
} GUPnPSearchCriteriaParserClass;

GUPnPSearchCriteriaParser *
gupnp_search_criteria_parser_new        (void);

gboolean
gupnp_search_criteria_parser_parse_text (GUPnPSearchCriteriaParser *parser,
                                         const char                *text,
                                         GError                   **error);

G_END_DECLS

#endif /* __GUPNP_SEARCH_CRITERIA_PARSER_H__ */
