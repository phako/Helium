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

class SettingsPrivate;
class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool displayDeviceIcons READ displayDeviceIcons WRITE setDisplayDeviceIcons NOTIFY displayDeviceIconsChanged)
    Q_PROPERTY(bool displayMediaArt READ displayMediaArt WRITE setDisplayMediaArt NOTIFY displayMediaArtChanged)
    Q_PROPERTY(bool startMediaSharing READ startMediaSharing WRITE setStartMediaSharing NOTIFY startMediaSharingChanged)
    Q_PROPERTY(bool mediaSharingAvailable READ mediaSharingAvailable NOTIFY mediaSharingAvailableChanged)
    Q_PROPERTY(bool showDevicePopUp READ showDevicePopUp WRITE setShowDevicePopUp NOTIFY showDevicePopUpChanged)
    Q_PROPERTY(bool filterInDetails READ filterInDetails WRITE setFilterInDetails NOTIFY filterInDetailsChanged)
public:
    static const QString RYGEL_DBUS_IFACE;

    explicit Settings(QObject *parent = 0);
    ~Settings();

    bool displayDeviceIcons(void);
    void setDisplayDeviceIcons(bool value);

    bool displayMediaArt(void);
    void setDisplayMediaArt(bool value);

    bool startMediaSharing(void);
    void setStartMediaSharing(bool value);

    bool mediaSharingAvailable(void);

    bool showDevicePopUp(void);
    void setShowDevicePopUp(bool value);

    bool filterInDetails(void);
    void setFilterInDetails(bool value);

Q_SIGNALS:
    void displayDeviceIconsChanged(void);
    void displayMediaArtChanged(void);
    void startMediaSharingChanged(void);
    void mediaSharingAvailableChanged(void);
    void showDevicePopUpChanged(void);
    void filterInDetailsChanged(void);

private:
    SettingsPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Settings)
};

#endif // SETTINGS_H
