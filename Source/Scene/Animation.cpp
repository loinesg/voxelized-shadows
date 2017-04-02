#include "Animation.hpp"

Animation::Animation(MeshInstance* meshInstance, Vector3 rotationSpeed)
    : meshInstance_(meshInstance),
    rotationSpeed_(rotationSpeed)
{
    
}

void Animation::update(float deltaTime)
{
    // Get the rotation angle along each axis
    float rotationX = rotationSpeed_.x * deltaTime;
    float rotationY = rotationSpeed_.y * deltaTime;
    float rotationZ = rotationSpeed_.z * deltaTime;
    
    // Rotate along each axis in turn
    meshInstance_->rotate(rotationX, meshInstance_->right().vec3());
    meshInstance_->rotate(rotationY, meshInstance_->up().vec3());
    meshInstance_->rotate(rotationZ, meshInstance_->forward().vec3());
}
