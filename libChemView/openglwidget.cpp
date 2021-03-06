#include "openglwidget.h"

#include "molecule.h"
#include "atom.h"
#include "bond.h"

#include <QMouseEvent>
#include <QApplication>


OpenGLWidget::OpenGLWidget(QWidget *parent) :
    QGLWidget(parent),
    m_molecule(nullptr),
    m_angularSpeed(0)
{
    resetCamera();
}

OpenGLWidget::~OpenGLWidget()
{
}

Molecule *OpenGLWidget::molecule() const
{
    return m_molecule;
}

void OpenGLWidget::setMolecule(Molecule *molecule)
{
    m_molecule = molecule;

    updateGL();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        // Stop rotating when clicked
        m_angularSpeed = 0.0;

        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
    }
    else if (e->button() == Qt::RightButton)
    {
        // Stop rotating when clicked
        m_angularSpeed = 0.0;

        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        // Stop rotating when clicked
        m_angularSpeed = 0.0;

        // Save mouse press position
        m_lastMousePosition = QVector2D(e->localPos());
    }
    else if (e->button() == Qt::MiddleButton)
    {
        QApplication::setOverrideCursor(Qt::SizeAllCursor);

        // Save mouse press position
        m_lastMousePosition = QVector2D(e->localPos());
    }

    e->accept();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QApplication::restoreOverrideCursor();

    if (e->button() == Qt::RightButton)
    {
        // Mouse release position - mouse press position
        QVector2D diff = QVector2D(e->localPos()) - m_lastMousePosition;

        // Rotation axis is perpendicular to the mouse position difference vector
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

        // Accelerate angular speed relative to the length of the mouse sweep
        qreal acc = diff.length() / 100.0;

        // Calculate new rotation axis as weighted sum
        m_rotationAxis = (m_rotationAxis * m_angularSpeed + n * acc).normalized();

        // Increase angular speed
        m_angularSpeed += acc;
    }

    e->accept();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        // Mouse release position - mouse press position
        QVector2D diff = QVector2D(e->localPos()) - m_lastMousePosition;
        m_lastMousePosition = QVector2D(e->localPos());

        // Rotation axis is perpendicular to the mouse position difference vector
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

        // Update rotation
        m_rotation = QQuaternion::fromAxisAndAngle(n, 2) * m_rotation;

        // Update scene
        updateGL();
    }
    else if (e->buttons() & Qt::MiddleButton)
    {
        QVector2D diff = (QVector2D(e->localPos()) - m_lastMousePosition) / 100;
        m_lastMousePosition = QVector2D(e->localPos());

        QVector3D n = QVector3D(-diff.x(), diff.y(), 0);
        m_camera.setViewCenter(m_camera.viewCenter() + n);

        updateGL();
    }
}

void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    m_angularSpeed = 0;
    m_rotation = QQuaternion();

    resetCamera();

    updateGL();
}

void OpenGLWidget::wheelEvent(QWheelEvent *e)
{
    QPoint numDegrees = e->angleDelta() / 8;

    if (!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;

        QVector3D val = m_camera.position() - QVector3D(0, 0, numSteps.y());

        if (val.z() > m_camera.farPlane() || val.z() < m_camera.nearPlane())
            return;

        m_camera.setPosition(val);

        resizeGL(width(), height());
        updateGL();
    }

    e->accept();
}

void OpenGLWidget::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    m_angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (m_angularSpeed < 0.01)
    {
        m_angularSpeed = 0.0;
    }
    else
    {
        // Update rotation
        m_rotation = QQuaternion::fromAxisAndAngle(m_rotationAxis, m_angularSpeed) * m_rotation;

        // Update scene
        updateGL();
    }
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qglClearColor(Qt::black);
    initShaders();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // Use QBasicTimer because its faster than QTimer
    m_timer.start(12, this);

    // Initializing meshes
    // Try these combinations too. ;-)
    // sphere: suzanne.obj & cylinder: cube.obj
    // sphere: cube.obj & cylinder: cube.obj
    m_atomMesh.init("://meshes/sphere.obj", "modelSpaceVertexPos", "modelSpaceVertexNormal", &m_program);
    m_bondMesh.init("://meshes/cylinder.obj", "modelSpaceVertexPos", "modelSpaceVertexNormal", &m_program);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    m_camera.setAspectRatio(aspect);
}

void OpenGLWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_molecule)
        return;

    m_program.setUniformValue(m_viewLocation, m_camera.viewMatrix());
    m_program.setUniformValue(m_projectionLocation, m_camera.projectionMatrix());
    m_program.setUniformValue(m_lightLocation, QVector3D(10, 10, 10));

    drawAtoms();
    drawBonds();
}

