#include "bond.h"

Bond::Bond(QObject *parent) :
    QObject(parent),
    m_fromAtom(0),
    m_toAtom(0),
    m_order(1)
{
}

Atom *Bond::fromAtom() const
{
    return m_fromAtom;
}

void Bond::setFromAtom(Atom *fromAtom)
{
    m_fromAtom = fromAtom;
}

Atom *Bond::toAtom() const
{
    return m_toAtom;
}

void Bond::setToAtom(Atom *toAtom)
{
    m_toAtom = toAtom;
}

short Bond::order() const
{
    return m_order;
}

void Bond::setOrder(short order)
{
    m_order = order;
}
