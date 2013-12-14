#include "mesh.h"

#include <QOpenGLShaderProgram>
#include <QFile>
#include <QDebug>

Mesh::Mesh() :
    isInitSuccessful(false)
{
}

void Mesh::init(QString meshPath, QString vertexPos, QString vertexNormal)
{
    m_vertexPos = vertexPos;
    m_vertexNormal = vertexNormal;

    initializeOpenGLFunctions();

    // Generate VBOs
    int nBuffers = sizeof(m_vboIds)/sizeof(GLuint);
    glGenBuffers(nBuffers, m_vboIds);

    bool loaded = read(meshPath);
    if (!loaded)
    {
        isInitSuccessful = false;
        qDebug() << Q_FUNC_INFO << "Object loading failed.";
        return;
    }

    // Transfer vertex data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_indexed_vertices.size() * sizeof(QVector3D), &m_indexed_vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, m_indexed_normals.size() * sizeof(QVector3D), &m_indexed_normals[0], GL_STATIC_DRAW);

    // Transfer the indices to VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned short), &m_indices[0] , GL_STATIC_DRAW);

    // Successfuly initialized.
    isInitSuccessful = true;
}

void Mesh::render(QOpenGLShaderProgram *program)
{
    if (!isInitSuccessful)
    {
        qDebug() << Q_FUNC_INFO << "Not initialized.";
        return;
    }

    // Vertices
    int vertexLocation = program->attributeLocation(m_vertexPos);
    program->enableAttributeArray(vertexLocation);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    // Normals
     int normalLocation = program->attributeLocation(m_vertexNormal);
     program->enableAttributeArray(normalLocation);
     glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
     glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[2]);

    // Draw the triangles !
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, (const void*)0);
}

/**
 * This method is based on loadOBJ() from http://www.opengl-tutorial.org/ and
 * model reading on qt-labs-modelviewer from https://qt.gitorious.org/qt-labs/modelviewer
 */
bool Mesh::read(QString path)
{
    // Variables to store vertices & normals
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;

    // Temporary variables to store values that read from file, (raw data)
    QVector<unsigned int> vertexIndices, normalIndices;
    QVector<QVector3D> tempVertices;
    QVector<QVector3D> tempNormals;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.errorString();
        return false;
    }

    QTextStream fs(&file);


    while (!fs.atEnd())
    {
        QString line = fs.readLine();

        if (line.isEmpty() || line[0] == '#')
            continue;

        QTextStream ls(&line);
        QString tag;
        ls >> tag;
        if (tag.compare("v") == 0)
        {
            float x, y, z;
            ls >> x >> y >> z;
            QVector3D vertex(x, y, z);
            tempVertices.append(vertex);
        }
        else if (tag.compare("vn") == 0)
        {
            float x, y, z;
            ls >> x >> y >> z;
            QVector3D normal(x, y, z);
            tempNormals.append(normal);
        }
        else if (tag.compare("f") == 0)
        {
            QString vertex;
            while (!ls.atEnd())
            {
                ls >> vertex;
                QStringList indices = vertex.split("//");
                vertexIndices.append(indices.value(0).toUInt());
                normalIndices.append(indices.value(1).toUInt());
            }
        }
    }

    // For each vertex of each triangle
    int vertexIndicesSize = vertexIndices.size();
    for(int i = 0; i < vertexIndicesSize; ++i)
    {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        QVector3D vertex = tempVertices[ vertexIndex-1 ];
        QVector3D normal = tempNormals[ normalIndex-1 ];

        // Put the attributes in buffers
        vertices.push_back(vertex);
        normals.push_back(normal);
    }

    // index VBOs
    indexVbo(vertices, normals);

    return true;
}

/**
 * Index VBOs
 */
void Mesh::indexVbo(QVector<QVector3D> &vertices, QVector<QVector3D> &normals)
{
    QMap<PackedVertex,unsigned short> vertexToOutIndex;

    // For each input vertex
    for (int i = 0; i < vertices.size(); ++i)
    {
        PackedVertex packed = {vertices[i], normals[i]};

        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);

        if (found)  // A similar vertex is already in the VBO, use it instead !
        {
            m_indices.push_back( index );
        }
        else    // If not, it needs to be added in the output data.
        {
            m_indexed_vertices.push_back(vertices[i]);
            m_indexed_normals.push_back(normals[i]);
            unsigned short newindex = (unsigned short)m_indexed_vertices.size() - 1;
            m_indices.push_back(newindex);
            vertexToOutIndex[packed] = newindex;
        }
    }
}

/**
 * Helper function for indexVBO
 */
bool Mesh::getSimilarVertexIndex(PackedVertex &packed, QMap<PackedVertex, unsigned short> &vertexToOutIndex, unsigned short &result)
{
    QMap<PackedVertex, unsigned short>::iterator it = vertexToOutIndex.find(packed);

    if (it == vertexToOutIndex.end())
    {
        return false;
    }
    else
    {
        result = it.value();
        return true;
    }
}