void OpenGLWidget::initShaders()
{
    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compile vertex shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "://shaders/vertex.vsh"))
        close();

    // Compile fragment shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "://shaders/fragment.fsh"))
        close();

    // Link shader pipeline
    if (!m_program.link())
        close();

    // Bind shader pipeline for use
    if (!m_program.bind())
        close();

    // Restore system locale
    setlocale(LC_ALL, "");

    // Get attribute locations
    m_vertexLocation = m_program.attributeLocation("modelSpaceVertexPos");
    m_colorLocation = m_program.uniformLocation("color");
    m_modelLocation = m_program.uniformLocation("model");
    m_viewLocation = m_program.uniformLocation("view");
    m_projectionLocation = m_program.uniformLocation("projection");
    m_lightLocation = m_program.uniformLocation("worldSpaceLightPosition");
}

inline void OpenGLWidget::resetCamera()
{
    m_camera.setFieldOfView(45.0f);
    m_camera.setNearPlane(0.1f);
    m_camera.setFarPlane(100.0f);
    m_camera.setPosition(QVector3D(0, 0, 15.f));
    m_camera.setViewCenter(QVector3D(0, 0, 0));

    qreal aspect = qreal(size().width()) / qreal(size().height() ? size().height() : 1);
    m_camera.setAspectRatio(aspect);
}

inline void OpenGLWidget::drawAtoms()
{
    const float atomRadiusScale = 0.5f;

    for (Atom *atom : m_molecule->atoms())
    {
        // Calculate model view transformation
        QMatrix4x4 model;
        model.rotate(m_rotation);
        model.translate(atom->position());
        model.scale(atom->radius() * atomRadiusScale);

        m_program.setUniformValue(m_modelLocation, model);
        m_program.setUniformValue(m_colorLocation, atom->color());

        m_atomMesh.render();
    }
}

inline void OpenGLWidget::drawBonds()
{
    for (Bond *bond : m_molecule->bonds())
    {
        QVector3D fromPos = bond->fromAtom()->position();
        QVector3D toPos = bond->toAtom()->position();
        short order = bond->order();


        // Following variables are used to calculate positions between bonds.
        QVector3D tot = fromPos + toPos;
        QVector3D diff = fromPos - toPos;
        QVector3D cross1 = QVector3D::crossProduct(fromPos , toPos).normalized() / 4;
        QVector3D cross2 = QVector3D::crossProduct(cross1 , diff).normalized() / 4;
        double sqrt3_2 = sqrt(3) / 2;

        for (int i = 0; i < order; ++i)
        {
            QMatrix4x4 model;
            model.rotate(m_rotation);

            switch (order)
            {
            case 1:
            {
                model.translate((tot) / 2.0);
                break;
            }
            case 2:
            {
                if (i == 0)
                    model.translate((tot + cross2) / 2.0);
                else if (i == 1)
                    model.translate((tot - cross2) / 2.0);
                break;
            }
            case 3:
            {
                if (i == 0)
                    model.translate((tot + cross1) / 2.0);
                if (i == 1)
                    model.translate((tot + (-cross1 / 2) + cross2 * sqrt3_2) / 2.0);
                if (i == 2)
                    model.translate((tot + (-cross1 / 2) - cross2 * sqrt3_2) / 2.0);
                break;
            }
            case 4:
            {
                if (i == 0)
                    model.translate((tot + cross1 + cross2) / 2.0);
                else if (i == 1)
                    model.translate((tot - cross1 - cross2) / 2.0);
                else if (i == 2)
                    model.translate((tot + cross1 - cross2) / 2.0);
                else if (i == 3)
                    model.translate((tot - cross1 + cross2) / 2.0);
            }
            }


            /**
             * Bond angle calculation code is based on a pseudo code
             * from http://www.thjsmith.com/40
             */

            // This is the default direction for the cylinder
            QVector3D y = QVector3D(0,1,0);

            // Get diff between two points you want cylinder along
            QVector3D p = (fromPos - toPos);

            // Get CROSS product (the axis of rotation)
            QVector3D t = QVector3D::crossProduct(y , p);

            // Get angle. LENGTH is magnitude of the vector
            double angle = 180 / M_PI * acos(QVector3D::dotProduct(y, p) / p.length());

            // Rotate to align with two atoms
            model.rotate(angle, t);

            // Scale to fill up the distace between two atoms
            float length = fromPos.distanceToPoint(toPos) / 2.0;
            model.scale(0.04, length, 0.04);

            m_program.setUniformValue(m_modelLocation, model);
            m_program.setUniformValue(m_colorLocation, QVector3D(0.564706f, 0.564706f, 0.564706f));

            // Draw cylinder geometry
            m_bondMesh.render();
        }
    }
}

