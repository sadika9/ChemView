#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "geometryengine.h"
#include "mesh.h"

#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QBasicTimer>

class Molecule;

class OpenGLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = 0);
    ~OpenGLWidget();


    Molecule *molecule() const;
    void setMolecule(Molecule *molecule);

public slots:
    void setFov(float fov);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

signals:
    void fovChanged(float fov);
    void nearPlaneChanged(float nearPlane);
    void farPlaneChanged(float farPlane);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void initShaders();

private:
    void draw();
    void drawAtoms();
    void drawBonds();

    float m_fov;
    float m_nearPlane;
    float m_farPlane;

    Molecule *m_molecule;

    GeometryEngine m_geometryEngine;
    Mesh m_atomMesh;
    Mesh m_bondMesh;

    GLuint m_modelLocation;
    GLuint m_viewLocation;
    GLuint m_projectionLocation;
    GLuint m_vertexLocation;
    GLuint m_colorLocation;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;

    QBasicTimer m_timer;

    QVector2D m_mousePressPosition;
    QVector3D m_rotationAxis;
    qreal m_angularSpeed;
    QQuaternion m_rotation;

    QOpenGLShaderProgram m_program;
};

#endif // OPENGLWIDGET_H
