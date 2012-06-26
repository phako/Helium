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

#ifndef DIDLLITEPARSER_P_H
#define DIDLLITEPARSER_P_H

#include "didlliteparser.h"

class DIDLLiteParserPrivate : QObject {
    Q_OBJECT
public:
    DIDLLiteParserPrivate(DIDLLiteParser *parent);

    RefPtrG<GUPnPDIDLLiteParser> m_parser;
    DIDLLiteObjectList m_objects;
    GError *m_lastError;

    DIDLLiteParser * const q_ptr;
    Q_DECLARE_PUBLIC(DIDLLiteParser)
};

#endif // DIDLLITEPARSER_P_H
