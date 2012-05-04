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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QMap>

class GConfItem;

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool displayDeviceIcons READ displayDeviceIcons WRITE setDisplayDeviceIcons NOTIFY displayDeviceIconsChanged)
    Q_PROPERTY(bool displayMediaArt READ displayMediaArt WRITE setDisplayMediaArt NOTIFY displayMediaArtChanged)
public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    Q_INVOKABLE bool displayDeviceIcons(void);
    Q_INVOKABLE void setDisplayDeviceIcons(bool value);

    Q_INVOKABLE bool displayMediaArt(void);
    Q_INVOKABLE void setDisplayMediaArt(bool value);

Q_SIGNALS:
    void displayDeviceIconsChanged(void);
    void displayMediaArtChanged(void);

public Q_SLOTS:
private:
    QMap<QString, GConfItem *> m_configItems;
};

#endif // SETTINGS_H
