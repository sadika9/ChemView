#ifndef MOLECULE_H
#define MOLECULE_H

#include <QObject>
#include <QVector>

class Atom;
class Bond;

class Molecule : public QObject
{
    Q_OBJECT
public:
    explicit Molecule(QObject *parent = 0);

    QVector<Atom *> atoms() const;
    void setAtoms(const QVector<Atom *> &atoms);

    QVector<Bond *> bonds() const;
    void setBonds(const QVector<Bond *> &bonds);

    void addAtom(Atom *atom);
    void addBond(Bond *bond);

private:
    QVector<Atom *> atoms_;
    QVector<Bond *> bonds_;
};

#endif // MOLECULE_H
