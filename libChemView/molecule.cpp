#include "molecule.h"

Molecule::Molecule(QObject *parent) :
    QObject(parent),
    m_weight(0)
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

QString Molecule::formula() const
{
    return m_formula;
}

void Molecule::setFormula(const QString &formula)
{
    m_formula = formula;
}

QString Molecule::title() const
{
    return m_title;
}

void Molecule::setTitle(const QString &title)
{
    m_title = title;
}

double Molecule::weight() const
{
    return m_weight;
}

void Molecule::setWeight(double weight)
{
    m_weight = weight;
}
