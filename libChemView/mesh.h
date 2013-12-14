#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>

class QOpenGLShaderProgram;

struct PackedVertex;

class Mesh : protected QOpenGLFunctions
{
public:
    Mesh();

    void init(QString meshPath, QString vertexPos, QString vertexNormal);
    void render(QOpenGLShaderProgram *program);

private:
    bool read(QString path);

    QVector<unsigned short> m_indices;
    QVector<QVector3D> m_indexed_vertices;
    QVector<QVector3D> m_indexed_normals;

    GLuint m_vboIds[3];

    QString m_vertexPos;
    QString m_vertexNormal;

    bool isInitSuccessful;
};

#endif // MESH_H
