#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4x4.hpp"

class Object
{
public:
    Object();
    
    // Transformation settings
    Vector3 position() const { return position_; }
    Quaternion rotation() const { return rotation_; }
    Vector3 scale() const { return scale_; }
    
    // Transformation matrices
    Matrix4x4 worldToLocal() const { return worldToLocal_; }
    Matrix4x4 localToWorld() const { return localToWorld_; }
    
    // Object axis in world space
    Vector4 forward() const;
    Vector4 up() const;
    Vector4 right() const;
    
    // Transform direction vector from local to world space
    Vector3 localToWorldVector(const Vector3 &v) const;
    
    // Directly sets the transformation
    void setPosition(const Vector3 &pos);
    void setRotation(const Quaternion &rot);
    void setScale(const Vector3 &scale);
    
    // Translates the object by the given world space vector
    void translate(const Vector3 &translation);
    
    // Rotates the object around the given world space vector.
    // The angle is measured in degrees.
    void rotate(float angle, const Vector3 &axis);

private:
    
    // Transformation settings
    Vector3 position_;
    Quaternion rotation_;
    Vector3 scale_;
    
    // Cached transformation matrices
    Matrix4x4 worldToLocal_;
    Matrix4x4 localToWorld_;
    
    void recreateTransformation();
};
