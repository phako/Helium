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
    m_stack.append(model);
    rootContext->setContextProperty("browseModel", model);
}

void BrowseModelStack::pop()
{
    if (m_stack.isEmpty()) {
        return;
    }

    if (m_stack.count() == 1) {
        rootContext->setContextProperty("browseModel", 0);
    } else {
        rootContext->setContextProperty("browseModel", m_stack.at(m_stack.count() - 2));
    }

    delete m_stack.takeLast();
}
