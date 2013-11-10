#include "openglwidget.h"

#include <QMouseEvent>

static const char *vertexShaderSource =
        "attribute highp vec4 posAttr;\n"
        "attribute lowp vec4 colAttr;\n"
        "varying lowp vec4 col;\n"
        "uniform highp mat4 model;\n"
        "uniform highp mat4 view;\n"
        "uniform highp mat4 projection;\n"
        "void main() {\n"
        "   col = colAttr;\n"
        "   gl_Position = projection * view * model * posAttr;\n"
        "}\n";

static const char *fragmentShaderSource =
        "varying lowp vec4 col;\n"
        "void main() {\n"
        "   gl_FragColor = col;\n"
        "}\n";


OpenGLWidget::OpenGLWidget(QWidget *parent) :
    QGLWidget(parent),
    m_zNear(3.0),
    m_zFar(7.0),
    m_fov(45.0),
    m_angularSpeed(0)
{
}

OpenGLWidget::~OpenGLWidget()
{
}

void OpenGLWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    m_mousePressPosition = QVector2D(e->localPos());
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
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
    m_projection.perspective(m_fov, aspect, m_zNear, m_zFar);

    // temporary, move this to proper location
    m_view.translate(0, 0, -5);
    //    m_view.rotate(rotation);
}

void OpenGLWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    m_vertexLocation = m_program.attributeLocation("posAttr");
    m_colorLocation = m_program.attributeLocation("colAttr");
    m_modelLocation = m_program.uniformLocation("model");
    m_viewLocation = m_program.uniformLocation("view");
    m_projectionLocation = m_program.uniformLocation("projection");
}

void OpenGLWidget::draw()
{
    QMatrix4x4 proj;
    proj.perspective(60, 4.0/3.0, 0.1, 100.0);

    QMatrix4x4 view;
    view.translate(0, 0, -5);
    view.rotate(m_rotation);

    QMatrix4x4 model;
    model.translate(0, 0, -2);
    model.rotate(100.0f * 2 /60, 0, 1, 0);

    m_program.setUniformValue(m_modelLocation, model);
    m_program.setUniformValue(m_viewLocation, view);
    m_program.setUniformValue(m_projectionLocation, proj);

    GLfloat vertices[] = {
        0.0f, 0.707f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    glVertexAttribPointer(m_vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colorLocation, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}
