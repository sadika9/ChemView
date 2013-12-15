#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = 0);

    float nearPlane() const;
    void setNearPlane(float nearPlane);

    float farPlane() const;
    void setFarPlane(float farPlane);

    float fieldOfView() const;
    void setFieldOfView(float fieldOfView);

    float aspectRatio() const;
    void setAspectRatio(float aspectRatio);

    QVector3D position() const;
    void setPosition(const QVector3D &position);

    QVector3D viewCenter() const;
    void setViewCenter(const QVector3D &viewCenter);

    QVector3D upVector() const;
    void setUpVector(const QVector3D &upVector);

    QMatrix4x4 projectionMatrix() const;
    QMatrix4x4 viewMatrix() const;

private:
    float m_nearPlane;
    float m_farPlane;

    float m_fieldOfView;
    float m_aspectRatio;

    QVector3D m_position;
    QVector3D m_viewCenter;
    QVector3D m_upVector;

};

#endif // CAMERA_H
