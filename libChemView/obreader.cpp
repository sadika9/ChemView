#include "obreader.h"

#include "molecule.h"
#include "atom.h"
#include "bond.h"

#include <openbabel/data.h>
#include <openbabel/obconversion.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/mol.h>
#include <openbabel/atom.h>

#include <QtWidgets/QMessageBox>
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
        qDebug() << "Unsupported File Format.";
        return false;
    }

    std::ifstream ifs;
    ifs.open(fileName.toStdString());

    if (!ifs)
    {
        qDebug() << "Could not open the file.";
        return false;
    }

    OBMol obMol;
    if (!conv.Read(&obMol, &ifs))
    {
        qDebug() << "Error occured while reading the file.";
        return false;
    }

    if (!obMol.Has3D())
    {
        QMessageBox::information(0,
                                 tr("OBReader"),
                                 tr("No 3D coordinate values present in this file.\n"
                                    "OBReader will generate the rough molecular geometry."));

        if (!buildGeometry(&obMol))
        {
            qDebug() << "Error in buildGeometry()";
            return false;
        }
    }

    if (!toMolecule(&obMol))
    {
        qDebug() << "Could not convert OBMol to Molecule.";
        return false;
    }

    return true;
}

bool OBReader::readSmiString(QString smiString)
{
    using namespace OpenBabel;

    OBConversion conv;

    if (!conv.SetInFormat("smi"))
    {
        qDebug() << "Error in conv.SetInFormat().";
        return false;
    }

    OBMol obMol;

    if (!conv.ReadString(&obMol, smiString.toStdString()))
    {
        qDebug() << "Error occured while reading the smi string.";
        return false;
    }

    if (!obMol.Has3D())
    {
        if (!buildGeometry(&obMol))
        {
            qDebug() << "Error in buildGeometry()";
            return false;
        }
    }

    if (!toMolecule(&obMol))
    {
        qDebug() << "Could not convert OBMol to Molecule.";
        return false;
    }


    return true;
}

bool OBReader::toMolecule(OpenBabel::OBMol *obMol)
{
    using namespace OpenBabel;

    if (!obMol)
    {
        qDebug() << Q_FUNC_INFO << "was called with null parameter.";
        return false;
    }

    m_molecule->setFormula(obMol->GetSpacedFormula().c_str());
    m_molecule->setTitle(obMol->GetTitle());
    m_molecule->setWeight(obMol->GetMolWt());

    OBElementTable elementTable; // to get atom data.
    QMap<unsigned int, Atom *> atomMap; // need this to construct bond relations

    FOR_ATOMS_OF_MOL(obAtom, obMol)
    {
        unsigned int atomicNum = obAtom->GetAtomicNum();

        Atom *atom = new Atom;

        atom->setPosition(QVector3D(obAtom->x(), obAtom->y(), obAtom->z()));
        atom->setElement(elementTable.GetSymbol(atomicNum));
        atom->setRadius(elementTable.GetCovalentRad(atomicNum));

        std::vector<double> rgb = elementTable.GetRGB(atomicNum);
        atom->setColor(QVector3D(rgb.at(0), rgb.at(1), rgb.at(2)));

        atomMap[obAtom->GetIdx()] = atom; // add atom to map. use to set bond's from and to atoms.
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

bool OBReader::buildGeometry(OpenBabel::OBMol *obMol)
{
    /**
     *  This function based on idea from Avogadro project.
     *  http://avogadro.openmolecules.net/wiki/Main_Page
     */

    using namespace OpenBabel;

    bool state = false; // function return state.

    OBBuilder builder;

    state = builder.Build(*obMol);
    state &= obMol->AddHydrogens();


    /** Code From
     *  http://openbabel.org/api/2.3.0/classOpenBabel_1_1OBForceField.shtml
     */
    // Select the forcefield, this returns a pointer that we
    // will later use to access the forcefield functions.
    OBForceField *pFF = OBForceField::FindForceField("MMFF94");

    // Make sure we have a valid pointer
    if (!pFF)
    {
        state = false;
        return state;
    }

    // We need to setup the forcefield before we can use it. Setup()
    // returns false if it failes to find the atom types, parameters, ...
    if (!pFF->Setup(*obMol))
    {
        qDebug() << "ERROR: could not setup force field.";
        state = false;
        return state;
    }

    // Perform the actual minimization, maximum 1000 steps
    pFF->ConjugateGradients(1000);

    // Get new coordinates
    state &= pFF->GetCoordinates(*obMol);

    return state;
}

