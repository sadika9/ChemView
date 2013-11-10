#include "bond.h"

Bond::Bond(QObject *parent) :
    QObject(parent),
    fromAtom_(0),
    toAtom_(0),
    order_(1)
{
}

Atom *Bond::fromAtom() const
{
    return fromAtom_;
}

void Bond::setFromAtom(Atom *fromAtom)
{
    fromAtom_ = fromAtom;
}

Atom *Bond::toAtom() const
{
    return toAtom_;
}

void Bond::setToAtom(Atom *toAtom)
{
    toAtom_ = toAtom;
}

short Bond::order() const
{
    return order_;
}

void Bond::setOrder(short order)
{
    order_ = order;
}
