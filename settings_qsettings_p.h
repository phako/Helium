#ifndef SETTINGS_QSETTINGS_P_H
#define SETTINGS_QSETTINGS_P_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QSettings>

#include "settings.h"

class SettingsPrivate : QObject {
    Q_OBJECT
public:
    SettingsPrivate(Settings *parent);
    void set(const QString &key, const QVariant &value);
    ~SettingsPrivate();
public Q_SLOTS:
    void onFileChanged(const QString &path);
private:
    QSettings m_settings;
    QFileSystemWatcher m_monitor;
    QMap<QString,QVariant> m_valueCache;
    Settings * const q_ptr;
    Q_DECLARE_PUBLIC(Settings)
};

#endif // SETTINGS_QSETTINGS_P_H
