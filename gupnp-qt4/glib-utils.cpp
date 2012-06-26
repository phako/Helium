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

#include "glib-utils.h"

/*!
 * \brief Convert a GValue to a QVariant
 * \param other GValue to convert
 * \return a QVariant with the GValue if it could be converted, an invalid
 *         QVariant otherwise.
 */
QVariant gValueToQVariant(const GValue *other)
{
    switch (G_VALUE_TYPE(other)) {
    case G_TYPE_BOOLEAN:
        return QVariant::fromValue(g_value_get_boolean(other) == TRUE);
    case G_TYPE_UINT:
        return QVariant::fromValue(g_value_get_uint(other));
    case G_TYPE_UINT64:
        return QVariant::fromValue(g_value_get_uint64(other));
    case G_TYPE_INT:
        return QVariant::fromValue(g_value_get_int(other));
    case G_TYPE_INT64:
        return QVariant::fromValue(g_value_get_int64(other));
    case G_TYPE_DOUBLE:
        return QVariant::fromValue(g_value_get_double(other));
    case G_TYPE_STRING:
        return QVariant::fromValue(QString::fromUtf8(g_value_get_string(other)));
    default:
        return QVariant();
    }
}

/*!
 * \brief Convert a QVariant to GValue
 * \param other QVariant to convert
 * \return a GValue holding the QVariant's content or an invalid GValue if it
 *         could not be converted.
 */
GValue *qVariantToGValue(const QVariant &other)
{
    GValue *gvalue = g_new0(GValue, 1);
    switch (other.type()) {
    case QVariant::Bool:
        g_value_init(gvalue, G_TYPE_BOOLEAN);
        g_value_set_boolean(gvalue, other.toBool() ? TRUE : FALSE);
        break;
    case QVariant::UInt:
        g_value_init(gvalue, G_TYPE_UINT);
        g_value_set_uint(gvalue, other.toUInt());
        break;
    case QVariant::Int:
        g_value_init(gvalue, G_TYPE_INT);
        g_value_set_int(gvalue, other.toInt());
        break;
    case QVariant::LongLong:
        g_value_init(gvalue, G_TYPE_INT64);
        g_value_set_int64(gvalue, other.toLongLong());
        break;
    case QVariant::ULongLong:
        g_value_init(gvalue, G_TYPE_UINT64);
        g_value_set_int64(gvalue, other.toULongLong());
        break;
    case QVariant::Double:
        g_value_init(gvalue, G_TYPE_DOUBLE);
        g_value_set_double(gvalue, other.toDouble());
        break;
    case QVariant::String:
        g_value_init(gvalue, G_TYPE_STRING);
        g_value_set_string(gvalue, other.toString().toUtf8().constData());
        break;
    default:
        // do nothing - probably assert or whatever
        break;
    }

    return gvalue;
}
