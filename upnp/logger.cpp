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

#include <QDateTime>
#include <QDir>

#include "logger.h"
#include "logger_p.h"
#include "settings.h"

QThread LoggerPrivate::loggerThread;
int LoggerPrivate::instanceCount = 0;

LoggerPrivate::LoggerPrivate(GUPnPContext *context, Logger *parent)
    : m_handler(QLatin1String("Helium-") +
                QLatin1String(gssdp_client_get_host_ip(GSSDP_CLIENT(context))) +
                QLatin1String("-") +
                QString::number(QDateTime::currentDateTimeUtc().toTime_t()) +
                QLatin1String(".log"))
    , m_logger(wrap(soup_logger_new(SOUP_LOGGER_LOG_BODY, -1)))
    , m_context(context)
    , q_ptr(parent)
{
    auto session = gupnp_context_get_session(context);
    soup_logger_set_printer(m_logger, LoggerPrivate::printer, this, 0);
    soup_session_add_feature(session, SOUP_SESSION_FEATURE(m_logger));

    connect (this, SIGNAL(dataAvailable(int,char,QByteArray *)),
             &m_handler, SLOT(onData(int,char,QByteArray *)));

    m_handler.moveToThread(&LoggerPrivate::loggerThread);
    if (not LoggerPrivate::loggerThread.isRunning()) {
        LoggerPrivate::loggerThread.start();
    }
    LoggerPrivate::instanceCount++;
}

LoggerPrivate::~LoggerPrivate()
{
    soup_session_remove_feature(gupnp_context_get_session(m_context),
                                SOUP_SESSION_FEATURE(m_logger));
    if (LoggerPrivate::instanceCount > 0) {
        LoggerPrivate::instanceCount--;
    }

    // Shutdown thread
    if (LoggerPrivate::instanceCount == 0) {
        LoggerPrivate::loggerThread.quit();
    }
}

void LoggerPrivate::printer(SoupLogger *logger, SoupLoggerLogLevel level, char direction, const char *data, gpointer user_data)
{
    auto self = static_cast<LoggerPrivate *>(user_data);

    if (logger != self->m_logger.data()) {
        return;
    }

    Q_EMIT self->dataAvailable(level, direction, new QByteArray(data));
}

LoggerHandler::LoggerHandler(const QString &file)
    : QObject(0)
    , m_file(Settings().debugPath() + QDir::separator() + file)
    , m_stream(&m_file)
{
    if (!QFile::exists(Settings().debugPath())) {
        QDir().mkpath(Settings().debugPath());
    }

    m_file.open(QIODevice::Append | QIODevice::ReadWrite);
}

LoggerHandler::~LoggerHandler()
{
    m_file.flush();
}

void LoggerHandler::onData(int level, char direction, QByteArray *line)
{
    Q_UNUSED(level);
    m_stream << direction << " " << line->constData() << "\n";
    delete line;
}

Logger::Logger(GUPnPContext *context, QObject *parent)
    : QObject(parent)
    , d_ptr(new LoggerPrivate(context, this))
{
}

GUPnPContext *Logger::getContext() const
{
    Q_D(const Logger);

    return d->m_context;
}
