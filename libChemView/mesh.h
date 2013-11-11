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

    void init(QString meshPath, QString position, QString texCoord);
    void render(QOpenGLShaderProgram *program);

private:
    bool loadObj(QString path);

    void indexVbo(QVector<QVector3D> &vertices,
                  QVector<QVector2D> &uvs,
                  QVector<QVector3D> &normals);

    bool getSimilarVertexIndex(PackedVertex &packed,
                               QMap<PackedVertex, unsigned short> &vertexToOutIndex,
                               unsigned short &result);

#ifdef USE_ASSIMP
    bool loadAssImp(QString path);
#endif


    QVector<unsigned short> m_indices;
    QVector<QVector3D> m_indexed_vertices;
    QVector<QVector2D> m_indexed_uvs;
    QVector<QVector3D> m_indexed_normals;

    GLuint m_vboIds[3];

    QString m_position;
    QString m_texCoord;

    bool isInitSuccessful;
};

struct PackedVertex
{
    QVector3D position;
    QVector2D uv;
    QVector3D normal;

    bool operator<(const PackedVertex that) const
    {
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
    }
};

#endif // MESH_H
