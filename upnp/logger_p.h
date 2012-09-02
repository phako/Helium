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

#ifndef LOGGER_P_H
#define LOGGER_P_H

#include <libsoup/soup.h>

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QThread>

#include "refptrg.h"
#include "logger.h"

typedef RefPtrG<SoupLogger> GSoupLogger;

class LoggerHandler : public QObject {
    Q_OBJECT
public:
    LoggerHandler(const QString &file);
    ~LoggerHandler();
public Q_SLOTS:
    void onData(int level, char direction, QByteArray *line);
private:
    QFile m_file;
    QTextStream m_stream;
};

class LoggerPrivate : public QObject {
    Q_OBJECT
    static QThread loggerThread;
    static int instanceCount;
public:
    LoggerPrivate(GUPnPContext *context, Logger *parent);
    ~LoggerPrivate();
    static void printer(SoupLogger *logger, SoupLoggerLogLevel level, char direction, const char *data, gpointer user_data);
Q_SIGNALS:
    void dataAvailable(int level, char direction, QByteArray *line);

private:
    LoggerHandler m_handler;
    GSoupLogger m_logger;
    GUPnPContext *m_context;
    Logger * const q_ptr;
    Q_DECLARE_PUBLIC(Logger)
};

#endif // LOGGER_P_H
