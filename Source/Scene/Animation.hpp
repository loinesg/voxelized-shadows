#pragma once

#include "Vector3.hpp"
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
    
    MeshInstance* meshInstance_;
    Vector3 rotationSpeed_;
};
