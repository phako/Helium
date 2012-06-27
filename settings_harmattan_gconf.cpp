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

#include "settings.h"

#include <QDebug>

#include <QtDBus/QDBusInterface>

#include <gq/GConfItem>

class SettingsPrivate {
public:
    SettingsPrivate();
    ~SettingsPrivate();
    QMap<QString, GConfItem *> m_configItems;
    bool m_sharingAvailable;
};

static const QString GCONF_PREFIX = QLatin1String("/apps/ControlPanel/Helium");
static const QString DISPLAY_DEVICE_ICONS = GCONF_PREFIX + QLatin1String("/Display/show-device-icons");
static const QString DISPLAY_MEDIA_ART = GCONF_PREFIX + QLatin1String("/Display/show-media-art");
static const QString START_MEDIA_SHARING = GCONF_PREFIX + QLatin1String("/Sharing/start-media-sharing");
static const QString SHOW_DEVICE_POPUP = GCONF_PREFIX + QLatin1String ("/Display/show-device-popup");
static const QString FILTER_IN_DETAILS = GCONF_PREFIX + QLatin1String ("/Display/filter-in-details");

const QString Settings::RYGEL_DBUS_IFACE = QLatin1String("org.gnome.Rygel1");

SettingsPrivate::SettingsPrivate()
    : m_configItems(),
      m_sharingAvailable(false)
{
    m_configItems[DISPLAY_DEVICE_ICONS] = new GConfItem(DISPLAY_DEVICE_ICONS);
    m_configItems[DISPLAY_MEDIA_ART] = new GConfItem(DISPLAY_MEDIA_ART);
    m_configItems[START_MEDIA_SHARING] = new GConfItem(START_MEDIA_SHARING);
    m_configItems[SHOW_DEVICE_POPUP] = new GConfItem(SHOW_DEVICE_POPUP);
    m_configItems[FILTER_IN_DETAILS] = new GConfItem(FILTER_IN_DETAILS);

    QDBusInterface fdo(QLatin1String("org.freedesktop.DBus"), QLatin1String("/"), QLatin1String("org.freedesktop.DBus"));
    m_sharingAvailable = fdo.call(QLatin1String("ListActivatableNames")).arguments().first().toStringList().contains(RYGEL_DBUS_IFACE);
}

SettingsPrivate::~SettingsPrivate()
{
    Q_FOREACH (GConfItem *item, m_configItems.values()) {
        item->deleteLater();
    }
}

Settings::Settings(QObject *parent)
    : QObject(parent)
    , d_ptr(new SettingsPrivate)
{
    Q_D(Settings);

    connect (d->m_configItems[DISPLAY_DEVICE_ICONS], SIGNAL(valueChanged()), SIGNAL(displayDeviceIconsChanged()));
    connect (d->m_configItems[DISPLAY_MEDIA_ART], SIGNAL(valueChanged()), SIGNAL(displayMediaArtChanged()));
    connect (d->m_configItems[START_MEDIA_SHARING], SIGNAL(valueChanged()), SIGNAL(startMediaSharingChanged()));
    connect (d->m_configItems[SHOW_DEVICE_POPUP], SIGNAL(valueChanged()), SIGNAL(showDevicePopUpChanged()));
    connect (d->m_configItems[FILTER_IN_DETAILS], SIGNAL(valueChanged()), SIGNAL(filterInDetailsChanged()));
}

Settings::~Settings()
{
    delete d_ptr;
}

bool Settings::displayDeviceIcons(void)
{
    Q_D(Settings);

    return d->m_configItems[DISPLAY_DEVICE_ICONS]->value().toBool();
}

void Settings::setDisplayDeviceIcons(bool value)
{
    Q_D(Settings);

    d->m_configItems[DISPLAY_DEVICE_ICONS]->set(QVariant::fromValue(value));
}

bool Settings::displayMediaArt(void)
{
    Q_D(Settings);

    return d->m_configItems[DISPLAY_MEDIA_ART]->value().toBool();
}

void Settings::setDisplayMediaArt(bool value)
{
    Q_D(Settings);

    d->m_configItems[DISPLAY_MEDIA_ART]->set(QVariant::fromValue(value));
}

bool Settings::startMediaSharing(void)
{
    Q_D(Settings);

    return d->m_configItems[START_MEDIA_SHARING]->value().toBool();
}

bool Settings::mediaSharingAvailable()
{
    Q_D(Settings);

    return d->m_sharingAvailable;
}

void Settings::setStartMediaSharing(bool value)
{
    Q_D(Settings);

    d->m_configItems[START_MEDIA_SHARING]->set(QVariant::fromValue(value));
}

bool Settings::showDevicePopUp(void)
{
    Q_D(Settings);

    return d->m_configItems[SHOW_DEVICE_POPUP]->value().toBool();
}

void Settings::setShowDevicePopUp(bool value)
{
    Q_D(Settings);

    qDebug() << "Changing showDevicePopUp" << value;
    d->m_configItems[SHOW_DEVICE_POPUP]->set(QVariant::fromValue(value));
}

bool Settings::filterInDetails(void)
{
    Q_D(Settings);

    return d->m_configItems[FILTER_IN_DETAILS]->value().toBool();
}

void Settings::setFilterInDetails(bool value)
{
    Q_D(Settings);

    d->m_configItems[FILTER_IN_DETAILS]->set(value);
}
