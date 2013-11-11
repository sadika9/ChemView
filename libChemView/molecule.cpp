#include "molecule.h"

Molecule::Molecule(QObject *parent) :
    QObject(parent)
{
}

QVector<Atom *> Molecule::atoms() const
{
    return m_atoms;
}

void Molecule::setAtoms(const QVector<Atom *> &atoms)
{
    m_atoms = atoms;
}

QVector<Bond *> Molecule::bonds() const
{
    return m_bonds;
}

void Molecule::setBonds(const QVector<Bond *> &bonds)
{
    m_bonds = bonds;
}

void Molecule::addAtom(Atom *atom)
{
    m_atoms.append(atom);
}

void Molecule::addBond(Bond *bond)
{
    m_bonds.append(bond);
}
