#include "obreader.h"

#include "molecule.h"
#include "atom.h"
#include "bond.h"

#include <openbabel/data.h>
#include <openbabel/obconversion.h>
#include <openbabel/mol.h>
#include <openbabel/atom.h>

#include <QDebug>

OBReader::OBReader(QObject *parent) :
    QObject(parent),
    m_molecule(new Molecule)
{
}

Molecule *OBReader::molecule() const
{
    return m_molecule;
}

bool OBReader::readFile(QString fileName)
{
    using namespace OpenBabel;

    OBConversion conv;

    OBFormat *format = conv.FormatFromExt(fileName.toStdString());

    if (!format || !conv.SetInFormat(format))
    {
        qDebug() << "format error.";
        return false;
    }

    std::ifstream ifs;
    ifs.open(fileName.toStdString());

    if (!ifs)
    {
        qDebug() << "cannot open the file open.";
        return false;
    }

    OBMol obMol;
    if (!conv.Read(&obMol, &ifs))
    {
        qDebug() << "conv.Read() error.";
        return false;
    }

    if (!toMolecule(&obMol))
        return false;

    return true;
}

bool OBReader::toMolecule(OpenBabel::OBMol *obMol)
{
    using namespace OpenBabel;

    if (!obMol)
        return false;

    QMap<unsigned int, Atom *> atomMap; // need this to construct bond relations

    OBElementTable eleTable;

    FOR_ATOMS_OF_MOL(obAtom, obMol)
    {
        unsigned int atomicNum = obAtom->GetAtomicNum();

        Atom *atom = new Atom;
        atom->setPosition(QVector3D(obAtom->x(), obAtom->y(), obAtom->z()));
        atom->setElement(eleTable.GetSymbol(atomicNum));

        std::vector<double> rgb = eleTable.GetRGB(atomicNum);
        atom->setColor(QVector3D(rgb.at(0), rgb.at(1), rgb.at(2)));

        atom->setRadius(eleTable.GetCovalentRad(atomicNum) * 0.5);

        atomMap[obAtom->GetIdx()] = atom;
        m_molecule->addAtom(atom);
    }

    FOR_BONDS_OF_MOL(obBond, obMol)
    {
        Bond *bond = new Bond;

        bond->setOrder(obBond->GetBondOrder());

        bond->setFromAtom(atomMap.value(obBond->GetBeginAtomIdx()));
        bond->setToAtom(atomMap.value(obBond->GetEndAtomIdx()));

        m_molecule->addBond(bond);
    }

    return true;
}

