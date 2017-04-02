#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "MeshInstance.hpp"

class Animation
{
public:
    Animation(MeshInstance* meshInstance, Vector3 rotationSpeed);
    
    // The mesh instance being animated
    MeshInstance* meshInstance() const { return meshInstance_; }
    
    // The rotation, in local space degrees per second
    Vector3 rotationSpeed() const { return rotationSpeed_; }
    
    // Applies the rotation for the given delta time
    void update(float deltaTime);
    
private:
    
    // The animation target
    MeshInstance* meshInstance_;
    
    // The rotation rate, in degrees per second, around each axis
    Vector3 rotationSpeed_;
    
    // The original rotation of the target object.
    // The rotation is applied from here each frame to avoid
    // floating point error accumulation.
    Quaternion originalRotation_;
    
    // The amount, in degrees, that the object has been rotated
    Vector3 totalRotation_;
    
    // The world space axis to rotate around
    Vector3 xAxis_;
    Vector3 yAxis_;
    Vector3 zAxis_;
};
