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

    QString formula() const;
    void setFormula(const QString &formula);

    QString title() const;
    void setTitle(const QString &title);

    double weight() const;
    void setWeight(double weight);

private:
    QVector<Atom *> m_atoms;
    QVector<Bond *> m_bonds;

    QString m_formula;
    QString m_title;
    double m_weight;
};

#endif // MOLECULE_H
