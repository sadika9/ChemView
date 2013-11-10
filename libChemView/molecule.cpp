#include "molecule.h"

Molecule::Molecule(QObject *parent) :
    QObject(parent)
{
}

QVector<Atom *> Molecule::atoms() const
{
    return atoms_;
}

void Molecule::setAtoms(const QVector<Atom *> &atoms)
{
    atoms_ = atoms;
}

QVector<Bond *> Molecule::bonds() const
{
    return bonds_;
}

void Molecule::setBonds(const QVector<Bond *> &bonds)
{
    bonds_ = bonds;
}

void Molecule::addAtom(Atom *atom)
{
    atoms_.append(atom);
}

void Molecule::addBond(Bond *bond)
{
    bonds_.append(bond);
}
