#pragma once

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
    
    CameraType type() const { return type_; }
    int pixelWidth() const { return pixelWidth_; }
    int pixelHeight() const { return pixelHeight_; }
    float nearPlane() const { return nearPlane_; }
    float farPlane() const { return farPlane_; }
    float orthographicSize() const { return orthographicSize_; }
    float fov() const { return fov_; }
    
    Matrix4x4 worldToCameraMatrix() const;
    Matrix4x4 projectionMatrix() const { return projectionMatrix_; }
    
    void setType(CameraType type);
    void setPixelWidth(int w);
    void setPixelHeight(int h);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);
    void setOrthographicSize(float size);
    void setFov(float fov);
    
private:
    CameraType type_;
    int pixelWidth_;
    int pixelHeight_;
    float nearPlane_;
    float farPlane_;
    float orthographicSize_;
    float fov_;
    Matrix4x4 projectionMatrix_;
    
    void updateProjectionMatrix();
};
