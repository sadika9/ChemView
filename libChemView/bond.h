#ifndef BOND_H
#define BOND_H

#include <QObject>

class Atom;

class Bond : public QObject
{
    Q_OBJECT
public:
    explicit Bond(QObject *parent = 0);


    Atom *fromAtom() const;
    void setFromAtom(Atom *fromAtom);

    Atom *toAtom() const;
    void setToAtom(Atom *toAtom);

    short order() const;
    void setOrder(short order);

private:
    Atom *m_fromAtom;
    Atom *m_toAtom;

    short m_order;
};

#endif // BOND_H
