#include "cmlreader.h"
#include "molecule.h"
#include "atom.h"
#include "bond.h"

#include <QStringList>
#include <QDebug>

CmlReader::CmlReader(QObject *parent) :
    QObject(parent),
    m_device(nullptr),
    m_molecule(new Molecule)
{
}

CmlReader::CmlReader(QIODevice *device, QObject *parent) :
    QObject(parent),
    m_device(device),
    m_molecule(new Molecule)
{

}

QIODevice *CmlReader::device() const
{
    return m_device;
}

void CmlReader::setDevice(QIODevice *device)
{
    m_device = device;
}

Molecule *CmlReader::molecule() const
{
    return m_molecule;
}

bool CmlReader::parse()
{
    QXmlStreamReader xml(m_device);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument)
            continue;

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "atomArray")
                continue;

            if (xml.name() == "atom")
                parseAtom(xml);


            if (xml.name() == "bondArray")
                continue;

            if (xml.name() == "bond")
                parseBond(xml);
        }
    }

    if (xml.hasError())
    {
        qDebug() << xml.errorString();

        return false;
    }


    for (int i = 0; i < m_bonds.size(); ++i)
    {
        BondStruct bs = m_bonds.at(i);

        Bond *bond = new Bond;
        bond->setFromAtom(m_atomMap.value(bs.fromAtomId));
        bond->setToAtom(m_atomMap.value(bs.toAtomId));
        bond->setOrder(bs.order);

        m_molecule->addBond(bond);
    }

    return true;
}

bool CmlReader::parseAtom(QXmlStreamReader &xml)
{
    if (!xml.isStartElement() && xml.name() == "atom")
        return false;

    QXmlStreamAttributes attributes = xml.attributes();

    Atom *atom = new Atom;

    if (!attributes.hasAttribute("elementType") ||
            !attributes.hasAttribute("x3") ||
            !attributes.hasAttribute("y3") ||
            !attributes.hasAttribute("z3"))
    {
        return false;
    }

    QString elementType = attributes.value("elementType").toString();
    atom->setElement(elementType);
    atom->setColor(atomColor(elementType));
    atom->setRadius(atomRadius(elementType));

    float x = attributes.value("x3").toFloat();
    float y = attributes.value("y3").toFloat();
    float z = attributes.value("z3").toFloat();

    atom->setPosition(QVector3D(x, y, z));

    m_atomMap[attributes.value("id").toString()] = atom;
    m_molecule->addAtom(atom);

    return true;
}

bool CmlReader::parseBond(QXmlStreamReader &xml)
{
    if (!xml.isStartElement() && xml.name() == "bond")
        return false;


    QXmlStreamAttributes attributes = xml.attributes();

    if (!attributes.hasAttribute("atomRefs2") || !attributes.hasAttribute("order"))
        return false;


    QString str = attributes.value("atomRefs2").toString();
    QStringList refs = str.split(" ");

    if (refs.size() != 2)
        return false;

    BondStruct b;
    b.fromAtomId = refs.at(0);
    b.toAtomId = refs.at(1);
    b.order = attributes.value("order").toShort();

    m_bonds.append(b);

    return true;
}

QVector3D CmlReader::atomColor(QString elementType)
{
    if (QString("H").compare(elementType) == 0)
        return QVector3D(1.0f, 1.0f, 1.0f);             // white
    else if (QString("C").compare(elementType) == 0)
        return QVector3D(0, 0, 0);                      // black
    else if (QString("O").compare(elementType) == 0)
        return QVector3D(1.0f, 0.0f, 0.0f);             // red
    else
        return QVector3D(0.98039216f, 0.68627451f, 0.74509804f);   // pink
}

float CmlReader::atomRadius(QString elementType)
{
    if (QString("H").compare(elementType) == 0)
        return 0.25;
    else if (QString("C").compare(elementType) == 0)
        return 0.5;
    else if (QString("O").compare(elementType) == 0)
        return 0.4;
    else
        return 0.3;
}
