#ifndef CUBEGEOMETRY_H
#define CUBEGEOMETRY_H

#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>

class QOpenGLShaderProgram;

class CubeGeometry : protected QOpenGLFunctions
{
public:
    CubeGeometry();
    ~CubeGeometry();

    void init(QString position, QString texCoord);
    void drawGeometry(QOpenGLShaderProgram *program);

    void setShaderAttrs(QString position, QString texCoord);

private:
    GLuint cubeVboIds[2];

    QString m_position;
    QString m_texCoord;

    struct VertexData
    {
        QVector3D position;
        QVector2D texCoord;
    };
};

#endif // CUBEGEOMETRY_H
