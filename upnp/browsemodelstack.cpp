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

#include "browsemodelstack.h"

#include <QDeclarativeContext>

extern QDeclarativeContext *rootContext;

BrowseModelStack BrowseModelStack::m_instance;

BrowseModelStack::BrowseModelStack(QObject *parent)
    : QObject(parent)
    , m_stack()
{
}

BrowseModelStack &BrowseModelStack::getDefault()
{
    return BrowseModelStack::m_instance;
}

void BrowseModelStack::push(BrowseModel *model)
{
    rootContext->setContextProperty(QLatin1String("browseModel"), model);

    // clear filter when navigating away
    if (not m_stack.isEmpty()) {
        m_stack.last()->setFilterFixedString(QString());
    }
    m_stack.append(model);
}

void BrowseModelStack::pop()
{
    if (m_stack.isEmpty()) {
        return;
    }

    BrowseModel *head = m_stack.takeLast();

    if (m_stack.count() == 0) {
        rootContext->setContextProperty(QLatin1String("browseModel"), &BrowseModel::empty());
    } else {
        rootContext->setContextProperty(QLatin1String("browseModel"), m_stack.last());
    }

    // don't delete the empty model
    if (head != &BrowseModel::empty()) {
        head->deleteLater();
    }
}

void BrowseModelStack::clear()
{
    rootContext->setContextProperty(QLatin1String("browseModel"), &BrowseModel::empty());
    Q_FOREACH(BrowseModel* model, m_stack) {
        if (model != &BrowseModel::empty()) {
            model->deleteLater();
        }
    }

    m_stack.clear();
}
