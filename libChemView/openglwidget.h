#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

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
    void setZNearPlane(float zNearPlane);
    void setZFarPlane(float zFarPlane);

signals:
    void fovChanged(float fov);
    void zNearPlaneChanged(float zNearPlane);
    void zFarPlaneChanged(float zFarPlane);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *e);

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
    float m_zNearPlane;
    float m_zFarPlane;

    Molecule *m_molecule;

    Mesh m_atomMesh;
    Mesh m_bondMesh;

    // uniform/attribute locations on shaders
    GLuint m_modelLocation;
    GLuint m_viewLocation;
    GLuint m_projectionLocation;
    GLuint m_vertexLocation;
    GLuint m_colorLocation;
    GLuint m_lightLocation;

    QMatrix4x4 m_projection;

    QBasicTimer m_timer;

    QVector2D m_lastMousePosition;
    QVector3D m_rotationAxis;
    qreal m_angularSpeed;
    QQuaternion m_rotation;
    QVector3D m_translation;

    QOpenGLShaderProgram m_program;
};

#endif // OPENGLWIDGET_H
