#include "Camera.hpp"

Camera::Camera()
    : framebuffer_(0),
    type_(CameraType::Perspective),
    pixelWidth_(100),
    pixelHeight_(100),
    nearPlane_(0.1),
    farPlane_(350.0),
    orthographicSize_(25.0),
    fov_(60.0)
{
    updateProjectionMatrix();
}

Matrix4x4 Camera::worldToCameraMatrix() const
{
    return projectionMatrix_ * worldToLocal();
}

void Camera::setFramebuffer(GLuint framebuffer)
{
    framebuffer_ = framebuffer;
}

void Camera::setType(CameraType type)
{
    type_ = type;
    updateProjectionMatrix();
}

void Camera::setPixelWidth(int w)
{
    pixelWidth_ = w;
    updateProjectionMatrix();
}

void Camera::setPixelHeight(int h)
{
    pixelHeight_ = h;
    updateProjectionMatrix();
}

void Camera::setNearPlane(float nearPlane)
{
    nearPlane_ = nearPlane;
    updateProjectionMatrix();
}

void Camera::setFarPlane(float farPlane)
{
    farPlane_ = farPlane;
    updateProjectionMatrix();
}

void Camera::setOrthographicSize(float size)
{
    orthographicSize_ = size;
    updateProjectionMatrix();
}

void Camera::setFov(float fov)
{
    fov_ = fov;
    updateProjectionMatrix();
}

void Camera::updateProjectionMatrix()
{
    float aspect = (float)pixelHeight_ / (float)pixelWidth_;
    
    if(type_ == CameraType::Perspective)
    {
        projectionMatrix_ = Matrix4x4::perspective(fov_, aspect, nearPlane_, farPlane_);
    }
    else if(type_ == CameraType::Orthographic)
    {
        float halfSize = orthographicSize_ / 2.0;
        float l = -halfSize;
        float r = halfSize;
        float t = (halfSize * aspect);
        float b = -(halfSize * aspect);
        
        projectionMatrix_ = Matrix4x4::orthographic(l, r, b, t, nearPlane_, farPlane_);
    }
}

