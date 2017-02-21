#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4x4.hpp"

class Object
{
public:
    Object();
    
    Vector3 position() const { return position_; }
    Quaternion rotation() const { return rotation_; }
    Vector3 scale() const { return scale_; }
    
    // Transformation matrices
    Matrix4x4 worldToLocal() const { return worldToLocal_; }
    Matrix4x4 localToWorld() const { return localToWorld_; }
    
    // Object axis in world space
    Vector4 forward() const;
    
    void setPosition(const Vector3 &pos);
    void setRotation(const Quaternion &rot);
    void setScale(const Vector3 &scale);

private:
    Vector3 position_;
    Quaternion rotation_;
    Vector3 scale_;
    Matrix4x4 worldToLocal_;
    Matrix4x4 localToWorld_;
    
    void recreateTransformation();
};
