#include "Camera.hpp"

Camera::Camera()
    : framebuffer_(0),
    type_(CameraType::Perspective),
    pixelOffsetX_(0),
    pixelOffsetY_(0),
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

Matrix4x4 Camera::cameraToWorldMatrix() const
{
    return localToWorld() * invProjectionMatrix_;
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

void Camera::setPixelOffsetX(int offset)
{
    pixelOffsetX_ = offset;
}

void Camera::setPixelOffsetY(int offset)
{
    pixelOffsetY_ = offset;
}

void Camera::setPixelWidth(int w)
{
    pixelWidth_ = w;
    
    // Aspect ratio changed. Recompute projection matrix.
    updateProjectionMatrix();
}

void Camera::setPixelHeight(int h)
{
    pixelHeight_ = h;
    
    // Aspect ratio changed. Recompute projection matrix.
    updateProjectionMatrix();
}

void Camera::setNearPlane(float nearPlane)
{
    nearPlane_ = nearPlane;
    
    // Clipping planes changed. Recompute projection matrix.
    updateProjectionMatrix();
}

void Camera::setFarPlane(float farPlane)
{
    farPlane_ = farPlane;
    
    // Clipping planes changed. Recompute projection matrix.
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
    float aspect = (float)pixelWidth_ / (float)pixelHeight_;
    
    if(type_ == CameraType::Perspective)
    {
        projectionMatrix_ = Matrix4x4::perspective(fov_, aspect, nearPlane_, farPlane_);
        invProjectionMatrix_ = Matrix4x4::perspectiveInverse(fov_, aspect, nearPlane_, farPlane_);
    }
    else if(type_ == CameraType::Orthographic)
    {
        float halfSize = orthographicSize_ / 2.0;
        float l = -(halfSize * aspect);
        float r = (halfSize * aspect);
        float t = halfSize;
        float b = -halfSize;
        
        projectionMatrix_ = Matrix4x4::orthographic(l, r, b, t, nearPlane_, farPlane_);
    }
}

