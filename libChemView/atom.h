#ifndef ATOM_H
#define ATOM_H

#include <QObject>
#include <QtGui/QVector3D>

class Atom : public QObject
{
    Q_OBJECT
public:
    explicit Atom(QObject *parent = 0);

    QString element() const;
    void setElement(const QString &element);

    QVector3D position() const;
    void setPosition(const QVector3D &position);

    QVector3D color() const;
    void setColor(const QVector3D &color);

private:
    QString m_element;
    QVector3D m_position;
    QVector3D m_color;
};

#endif // ATOM_H
