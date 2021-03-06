#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>

class QOpenGLShaderProgram;

class Mesh : protected QOpenGLFunctions
{
public:
    Mesh();

    void init(QString meshPath, QString vertexPos, QString vertexNormal, QOpenGLShaderProgram *program);
    void render();

private:
    bool read(QString filePath);

    QVector<unsigned short> m_indices;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;

    GLuint m_vboIds[3];

    QString m_vertexPos;
    QString m_vertexNormal;
    QOpenGLShaderProgram *m_program;

    bool isInitSuccessful;
};

#endif // MESH_H
