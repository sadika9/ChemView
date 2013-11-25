#ifndef CMLREADER_H
#define CMLREADER_H

#include <QObject>
#include <QXmlStreamReader>
#include <QVector3D>
#include <QMap>

class QIODevice;
class Molecule;
class Atom;

/**
 * @brief The CmlReader class
 * @note This class needs serious restructring!!!
 */
class CmlReader : public QObject
{
    Q_OBJECT
public:
    explicit CmlReader(QObject *parent = 0);
    explicit CmlReader(QIODevice * device, QObject *parent = 0);

    ~CmlReader();

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    Molecule *molecule() const;

    /**
     * @brief Parse CML file
     * @return true if no error occured during parsing.
     */
    bool parse();


private:
    void initAtomData();
    bool parseAtom(QXmlStreamReader &xml);
    bool parseBond(QXmlStreamReader &xml);

    QVector3D atomColor(QString elementType);
    float atomRadius(QString elementType);


    QIODevice *m_device;
    Molecule *m_molecule;

    QMap<QString, Atom *> m_atomMap;

    struct BondStruct
    {
        QString fromAtomId;
        QString toAtomId;
        short order;
    };
    QVector<BondStruct> m_bonds;
public:
    // key: element type, value: pair of radius and color in rgb
    QMap<QString, QPair<float, QVector3D>> m_atomData;
};

#endif // CMLREADER_H
