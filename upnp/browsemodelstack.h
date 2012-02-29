#ifndef BROWSEMODELSTACK_H
#define BROWSEMODELSTACK_H

#include <QObject>
#include "browsemodel.h"

class BrowseModelStack : public QObject
{
    Q_OBJECT
public:
    static BrowseModelStack &getDefault();
    void push(BrowseModel *model);

Q_SIGNALS:

public Q_SLOTS:
    void pop();
    bool empty() { return m_stack.isEmpty(); }

private:
    static BrowseModelStack m_instance;
    explicit BrowseModelStack(QObject *parent = 0);
    QList<BrowseModel*> m_stack;
};

#endif // BROWSEMODELSTACK_H
