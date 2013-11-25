#ifndef OBREADER_H
#define OBREADER_H

#include <QObject>

class Molecule;

namespace OpenBabel {
class OBMol;
}

class OBReader : public QObject
{
    Q_OBJECT
public:
    explicit OBReader(QObject *parent = 0);


    Molecule *molecule() const;

    bool readFile(QString fileName);

private:
    bool toMolecule(OpenBabel::OBMol *obMol);

    Molecule *m_molecule;
};

#endif // OBREADER_H
