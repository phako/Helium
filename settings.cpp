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

#include <gq/GConfItem>

static const QString GCONF_PREFIX = QLatin1String("/apps/ControlPanel/Helium");
static const QString DISPLAY_DEVICE_ICONS = GCONF_PREFIX + QLatin1String("/Display/show-device-icons");
static const QString DISPLAY_MEDIA_ART = GCONF_PREFIX + QLatin1String("/Display/show-media-art");

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_configItems()
{
    m_configItems[DISPLAY_DEVICE_ICONS] = new GConfItem(DISPLAY_DEVICE_ICONS);
    connect (m_configItems[DISPLAY_DEVICE_ICONS], SIGNAL(valueChanged()), SIGNAL(displayDeviceIconsChanged()));

    m_configItems[DISPLAY_MEDIA_ART] = new GConfItem(DISPLAY_MEDIA_ART);
    connect (m_configItems[DISPLAY_MEDIA_ART], SIGNAL(valueChanged()), SIGNAL(displayMediaArtChanged()));
}

Settings::~Settings()
{
    Q_FOREACH (GConfItem *item, m_configItems.values()) {
        item->deleteLater();
    }
}

bool Settings::displayDeviceIcons(void)
{
    return m_configItems[DISPLAY_DEVICE_ICONS]->value().toBool();
}

void Settings::setDisplayDeviceIcons(bool value)
{
    m_configItems[DISPLAY_DEVICE_ICONS]->set(QVariant::fromValue(value));
}

bool Settings::displayMediaArt(void)
{
    return m_configItems[DISPLAY_MEDIA_ART]->value().toBool();
}

void Settings::setDisplayMediaArt(bool value)
{
    m_configItems[DISPLAY_MEDIA_ART]->set(QVariant::fromValue(value));
}
