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

#ifndef SERVICEPROXYCALL_H
#define SERVICEPROXYCALL_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>

class ServiceProxyCallPrivate;
class ServiceProxy;
class ServiceProxyCall : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServiceProxyCall)
public:
    explicit ServiceProxyCall(ServiceProxy *parent,
                              const QString &action,
                              const QStringList &params,
                              const QVariantList &values);
    ~ServiceProxyCall();

    void finalize(const QStringList &params = QStringList());

    QVariant get(const QString &key) const;

    bool hasError(void) const;
    int errorCode(void) const;
    QString errorMessage(void) const;
Q_SIGNALS:
    void ready(void);

public Q_SLOTS:
    void cancel(void);
    void run(void);

protected:
    ServiceProxyCallPrivate * const d_ptr;
private:
    Q_DECLARE_PRIVATE(ServiceProxyCall)
};

#endif // SERVICEPROXYCALL_H
