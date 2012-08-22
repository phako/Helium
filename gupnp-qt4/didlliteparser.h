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

#ifndef DIDLLITEPARSER_H
#define DIDLLITEPARSER_H

#include <libgupnp-av/gupnp-didl-lite-object.h>

#include <QObject>

#include "refptrg.h"

typedef RefPtrG<GUPnPDIDLLiteObject> DIDLLiteObject;
typedef QList<DIDLLiteObject> DIDLLiteObjectList;

class DIDLLiteParserPrivate;
class DIDLLiteParser : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DIDLLiteParser)
public:
    explicit DIDLLiteParser(QObject *parent = 0);
    DIDLLiteObjectList parse(const QString &didlLite);
    DIDLLiteObjectList parse(const char *didlLite);
    bool hasError() const;
    QString errorMessage() const;

private:
    DIDLLiteParserPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(DIDLLiteParser)
};

#endif // DIDLLITEPARSER_H
