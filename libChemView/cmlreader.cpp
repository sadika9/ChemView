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
    //    delete m_molecule;
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

    m_atomData["Ac"] = qMakePair(0.3f, QVector3D(0.439216f, 0.670588f, 0.980392f));
    m_atomData["Ag"] = qMakePair(0.3f, QVector3D(0.752941f, 0.752941f, 0.752941f));
    m_atomData["Al"] = qMakePair(0.3f, QVector3D(0.74902f, 0.65098f, 0.65098f));
    m_atomData["Am"] = qMakePair(0.3f, QVector3D(0.329412f, 0.360784f, 0.94902f));
    m_atomData["Ar"] = qMakePair(0.3f, QVector3D(0.501961f, 0.819608f, 0.890196f));
    m_atomData["As"] = qMakePair(0.3f, QVector3D(0.741176f, 0.501961f, 0.890196f));
    m_atomData["At"] = qMakePair(0.3f, QVector3D(0.458824f, 0.309804f, 0.270588f));
    m_atomData["Au"] = qMakePair(0.3f, QVector3D(1.0f, 0.819608f, 0.137255f));
    m_atomData["B"] = qMakePair(0.3f, QVector3D(1.0f, 0.709804f, 0.709804f));
    m_atomData["Ba"] = qMakePair(0.3f, QVector3D(0.0f, 0.788235f, 0.0f));
    m_atomData["Be"] = qMakePair(0.3f, QVector3D(0.760784f, 1.0f, 0.0f));
    m_atomData["Bh"] = qMakePair(0.3f, QVector3D(0.878431f, 0.0f, 0.219608f));
    m_atomData["Bi"] = qMakePair(0.3f, QVector3D(0.619608f, 0.309804f, 0.709804f));
    m_atomData["Bk"] = qMakePair(0.3f, QVector3D(0.541176f, 0.309804f, 0.890196f));
    m_atomData["Br"] = qMakePair(0.3f, QVector3D(0.65098f, 0.160784f, 0.160784f));
    m_atomData["C"] = qMakePair(0.5f, QVector3D(0.564706f, 0.564706f, 0.564706f));
    m_atomData["Ca"] = qMakePair(0.3f, QVector3D(0.239216f, 1.0f, 0.0f));
    m_atomData["Cd"] = qMakePair(0.3f, QVector3D(1.0f, 0.85098f, 0.560784f));
    m_atomData["Ce"] = qMakePair(0.3f, QVector3D(1.0f, 1.0f, 0.780392f));
    m_atomData["Cf"] = qMakePair(0.3f, QVector3D(0.631373f, 0.211765f, 0.831373f));
    m_atomData["Cl"] = qMakePair(0.3f, QVector3D(0.121569f, 0.941176f, 0.121569f));
    m_atomData["Cm"] = qMakePair(0.3f, QVector3D(0.470588f, 0.360784f, 0.890196f));
    m_atomData["Co"] = qMakePair(0.3f, QVector3D(0.941176f, 0.564706f, 0.627451f));
    m_atomData["Cr"] = qMakePair(0.3f, QVector3D(0.541176f, 0.6f, 0.780392f));
    m_atomData["Cs"] = qMakePair(0.3f, QVector3D(0.341176f, 0.0901961f, 0.560784f));
    m_atomData["Cu"] = qMakePair(0.3f, QVector3D(0.784314f, 0.501961f, 0.2f));
    m_atomData["Db"] = qMakePair(0.3f, QVector3D(0.819608f, 0.0f, 0.309804f));
    m_atomData["Dy"] = qMakePair(0.3f, QVector3D(0.121569f, 1.0f, 0.780392f));
    m_atomData["Er"] = qMakePair(0.3f, QVector3D(0.0f, 0.901961f, 0.458824f));
    m_atomData["Es"] = qMakePair(0.3f, QVector3D(0.701961f, 0.121569f, 0.831373f));
    m_atomData["Eu"] = qMakePair(0.3f, QVector3D(0.380392f, 1.0f, 0.780392f));
    m_atomData["F"] = qMakePair(0.3f, QVector3D(0.564706f, 0.878431f, 0.313726f));
    m_atomData["Fe"] = qMakePair(0.3f, QVector3D(0.878431f, 0.4f, 0.2f));
    m_atomData["Fm"] = qMakePair(0.3f, QVector3D(0.701961f, 0.121569f, 0.729412f));
    m_atomData["Fr"] = qMakePair(0.3f, QVector3D(0.258824f, 0.0f, 0.4f));
    m_atomData["Ga"] = qMakePair(0.3f, QVector3D(0.760784f, 0.560784f, 0.560784f));
    m_atomData["Gd"] = qMakePair(0.3f, QVector3D(0.270588f, 1.0f, 0.780392f));
    m_atomData["Ge"] = qMakePair(0.3f, QVector3D(0.4f, 0.560784f, 0.560784f));
    m_atomData["H"] = qMakePair(0.25f, QVector3D(1.0f, 1.0f, 1.0f));
    m_atomData["He"] = qMakePair(0.3f, QVector3D(0.85098f, 1.0f, 1.0f));
    m_atomData["Hf"] = qMakePair(0.3f, QVector3D(0.301961f, 0.760784f, 1.0f));
    m_atomData["Hg"] = qMakePair(0.3f, QVector3D(0.721569f, 0.721569f, 0.815686f));
    m_atomData["Ho"] = qMakePair(0.3f, QVector3D(0.0f, 1.0f, 0.611765f));
    m_atomData["Hs"] = qMakePair(0.3f, QVector3D(0.901961f, 0.0f, 0.180392f));
    m_atomData["I"] = qMakePair(0.3f, QVector3D(0.580392f, 0.0f, 0.580392f));
    m_atomData["In"] = qMakePair(0.3f, QVector3D(0.65098f, 0.458824f, 0.45098f));
    m_atomData["Ir"] = qMakePair(0.3f, QVector3D(0.0901961f, 0.329412f, 0.529412f));
    m_atomData["K"] = qMakePair(0.3f, QVector3D(0.560784f, 0.25098f, 0.831373f));
    m_atomData["Kr"] = qMakePair(0.3f, QVector3D(0.360784f, 0.721569f, 0.819608f));
    m_atomData["La"] = qMakePair(0.3f, QVector3D(0.439216f, 0.831373f, 1.0f));
    m_atomData["Li"] = qMakePair(0.3f, QVector3D(0.8f, 0.501961f, 1.0f));
    m_atomData["Lr"] = qMakePair(0.3f, QVector3D(0.780392f, 0.0f, 0.4f));
    m_atomData["Lu"] = qMakePair(0.3f, QVector3D(0.0f, 0.670588f, 0.141176f));
    m_atomData["Md"] = qMakePair(0.3f, QVector3D(0.701961f, 0.0509804f, 0.65098f));
    m_atomData["Mg"] = qMakePair(0.3f, QVector3D(0.541176f, 1.0f, 0.0f));
    m_atomData["Mn"] = qMakePair(0.3f, QVector3D(0.611765f, 0.478431f, 0.780392f));
    m_atomData["Mo"] = qMakePair(0.3f, QVector3D(0.329412f, 0.709804f, 0.709804f));
    m_atomData["Mt"] = qMakePair(0.3f, QVector3D(0.921569f, 0.0f, 0.14902f));
    m_atomData["N"] = qMakePair(0.3f, QVector3D(0.188235f, 0.313726f, 0.972549f));
    m_atomData["Na"] = qMakePair(0.3f, QVector3D(0.670588f, 0.360784f, 0.94902f));
    m_atomData["Nb"] = qMakePair(0.3f, QVector3D(0.45098f, 0.760784f, 0.788235f));
    m_atomData["Nd"] = qMakePair(0.3f, QVector3D(0.780392f, 1.0f, 0.780392f));
    m_atomData["Ne"] = qMakePair(0.3f, QVector3D(0.701961f, 0.890196f, 0.960784f));
    m_atomData["Ni"] = qMakePair(0.3f, QVector3D(0.313726f, 0.815686f, 0.313726f));
    m_atomData["No"] = qMakePair(0.3f, QVector3D(0.741176f, 0.0509804f, 0.529412f));
    m_atomData["Np"] = qMakePair(0.3f, QVector3D(0.0f, 0.501961f, 1.0f));
    m_atomData["O"] = qMakePair(0.4f, QVector3D(1.0f, 0.0509804f, 0.0509804f));
    m_atomData["Os"] = qMakePair(0.3f, QVector3D(0.14902f, 0.4f, 0.588235f));
    m_atomData["P"] = qMakePair(0.3f, QVector3D(1.0f, 0.501961f, 0.0f));
    m_atomData["Pa"] = qMakePair(0.3f, QVector3D(0.0f, 0.631373f, 1.0f));
    m_atomData["Pb"] = qMakePair(0.3f, QVector3D(0.341176f, 0.34902f, 0.380392f));
    m_atomData["Pd"] = qMakePair(0.3f, QVector3D(0.0f, 0.411765f, 0.521569f));
    m_atomData["Pm"] = qMakePair(0.3f, QVector3D(0.639216f, 1.0f, 0.780392f));
    m_atomData["Po"] = qMakePair(0.3f, QVector3D(0.670588f, 0.360784f, 0.0f));
    m_atomData["Pr"] = qMakePair(0.3f, QVector3D(0.85098f, 1.0f, 0.780392f));
    m_atomData["Pt"] = qMakePair(0.3f, QVector3D(0.815686f, 0.815686f, 0.878431f));
    m_atomData["Pu"] = qMakePair(0.3f, QVector3D(0.0f, 0.419608f, 1.0f));
    m_atomData["Ra"] = qMakePair(0.3f, QVector3D(0.0f, 0.490196f, 0.0f));
    m_atomData["Rb"] = qMakePair(0.3f, QVector3D(0.439216f, 0.180392f, 0.690196f));
    m_atomData["Re"] = qMakePair(0.3f, QVector3D(0.14902f, 0.490196f, 0.670588f));
    m_atomData["Rf"] = qMakePair(0.3f, QVector3D(0.8f, 0.0f, 0.34902f));
    m_atomData["Rh"] = qMakePair(0.3f, QVector3D(0.0392157f, 0.490196f, 0.54902f));
    m_atomData["Rn"] = qMakePair(0.3f, QVector3D(0.258824f, 0.509804f, 0.588235f));
    m_atomData["Ru"] = qMakePair(0.3f, QVector3D(0.141176f, 0.560784f, 0.560784f));
    m_atomData["S"] = qMakePair(0.3f, QVector3D(1.0f, 1.0f, 0.188235f));
    m_atomData["Sb"] = qMakePair(0.3f, QVector3D(0.619608f, 0.388235f, 0.709804f));
    m_atomData["Sc"] = qMakePair(0.3f, QVector3D(0.901961f, 0.901961f, 0.901961f));
    m_atomData["Se"] = qMakePair(0.3f, QVector3D(1.0f, 0.631373f, 0.0f));
    m_atomData["Sg"] = qMakePair(0.3f, QVector3D(0.85098f, 0.0f, 0.270588f));
    m_atomData["Si"] = qMakePair(0.3f, QVector3D(0.941176f, 0.784314f, 0.627451f));
    m_atomData["Sm"] = qMakePair(0.3f, QVector3D(0.560784f, 1.0f, 0.780392f));
    m_atomData["Sn"] = qMakePair(0.3f, QVector3D(0.4f, 0.501961f, 0.501961f));
    m_atomData["Sr"] = qMakePair(0.3f, QVector3D(0.0f, 1.0f, 0.0f));
    m_atomData["Ta"] = qMakePair(0.3f, QVector3D(0.301961f, 0.65098f, 1.0f));
    m_atomData["Tb"] = qMakePair(0.3f, QVector3D(0.188235f, 1.0f, 0.780392f));
    m_atomData["Tc"] = qMakePair(0.3f, QVector3D(0.231373f, 0.619608f, 0.619608f));
    m_atomData["Te"] = qMakePair(0.3f, QVector3D(0.831373f, 0.478431f, 0.0f));
    m_atomData["Th"] = qMakePair(0.3f, QVector3D(0.0f, 0.729412f, 1.0f));
    m_atomData["Ti"] = qMakePair(0.3f, QVector3D(0.74902f, 0.760784f, 0.780392f));
    m_atomData["Tl"] = qMakePair(0.3f, QVector3D(0.65098f, 0.329412f, 0.301961f));
    m_atomData["Tm"] = qMakePair(0.3f, QVector3D(0.0f, 0.831373f, 0.321569f));
    m_atomData["U"] = qMakePair(0.3f, QVector3D(0.0f, 0.560784f, 1.0f));
    m_atomData["V"] = qMakePair(0.3f, QVector3D(0.65098f, 0.65098f, 0.670588f));
    m_atomData["W"] = qMakePair(0.3f, QVector3D(0.129412f, 0.580392f, 0.839216f));
    m_atomData["Xe"] = qMakePair(0.3f, QVector3D(0.258824f, 0.619608f, 0.690196f));
    m_atomData["Y"] = qMakePair(0.3f, QVector3D(0.580392f, 1.0f, 1.0f));
    m_atomData["Yb"] = qMakePair(0.3f, QVector3D(0.0f, 0.74902f, 0.219608f));
    m_atomData["Zn"] = qMakePair(0.3f, QVector3D(0.490196f, 0.501961f, 0.690196f));
    m_atomData["Zr"] = qMakePair(0.3f, QVector3D(0.580392f, 0.878431f, 0.878431f));
}
