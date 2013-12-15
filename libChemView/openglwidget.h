#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "camera.h"
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
    void resetCamera();
    void drawAtoms();
    void drawBonds();

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

    Camera m_camera;

    QBasicTimer m_timer;

    QVector2D m_lastMousePosition;
    QVector3D m_rotationAxis;
    qreal m_angularSpeed;
    QQuaternion m_rotation;

    QOpenGLShaderProgram m_program;
};

#endif // OPENGLWIDGET_H
