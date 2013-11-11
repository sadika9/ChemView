#include "atom.h"

Atom::Atom(QObject *parent) :
    QObject(parent),
    m_position(0, 0, 0)
{
}

QString Atom::element() const
{
    return m_element;
}

void Atom::setElement(const QString &element)
{
    m_element = element;
}

QVector3D Atom::position() const
{
    return m_position;
}

void Atom::setPosition(const QVector3D &position)
{
    m_position = position;
}

QVector3D Atom::color() const
{
    return m_color;
}

void Atom::setColor(const QVector3D &color)
{
    m_color = color;
}

