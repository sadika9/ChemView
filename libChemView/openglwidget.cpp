#include "openglwidget.h"

#include "molecule.h"
#include "atom.h"
#include "bond.h"

#include <QMouseEvent>
#include <QApplication>


static const char *vertexShaderSource =
        "attribute highp vec3 modelSpaceVertexPos;\n"
        "attribute highp vec3 modelSpaceVertexNormal;\n"
        "attribute lowp vec4 colorAttr;\n"
        "varying highp vec3 worldSpacePos;\n"
        "varying highp vec3 cameraSpaceNormal;\n"
        "varying highp vec3 cameraSpaceEyeDirection;\n"
        "varying highp vec3 cameraSpaceLightDirection;\n"
        "varying lowp vec4 col;\n"
        "uniform highp mat4 model;\n"
        "uniform highp mat4 view;\n"
        "uniform highp mat4 projection;\n"
        "uniform highp vec3 worldSpaceLightPosition;\n"
        "void main() {\n"
        "   //col = colorAttr;\n"
        "   gl_Position = projection * view * model * vec4(modelSpaceVertexPos, 1);\n"
        "   worldSpacePos = (model * vec4(modelSpaceVertexPos ,1)).xyz;\n"
        "   vec3 cameraSpaceVertexPos = (view * model * vec4(modelSpaceVertexPos ,1)).xyz;\n"
        "   cameraSpaceEyeDirection = vec3(0,0,0) - cameraSpaceVertexPos;\n"
        "   vec3 cameraSpaceLightPos = (view * vec4(worldSpaceLightPosition ,1)).xyz;\n"
        "   cameraSpaceLightDirection = cameraSpaceLightPos + cameraSpaceEyeDirection;\n"
        "   cameraSpaceNormal = (view * model * vec4(modelSpaceVertexNormal, 0)).xyz;\n"
        "}\n";

static const char *fragmentShaderSource =
        "varying highp vec3 worldSpacePos;\n"
        "varying highp vec3 cameraSpaceNormal;\n"
        "varying highp vec3 cameraSpaceEyeDirection;\n"
        "varying highp vec3 cameraSpaceLightDirection;\n"
        "uniform highp mat4 model;\n"
        "uniform highp mat4 view;\n"
        "uniform highp vec3 worldSpaceLightPosition;\n"
        "uniform lowp vec3 color;\n"
        "void main() {\n"
        "   vec3 LightColor = vec3(1,1,1);"
        "   float LightPower = 200.0f;"
        "   vec3 MaterialDiffuseColor = color.xyz;"
        "   vec3 MaterialAmbientColor = vec3(0.2,0.2,0.2) * MaterialDiffuseColor;"
        "   vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);"
        "   float distance = length(worldSpaceLightPosition - worldSpacePos);"
        "   vec3 n = normalize(cameraSpaceNormal);"
        "   vec3 l = normalize(cameraSpaceLightDirection);"
        "   float cosTheta = clamp( dot( n,l ), 0,1 );"
        "   vec3 E = normalize(cameraSpaceEyeDirection);"
        "   vec3 R = reflect(-l,n);"
        "   float cosAlpha = clamp( dot( E,R ), 0,1 );"
        "   gl_FragColor = vec4("
        "   MaterialAmbientColor +"
        "   MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +"
        "   MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance), 1);\n"
        "}\n";


OpenGLWidget::OpenGLWidget(QWidget *parent) :
    QGLWidget(parent),
    m_fov(45.0),
    m_zNearPlane(0.1),
    m_zFarPlane(100.0),
    m_molecule(nullptr),
    m_angularSpeed(0),
    m_translation(QVector3D(0, 0, -15))
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

    updateGL();
}


void OpenGLWidget::setFov(float fov)
{
    if (m_fov == fov)
        return;

    m_fov = fov;
    emit fovChanged(m_fov);
    updateGL();
}

void OpenGLWidget::setZNearPlane(float zNearPlane)
{
    if (m_zNearPlane == zNearPlane)
        return;

    m_zNearPlane = zNearPlane;
    emit zNearPlaneChanged(m_zNearPlane);
    updateGL();
}

void OpenGLWidget::setZFarPlane(float zFarPlane)
{
    if (m_zFarPlane == zFarPlane)
        return;

    m_zFarPlane = zFarPlane;
    emit zFarPlaneChanged(m_zFarPlane);
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

        QVector3D n = QVector3D(diff.x(), -diff.y(), 0);
        m_translation += n;

        updateGL();
    }
}

void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    m_fov = 45.0;
    m_zNearPlane = 0.1;
    m_zFarPlane = 100.0,
    m_angularSpeed = 0;
    m_translation = QVector3D(0, 0, -15);
    m_rotation = QQuaternion();

    resizeGL(width(), height());
    updateGL();
}

void OpenGLWidget::wheelEvent(QWheelEvent *e)
{
    QPoint numDegrees = e->angleDelta() / 8;

    if (!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        float fov = m_fov - numSteps.y() * 5;

        if (fov <= 0)
        {
            fov = 1;
        }
        else if (fov > 150)
        {
            fov = 150;
        }

        setFov(fov);

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
#ifdef USE_ASSIMP
    QString sphere("://meshes/sphere.obj");
    QString cylinder("://meshes/cylinder.obj");
#endif

#ifndef USE_ASSIMP
    QString sphere("meshes/sphere.obj");
    QString cylinder("meshes/cylinder.obj");
#endif
    m_atomMesh.init(sphere, "modelSpaceVertexPos", "modelSpaceVertexNormal", "a_texcoord");
    m_bondMesh.init(cylinder, "modelSpaceVertexPos", "modelSpaceVertexNormal", "a_texcoord");
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
    m_projection.perspective(m_fov, aspect, m_zNearPlane, m_zFarPlane);

    /*
    // Perspective frustum projection
    // using m_fov to zoom in/out purpose
    // other constants are used to get proper viewing.
    float wFrustum = w * m_fov / (2.0 * 100000);
    float hFrustum = h * m_fov / (2.0 * 100000);
    m_projection.frustum(-wFrustum, wFrustum, -hFrustum, hFrustum, m_nearPlane, m_farPlane);
    */

    /*
    // Orthographic projection
    // here we use m_fov to zoom in/out purpose
    // other constants are used to get proper viewing.
    float wOrtho = w * m_fov / (2.0 * 1000);
    float hOrtho = h * m_fov / (2.0 * 1000);
    m_projection.ortho(-wOrtho, wOrtho, -hOrtho, hOrtho, m_nearPlane, m_farPlane);
    */


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

inline void OpenGLWidget::draw()
{
    QMatrix4x4 view;
    view.translate(m_translation);

    m_program.setUniformValue(m_viewLocation, view);
    m_program.setUniformValue(m_projectionLocation, m_projection);
    m_program.setUniformValue("worldSpaceLightPosition", QVector3D(10, 10, 10));

    drawAtoms();
    drawBonds();
}

inline void OpenGLWidget::drawAtoms()
{
    for (Atom *atom : m_molecule->atoms())
    {
        // Calculate model view transformation
        QMatrix4x4 model;
        model.rotate(m_rotation);
        model.translate(atom->position());
        model.scale(atom->radius());

        m_program.setUniformValue(m_modelLocation, model);
        m_program.setUniformValue("color", atom->color() / 255.0f);

        m_atomMesh.render(&m_program);
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
            m_program.setUniformValue("color", QVector3D(0.56470588f, 0.56470588f, 0.56470588f));

            // Draw cube geometry
            m_bondMesh.render(&m_program);
        }
    }
}

