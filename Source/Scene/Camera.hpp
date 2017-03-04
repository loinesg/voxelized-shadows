#pragma once

#include <QGLWidget>

#include "Matrix4x4.hpp"
#include "Object.hpp"

enum class CameraType
{
    Perspective,
    Orthographic
};

class Camera : public Object
{
public:
    Camera();
    
    // Render target framebuffer
    GLuint framebuffer() const { return framebuffer_; }
    
    // Camera type
    CameraType type() const { return type_; }
    
    // Viewport settings
    int pixelOffsetX() const { return pixelOffsetX_; }
    int pixelOffsetY() const { return pixelOffsetY_; }
    int pixelWidth() const { return pixelWidth_; }
    int pixelHeight() const { return pixelHeight_; }
    
    // Clipping planes
    float nearPlane() const { return nearPlane_; }
    float farPlane() const { return farPlane_; }
    
    // Orthographic / perspective settings
    float orthographicSize() const { return orthographicSize_; }
    float fov() const { return fov_; }
    
    // Camera matrices
    Matrix4x4 worldToCameraMatrix() const;
    Matrix4x4 projectionMatrix() const { return projectionMatrix_; }
    Matrix4x4 cameraToWorldMatrix() const;
    
    void setFramebuffer(GLuint framebuffer);
    void setType(CameraType type);
    void setPixelOffsetX(int offset);
    void setPixelOffsetY(int offset);
    void setPixelWidth(int w);
    void setPixelHeight(int h);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);
    void setOrthographicSize(float size);
    void setFov(float fov);
    
private:
    GLuint framebuffer_;
    CameraType type_;
    int pixelOffsetX_;
    int pixelOffsetY_;
    int pixelWidth_;
    int pixelHeight_;
    float nearPlane_;
    float farPlane_;
    float orthographicSize_;
    float fov_;
    Matrix4x4 projectionMatrix_;
    Matrix4x4 invProjectionMatrix_;
    
    void updateProjectionMatrix();
};
