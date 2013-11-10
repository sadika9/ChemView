#ifndef CMLREADER_H
#define CMLREADER_H

#include <QObject>
#include <QXmlStreamReader>
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

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    Molecule *molecule() const;

    /**
     * @brief Parse CML file
     * @return true if no error occured during parsing.
     */
    bool parse();


private:
    bool parseAtom(QXmlStreamReader &xml);
    bool parseBond(QXmlStreamReader &xml);

    QVector3D atomColor(QString elementType);


    QIODevice *device_;
    Molecule *molecule_;

    QMap<QString, Atom *> atomMap;

    struct BondStruct
    {
        QString fromAtomId;
        QString toAtomId;
        short order;
    };
    QVector<BondStruct> bonds;
};

#endif // CMLREADER_H
