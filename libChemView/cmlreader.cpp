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
    initAtomData();
}

CmlReader::CmlReader(QIODevice *device, QObject *parent) :
    QObject(parent),
    m_device(device),
    m_molecule(new Molecule)
{
    initAtomData();
}

CmlReader::~CmlReader()
{
    delete m_molecule;
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
    QStringList refs = str.split(" ", QString::SkipEmptyParts);

    if (refs.size() != 2)
        return false;

    BondStruct b;
    b.fromAtomId = refs.at(0);
    b.toAtomId = refs.at(1);
    b.order = attributes.value("order").toShort();

    m_bonds.append(b);

    return true;
}

inline QVector3D CmlReader::atomColor(QString elementType)
{
    return m_atomData.value(elementType, qMakePair(0.3f, QVector3D(255, 192, 203))).second;
}

float CmlReader::atomRadius(QString elementType)
{
    return m_atomData.value(elementType, qMakePair(0.3f, QVector3D(255, 192, 203))).first;
}

void CmlReader::initAtomData()
{
    // key: element type, value: pair of radius and color in rgb
    // Color codes from http://jmol.sourceforge.net/jscolors/
    // Default size is 0.3 change it appropriately

    m_atomData["H"] = qMakePair(0.25f, QVector3D(255,255,255));
    m_atomData["He"] = qMakePair(0.3f, QVector3D(217,255,255));
    m_atomData["Li"] = qMakePair(0.3f, QVector3D(204,128,255));
    m_atomData["Be"] = qMakePair(0.3f, QVector3D(194,255,0));
    m_atomData["B"] = qMakePair(0.3f, QVector3D(255,181,181));
    m_atomData["C"] = qMakePair(0.5f, QVector3D(144,144,144));
    m_atomData["N"] = qMakePair(0.3f, QVector3D(48,80,248));
    m_atomData["O"] = qMakePair(0.4f, QVector3D(255,13,13));
    m_atomData["F"] = qMakePair(0.3f, QVector3D(144,224,80));
    m_atomData["Ne"] = qMakePair(0.3f, QVector3D(179,227,245));
    m_atomData["Na"] = qMakePair(0.3f, QVector3D(171,92,242));
    m_atomData["Mg"] = qMakePair(0.3f, QVector3D(138,255,0));
    m_atomData["Al"] = qMakePair(0.3f, QVector3D(191,166,166));
    m_atomData["Si"] = qMakePair(0.3f, QVector3D(240,200,160));
    m_atomData["P"] = qMakePair(0.3f, QVector3D(255,128,0));
    m_atomData["S"] = qMakePair(0.3f, QVector3D(255,255,48));
    m_atomData["Cl"] = qMakePair(0.3f, QVector3D(31,240,31));
    m_atomData["Ar"] = qMakePair(0.3f, QVector3D(128,209,227));
    m_atomData["K"] = qMakePair(0.3f, QVector3D(143,64,212));
    m_atomData["Ca"] = qMakePair(0.3f, QVector3D(61,255,0));
    m_atomData["Sc"] = qMakePair(0.3f, QVector3D(230,230,230));
    m_atomData["Ti"] = qMakePair(0.3f, QVector3D(191,194,199));
    m_atomData["V"] = qMakePair(0.3f, QVector3D(166,166,171));
    m_atomData["Cr"] = qMakePair(0.3f, QVector3D(138,153,199));
    m_atomData["Mn"] = qMakePair(0.3f, QVector3D(156,122,199));
    m_atomData["Fe"] = qMakePair(0.3f, QVector3D(224,102,51));
    m_atomData["Co"] = qMakePair(0.3f, QVector3D(240,144,160));
    m_atomData["Ni"] = qMakePair(0.3f, QVector3D(80,208,80));
    m_atomData["Cu"] = qMakePair(0.3f, QVector3D(200,128,51));
    m_atomData["Zn"] = qMakePair(0.3f, QVector3D(125,128,176));
    m_atomData["Ga"] = qMakePair(0.3f, QVector3D(194,143,143));
    m_atomData["Ge"] = qMakePair(0.3f, QVector3D(102,143,143));
    m_atomData["As"] = qMakePair(0.3f, QVector3D(189,128,227));
    m_atomData["Se"] = qMakePair(0.3f, QVector3D(255,161,0));
    m_atomData["Br"] = qMakePair(0.3f, QVector3D(166,41,41));
    m_atomData["Kr"] = qMakePair(0.3f, QVector3D(92,184,209));
    m_atomData["Rb"] = qMakePair(0.3f, QVector3D(112,46,176));
    m_atomData["Sr"] = qMakePair(0.3f, QVector3D(0,255,0));
    m_atomData["Y"] = qMakePair(0.3f, QVector3D(148,255,255));
    m_atomData["Zr"] = qMakePair(0.3f, QVector3D(148,224,224));
    m_atomData["Nb"] = qMakePair(0.3f, QVector3D(115,194,201));
    m_atomData["Mo"] = qMakePair(0.3f, QVector3D(84,181,181));
    m_atomData["Tc"] = qMakePair(0.3f, QVector3D(59,158,158));
    m_atomData["Ru"] = qMakePair(0.3f, QVector3D(36,143,143));
    m_atomData["Rh"] = qMakePair(0.3f, QVector3D(10,125,140));
    m_atomData["Pd"] = qMakePair(0.3f, QVector3D(0,105,133));
    m_atomData["Ag"] = qMakePair(0.3f, QVector3D(192,192,192));
    m_atomData["Cd"] = qMakePair(0.3f, QVector3D(255,217,143));
    m_atomData["In"] = qMakePair(0.3f, QVector3D(166,117,115));
    m_atomData["Sn"] = qMakePair(0.3f, QVector3D(102,128,128));
    m_atomData["Sb"] = qMakePair(0.3f, QVector3D(158,99,181));
    m_atomData["Te"] = qMakePair(0.3f, QVector3D(212,122,0));
    m_atomData["I"] = qMakePair(0.3f, QVector3D(148,0,148));
    m_atomData["Xe"] = qMakePair(0.3f, QVector3D(66,158,176));
    m_atomData["Cs"] = qMakePair(0.3f, QVector3D(87,23,143));
    m_atomData["Ba"] = qMakePair(0.3f, QVector3D(0,201,0));
    m_atomData["La"] = qMakePair(0.3f, QVector3D(112,212,255));
    m_atomData["Ce"] = qMakePair(0.3f, QVector3D(255,255,199));
    m_atomData["Pr"] = qMakePair(0.3f, QVector3D(217,255,199));
    m_atomData["Nd"] = qMakePair(0.3f, QVector3D(199,255,199));
    m_atomData["Pm"] = qMakePair(0.3f, QVector3D(163,255,199));
    m_atomData["Sm"] = qMakePair(0.3f, QVector3D(143,255,199));
    m_atomData["Eu"] = qMakePair(0.3f, QVector3D(97,255,199));
    m_atomData["Gd"] = qMakePair(0.3f, QVector3D(69,255,199));
    m_atomData["Tb"] = qMakePair(0.3f, QVector3D(48,255,199));
    m_atomData["Dy"] = qMakePair(0.3f, QVector3D(31,255,199));
    m_atomData["Ho"] = qMakePair(0.3f, QVector3D(0,255,156));
    m_atomData["Er"] = qMakePair(0.3f, QVector3D(0,230,117));
    m_atomData["Tm"] = qMakePair(0.3f, QVector3D(0,212,82));
    m_atomData["Yb"] = qMakePair(0.3f, QVector3D(0,191,56));
    m_atomData["Lu"] = qMakePair(0.3f, QVector3D(0,171,36));
    m_atomData["Hf"] = qMakePair(0.3f, QVector3D(77,194,255));
    m_atomData["Ta"] = qMakePair(0.3f, QVector3D(77,166,255));
    m_atomData["W"] = qMakePair(0.3f, QVector3D(33,148,214));
    m_atomData["Re"] = qMakePair(0.3f, QVector3D(38,125,171));
    m_atomData["Os"] = qMakePair(0.3f, QVector3D(38,102,150));
    m_atomData["Ir"] = qMakePair(0.3f, QVector3D(23,84,135));
    m_atomData["Pt"] = qMakePair(0.3f, QVector3D(208,208,224));
    m_atomData["Au"] = qMakePair(0.3f, QVector3D(255,209,35));
    m_atomData["Hg"] = qMakePair(0.3f, QVector3D(184,184,208));
    m_atomData["Tl"] = qMakePair(0.3f, QVector3D(166,84,77));
    m_atomData["Pb"] = qMakePair(0.3f, QVector3D(87,89,97));
    m_atomData["Bi"] = qMakePair(0.3f, QVector3D(158,79,181));
    m_atomData["Po"] = qMakePair(0.3f, QVector3D(171,92,0));
    m_atomData["At"] = qMakePair(0.3f, QVector3D(117,79,69));
    m_atomData["Rn"] = qMakePair(0.3f, QVector3D(66,130,150));
    m_atomData["Fr"] = qMakePair(0.3f, QVector3D(66,0,102));
    m_atomData["Ra"] = qMakePair(0.3f, QVector3D(0,125,0));
    m_atomData["Ac"] = qMakePair(0.3f, QVector3D(112,171,250));
    m_atomData["Th"] = qMakePair(0.3f, QVector3D(0,186,255));
    m_atomData["Pa"] = qMakePair(0.3f, QVector3D(0,161,255));
    m_atomData["U"] = qMakePair(0.3f, QVector3D(0,143,255));
    m_atomData["Np"] = qMakePair(0.3f, QVector3D(0,128,255));
    m_atomData["Pu"] = qMakePair(0.3f, QVector3D(0,107,255));
    m_atomData["Am"] = qMakePair(0.3f, QVector3D(84,92,242));
    m_atomData["Cm"] = qMakePair(0.3f, QVector3D(120,92,227));
    m_atomData["Bk"] = qMakePair(0.3f, QVector3D(138,79,227));
    m_atomData["Cf"] = qMakePair(0.3f, QVector3D(161,54,212));
    m_atomData["Es"] = qMakePair(0.3f, QVector3D(179,31,212));
    m_atomData["Fm"] = qMakePair(0.3f, QVector3D(179,31,186));
    m_atomData["Md"] = qMakePair(0.3f, QVector3D(179,13,166));
    m_atomData["No"] = qMakePair(0.3f, QVector3D(189,13,135));
    m_atomData["Lr"] = qMakePair(0.3f, QVector3D(199,0,102));
    m_atomData["Rf"] = qMakePair(0.3f, QVector3D(204,0,89));
    m_atomData["Db"] = qMakePair(0.3f, QVector3D(209,0,79));
    m_atomData["Sg"] = qMakePair(0.3f, QVector3D(217,0,69));
    m_atomData["Bh"] = qMakePair(0.3f, QVector3D(224,0,56));
    m_atomData["Hs"] = qMakePair(0.3f, QVector3D(230,0,46));
    m_atomData["Mt"] = qMakePair(0.3f, QVector3D(235,0,38));
}
