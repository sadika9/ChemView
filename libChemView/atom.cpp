#include "atom.h"

Atom::Atom(QObject *parent) :
    QObject(parent),
    position_(0, 0, 0)
{
}

QString Atom::element() const
{
    return element_;
}

void Atom::setElement(const QString &element)
{
    element_ = element;
}

QVector3D Atom::position() const
{
    return position_;
}

void Atom::setPosition(const QVector3D &position)
{
    position_ = position;
}

QVector3D Atom::color() const
{
    return color_;
}

void Atom::setColor(const QVector3D &color)
{
    color_ = color;
}

