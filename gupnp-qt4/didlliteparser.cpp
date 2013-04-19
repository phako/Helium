/*
This file is part of Helium.

Helium is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Helium is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Helium.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <libgupnp-av/gupnp-av.h>

#include "didlliteparser.h"
#include "didlliteparser_p.h"

/*!
 * \brief static call-back for gupnp_didl_lite_parser_parse_didl
 * \param parser unused
 * \param object the newly created object
 * \param user_data pointer to the internal list of objects in d ptr
 */
static void on_didl_lite_object_available(GUPnPDIDLLiteParser *parser, GUPnPDIDLLiteObject *object, gpointer user_data)
{
    Q_UNUSED(parser);

    auto list = static_cast<DIDLLiteObjectList*>(user_data);
    *list << DIDLLiteObject(object);
}

DIDLLiteParserPrivate::DIDLLiteParserPrivate(DIDLLiteParser *parent)
    : QObject(parent)
    , m_parser(wrap(gupnp_didl_lite_parser_new()))
    , m_objects()
    , m_lastError(0)
    , q_ptr(parent)
{
    g_signal_connect(m_parser.data(), "object-available", G_CALLBACK(on_didl_lite_object_available), &m_objects);
}

DIDLLiteParserPrivate::~DIDLLiteParserPrivate()
{
    if (m_lastError != 0) {
        g_error_free(m_lastError);
        m_lastError = NULL;
    }
}

DIDLLiteParser::DIDLLiteParser(QObject *parent)
    : QObject(parent)
    , d_ptr(new DIDLLiteParserPrivate(this))
{
}

/*!
 * \brief Parse a DIDLLite XML snippet into GUPnPDIDLLiteObjects.
 *
 * \param didlLite the DIDLLite XML snippet to parse
 * \return a list of parsed objects or an empty list if an error occured.
 * \sa hasError(), errorMessage()
 */
DIDLLiteObjectList DIDLLiteParser::parse(const QString &didlLite)
{
    return parse(didlLite.toUtf8().constData());
}

/*!
 * \brief Parse a DIDLLite XML snippet into GUPnPDIDLLiteObjects.
 *
 * \param didlLite the DIDLLite XML snippet to parse
 * \return a list of parsed objects or an empty list if an error occured.
 * \sa hasError(), errorMessage()
 */
DIDLLiteObjectList DIDLLiteParser::parse(const char *didlLite)
{
    Q_D(DIDLLiteParser);
    GError *error = 0;

    d->m_objects.clear();

    if (gupnp_didl_lite_parser_parse_didl(d->m_parser, didlLite, &error) == FALSE) {
        d->m_lastError = error;
        qDebug() << "Faild to parse DIDLLite XML" << error->message;
    }

    return d->m_objects;
}

/*!
 * \brief Check if last parsing operation had an error
 * \return true, if last parse failed, false otherwise.
 * \sa errorMessage()
 */
bool DIDLLiteParser::hasError() const
{
    Q_D(const DIDLLiteParser);

    return d->m_lastError != 0;
}

/*!
 * \brief Get the error message of the last parse error.
 * \return empty string if no error, message of the last error otherwise.
 */
QString DIDLLiteParser::errorMessage() const
{
    Q_D(const DIDLLiteParser);

    if (not hasError()) {
        return QString();
    }

    return QString::fromUtf8(d->m_lastError->message);
}
