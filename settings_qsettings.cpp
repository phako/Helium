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
#include <QSettings>

#ifdef HARMATTAN
#error Should not be reached
#endif

static const QString DISPLAY_DEVICE_ICONS = QLatin1String("Display/show-device-icons");
static const QString DISPLAY_MEDIA_ART = QLatin1String("Display/show-media-art");
static const QString START_MEDIA_SHARING = QLatin1String("Sharing/start-media-sharing");
static const QString SHOW_DEVICE_POPUP = QLatin1String ("Display/show-device-popup");
static const QString FILTER_IN_DETAILS = QLatin1String ("Display/filter-in-details");


class  SettingsPrivate {
public:
    SettingsPrivate();
    ~SettingsPrivate();
    QSettings m_settings;
};

SettingsPrivate::SettingsPrivate()
    : m_settings(QLatin1String("org.jensge"), QLatin1String("Helium"))
{
}

SettingsPrivate::~SettingsPrivate()
{
    m_settings.sync();
}

Settings::Settings(QObject *parent)
    : QObject(parent)
    , d_ptr(new SettingsPrivate)
{
}

Settings::~Settings()
{
}

bool Settings::displayDeviceIcons(void)
{
    Q_D(Settings);

    return d->m_settings.value(DISPLAY_DEVICE_ICONS, false).toBool();
}

void Settings::setDisplayDeviceIcons(bool value)
{
    Q_D(Settings);

    d->m_settings.setValue(DISPLAY_DEVICE_ICONS, value);
    d->m_settings.sync();

    Q_EMIT displayDeviceIconsChanged();
}

bool Settings::displayMediaArt(void)
{
    Q_D(Settings);

    return d->m_settings.value(DISPLAY_MEDIA_ART, true).toBool();
}

void Settings::setDisplayMediaArt(bool value)
{
    Q_D(Settings);

    d->m_settings.setValue(DISPLAY_MEDIA_ART, value);
    d->m_settings.sync();
    Q_EMIT displayMediaArtChanged();
}

bool Settings::startMediaSharing(void)
{
    return false;
}

void Settings::setStartMediaSharing(bool value)
{
    Q_UNUSED(value);
}

bool Settings::mediaSharingAvailable()
{
    return false;
}

bool Settings::showDevicePopUp(void)
{
    Q_D(Settings);

    return d->m_settings.value(SHOW_DEVICE_POPUP, true).toBool();
}

void Settings::setShowDevicePopUp(bool value)
{
    Q_D(Settings);

    d->m_settings.setValue(SHOW_DEVICE_POPUP, value);
    d->m_settings.sync();
    Q_EMIT showDevicePopUpChanged();
}

bool Settings::filterInDetails(void)
{
    Q_D(Settings);

    bool val = d->m_settings.value(FILTER_IN_DETAILS, false).toBool();

    qDebug() << "filterInDetails()" << val;

    return val;
}

void Settings::setFilterInDetails(bool value)
{
    Q_D(Settings);

    d->m_settings.setValue(FILTER_IN_DETAILS, value);
    d->m_settings.sync();
    Q_EMIT filterInDetailsChanged();
}
