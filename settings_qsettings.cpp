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


#include <QDebug>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QStringList>

#include "settings.h"
#include "settings_qsettings_p.h"

#ifdef HARMATTAN
#error Should not be reached
#endif

static const QString DISPLAY_DEVICE_ICONS = QLatin1String("Display/show-device-icons");
static const QString DISPLAY_MEDIA_ART = QLatin1String("Display/show-media-art");
static const QString START_MEDIA_SHARING = QLatin1String("Sharing/start-media-sharing");
static const QString SHOW_DEVICE_POPUP = QLatin1String ("Display/show-device-popup");
static const QString FILTER_IN_DETAILS = QLatin1String ("Display/filter-in-details");
static const QString DEBUG = QLatin1String ("Debug/enable");
static const QString DEBUG_PATH = QLatin1String ("Debug/output-path");

SettingsPrivate::SettingsPrivate(Settings *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, QLatin1String("org.jensge"),
                 QLatin1String("Helium"))
    , m_monitor()
    , m_valueCache()
    , q_ptr(parent)
{
    m_monitor.addPath(m_settings.fileName());
    connect(&m_monitor, SIGNAL(fileChanged(QString)),
            this, SLOT(onFileChanged(QString)));
}

void SettingsPrivate::set(const QString &key, const QVariant &value)
{
    m_valueCache[key] = value;
    m_settings.setValue(key, value);
    m_settings.sync();
}

SettingsPrivate::~SettingsPrivate()
{
}

void SettingsPrivate::onFileChanged(const QString &path)
{
    Q_Q(Settings);
    Q_UNUSED(path);

    if (m_valueCache[DISPLAY_DEVICE_ICONS] != q->displayDeviceIcons()) {
        m_valueCache[DISPLAY_DEVICE_ICONS] = q->displayDeviceIcons();
        Q_EMIT q->displayDeviceIconsChanged();
    }

    if (m_valueCache[DISPLAY_MEDIA_ART] != q->displayMediaArt()) {
        m_valueCache[DISPLAY_MEDIA_ART] = q->displayMediaArt();
        Q_EMIT q->displayMediaArtChanged();
    }

    if (m_valueCache[SHOW_DEVICE_POPUP] != q->showDevicePopUp()) {
        m_valueCache[SHOW_DEVICE_POPUP] = q->showDevicePopUp();
        Q_EMIT q->showDevicePopUp();
    }

    if (m_valueCache[FILTER_IN_DETAILS] != q->filterInDetails()) {
        m_valueCache[FILTER_IN_DETAILS] = q->filterInDetails();
        Q_EMIT q->filterInDetailsChanged();
    }

    if (m_valueCache[DEBUG] != q->debug()) {
        m_valueCache[DEBUG] = q->debug();
        Q_EMIT q->debugChanged();
    }

    if (m_valueCache[DEBUG_PATH] != q->debugPath()) {
        m_valueCache[DEBUG_PATH] = q->debugPath();
        Q_EMIT q->debugPathChanged();
    }
}

Settings::Settings(QObject *parent)
    : QObject(parent)
    , d_ptr(new SettingsPrivate(this))
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

    d->set(DISPLAY_DEVICE_ICONS, value);

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

    d->set(DISPLAY_MEDIA_ART, value);
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

    d->set(SHOW_DEVICE_POPUP, value);
    Q_EMIT showDevicePopUpChanged();
}

bool Settings::filterInDetails(void)
{
    Q_D(Settings);

    bool val = d->m_settings.value(FILTER_IN_DETAILS, false).toBool();

    return val;
}

void Settings::setFilterInDetails(bool value)
{
    Q_D(Settings);

    d->set(FILTER_IN_DETAILS, value);
    Q_EMIT filterInDetailsChanged();
}

bool Settings::debug(void)
{
    Q_D(Settings);
    bool val = d->m_settings.value(DEBUG, false).toBool();

    return val;
}

void Settings::setDebug(bool value)
{
    Q_D(Settings);

    d->set(DEBUG, value);
    Q_EMIT debugChanged();
}

QString Settings::debugPath()
{
    Q_D(Settings);

    return d->m_settings.value(DEBUG_PATH, QLatin1String("/tmp")).toString();
}

void Settings::setDebugPath(const QString &value)
{
    Q_D(Settings);

    d->set(DEBUG_PATH, value);
    Q_EMIT debugPathChanged();
}
