#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "MeshInstance.hpp"

class Animation
{
public:
    Animation(MeshInstance* meshInstance, float startTime, float resetInterval, Vector3 rotationSpeed, Vector3 translationSpeed);
    
    // The mesh instance being animated
    MeshInstance* meshInstance() const { return meshInstance_; }
    
    // The rotation, in local space degrees per second
    Vector3 rotationSpeed() const { return rotationSpeed_; }
    
    // The translation in world space units per second
    Vector3 translationSpeed() const { return translationSpeed_; }
    
    // Applies the rotation for the given delta time
    void update(float deltaTime);
    
private:
    
    // The animation target
    MeshInstance* meshInstance_;
    
    // The time until the animation starts
    float startTime_;
    
    // The time between each reset
    float resetInterval_;
    
    // The time until the next reset
    float nextResetTime_;
    
    // The rotation rate, in degrees per second, around each axis
    Vector3 rotationSpeed_;
    Vector3 currentRotation_;
    Quaternion originalRotation_;
    
    // The world space axis to rotate around
    Vector3 xAxis_;
    Vector3 yAxis_;
    Vector3 zAxis_;
    
    // The translation rate and original position
    Vector3 translationSpeed_;
    Vector3 currentPosition_;
    Vector3 originalPosition_;
};
