#include "openglwidget.h"

#include "molecule.h"
#include "atom.h"
#include "bond.h"

#include <QMouseEvent>
#include <QApplication>


static const char *vertexShaderSource =
        "attribute highp vec4 modelSpaceVertexPos;\n"
        "attribute lowp vec4 colorAttr;\n"
        "varying lowp vec4 col;\n"
        "uniform highp mat4 model;\n"
        "uniform highp mat4 view;\n"
        "uniform highp mat4 projection;\n"
        "void main() {\n"
        "   //col = colorAttr;\n"
        "   gl_Position = projection * view * model * modelSpaceVertexPos;\n"
        "}\n";

static const char *fragmentShaderSource =
        "uniform lowp vec3 color;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(color, 1);\n"
        "}\n";


OpenGLWidget::OpenGLWidget(QWidget *parent) :
    QGLWidget(parent),
    m_fov(45.0),
    m_nearPlane(0.1),
    m_farPlane(100.0),
    m_molecule(nullptr),
    m_angularSpeed(0)
{
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
}


void OpenGLWidget::setFov(float fov)
{
    if (m_fov == fov)
        return;

    m_fov = fov;
    emit fovChanged(m_fov);
    updateGL();
}

void OpenGLWidget::setNearPlane(float nearPlane)
{
    if (m_nearPlane == nearPlane)
        return;

    m_nearPlane = nearPlane;
    emit nearPlaneChanged(m_nearPlane);
    updateGL();
}

void OpenGLWidget::setFarPlane(float farPlane)
{
    if (m_farPlane == farPlane)
        return;

    m_farPlane = farPlane;
    emit farPlaneChanged(m_farPlane);
    updateGL();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        // Stop rotating when clicked
        m_angularSpeed = 0.0;

        // Save mouse press position
        m_mousePressPosition = QVector2D(e->localPos());
    }

    e->accept();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        QApplication::restoreOverrideCursor();

        // Mouse release position - mouse press position
        QVector2D diff = QVector2D(e->localPos()) - m_mousePressPosition;

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

void OpenGLWidget::wheelEvent(QWheelEvent *e)
{
    QPoint numDegrees = e->angleDelta() / 8;

    if (!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        setFov(m_fov + numSteps.y() * 5);

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

    // Initialize meshes
    m_atomMesh.init("://meshes/sphere.obj", "modelSpaceVertexPos", "a_texcoord");
    m_bondMesh.init("://meshes/cylinder.obj", "modelSpaceVertexPos", "a_texcoord");
}

void OpenGLWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    // Reset projection
    m_projection.setToIdentity();
    // Set perspective projection
    m_projection.perspective(m_fov, aspect, m_nearPlane, m_farPlane);

    // temporary, move this to proper location
    m_view.translate(0, 0, -5);
    //    m_view.rotate(rotation);
}

void OpenGLWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_molecule)
        return;

    draw();
}

void OpenGLWidget::initShaders()
{
    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compile vertex shader
    //    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, "vertex.vert"))
    //        close();
    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource))
        close();

    // Compile fragment shader
    //    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment.frag"))
    //        close();
    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource))
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
    m_colorLocation = m_program.attributeLocation("colorAttr");
    m_modelLocation = m_program.uniformLocation("model");
    m_viewLocation = m_program.uniformLocation("view");
    m_projectionLocation = m_program.uniformLocation("projection");
}

void OpenGLWidget::draw()
{
    drawAtoms();
    drawBonds();
}

void OpenGLWidget::drawAtoms()
{
    for (Atom *atom : m_molecule->atoms())
    {
        // Calculate model view transformation
        QMatrix4x4 model;
        model.translate(atom->position());
        model.scale(0.3);

        QMatrix4x4 view;
        view.translate(0, 0, -5);
        view.rotate(m_rotation);

        // Set model-view-projection matrix
        m_program.setUniformValue(m_modelLocation, model);
        m_program.setUniformValue(m_viewLocation, view);
        m_program.setUniformValue(m_projectionLocation, m_projection);

        m_program.setUniformValue("color", atom->color());

        m_atomMesh.render(&m_program);
    }
}

void OpenGLWidget::drawBonds()
{
    for (Bond *bond : m_molecule->bonds())
    {
        QVector3D fromPos = bond->fromAtom()->position();
        QVector3D toPos = bond->toAtom()->position();
        QVector3D center = fromPos + toPos;
        short order = bond->order();

        for (int i = 0; i < order; ++i)
        {
            QMatrix4x4 model;

            switch (order)
            {
            case 1:
                model.translate((center) / 2.0);
                break;
            case 2:
                QVector3D offset(0.1, 0.1, 0.1);

                if (i == 0)
                    model.translate((center + offset) / 2.0);
                else if (i == 1)
                    model.translate((center - offset) / 2.0);
                break;
                //            case 3:
                //                if (i == 0)
                //                    model.translate((center + QVector3D(0.05, 0.05, 0.1)));
                //                if (i == 1)
                //                    model.translate((center + QVector3D(0.05, 0.1, 0.05)));
                //                if (i == 2)
                //                    model.translate((center + QVector3D(0.1, 0.05, 0.05)));
                //                break;
            }


            /**
             * Bond angle calculation code is based on a pseudo code
             * from http://www.thjsmith.com/40
             */

            // This is the default direction for the cylinder
            QVector3D x = QVector3D(1,0,0);

            // Get diff between two points you want cylinder along
            QVector3D p = (fromPos - toPos);

            // Get CROSS product (the axis of rotation)
            QVector3D t = QVector3D::crossProduct(x , p);

            // Get angle. LENGTH is magnitude of the vector
            double angle = 180 / M_PI * acos(QVector3D::dotProduct(x, p) / p.length());

            // Rotate to align with two atoms
            model.rotate(angle, t);

            // Scale to fill up the distace between two atoms
            float length = fromPos.distanceToPoint(toPos) / 2.0;
            model.scale(length, 0.04, 0.04);


            QMatrix4x4 view;
            view.translate(0, 0, -5);
            view.rotate(m_rotation);

            // Set model-view-projection matrix
            m_program.setUniformValue(m_modelLocation, model);
            m_program.setUniformValue(m_viewLocation, view);
            m_program.setUniformValue(m_projectionLocation, m_projection);

            // Draw cube geometry
            m_bondMesh.render(&m_program);
        }
    }
}

