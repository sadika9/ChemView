#include "camera.h"

Camera::Camera(QObject *parent) :
    QObject(parent),
    m_nearPlane(0.1f),
    m_farPlane(100.0f),
    m_fieldOfView(45.0f),
    m_aspectRatio(1.0f),
    m_position(0.0f, 0.0f, 1.0f),
    m_viewCenter(0.0f, 0.0f, 0.0f),
    m_upVector(0.0f, 1.0f, 0.0f)
{
}

float Camera::nearPlane() const
{
    return m_nearPlane;
}

void Camera::setNearPlane(float nearPlane)
{
    m_nearPlane = nearPlane;
}

float Camera::farPlane() const
{
    return m_farPlane;
}

void Camera::setFarPlane(float farPlane)
{
    m_farPlane = farPlane;
}

float Camera::fieldOfView() const
{
    return m_fieldOfView;
}

void Camera::setFieldOfView(float fieldOfView)
{
    m_fieldOfView = fieldOfView;
}

float Camera::aspectRatio() const
{
    return m_aspectRatio;
}

void Camera::setAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

QVector3D Camera::position() const
{
    return m_position;
}

void Camera::setPosition(const QVector3D &position)
{
    m_position = position;
}

QVector3D Camera::viewCenter() const
{
    return m_viewCenter;
}

void Camera::setViewCenter(const QVector3D &viewCenter)
{
    m_viewCenter = viewCenter;
}

QVector3D Camera::upVector() const
{
    return m_upVector;
}

void Camera::setUpVector(const QVector3D &upVector)
{
    m_upVector = upVector;
}

QMatrix4x4 Camera::projectionMatrix() const
{
    QMatrix4x4 mat;
    mat.perspective(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);

    return mat;
}

QMatrix4x4 Camera::viewMatrix() const
{
    QMatrix4x4 mat;
    mat.lookAt(m_position, m_viewCenter, m_upVector);

    return mat;
}
