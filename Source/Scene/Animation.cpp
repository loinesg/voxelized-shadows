#include "Animation.hpp"

#include <math.h>

Animation::Animation(MeshInstance* meshInstance, Vector3 rotationSpeed)
    : meshInstance_(meshInstance),
    rotationSpeed_(rotationSpeed)
{
    // Animated mesh instances are made non-static
    meshInstance->makeNonStatic();
    
    // Store the original rotation
    originalRotation_ = meshInstance_->rotation();
    
    // Get the axis in world space
    xAxis_ = meshInstance_->right().vec3();
    yAxis_ = meshInstance_->up().vec3();
    zAxis_ = meshInstance_->forward().vec3();
}

void Animation::update(float deltaTime)
{
    // Get the rotation angle along each axis
    float rotationX = rotationSpeed_.x * deltaTime;
    float rotationY = rotationSpeed_.y * deltaTime;
    float rotationZ = rotationSpeed_.z * deltaTime;
    
    // Add the new rotation.
    // Wrap around at 360 degrees so that the
    totalRotation_.x = fmod(totalRotation_.x + rotationX, 360.0);
    totalRotation_.y = fmod(totalRotation_.y + rotationY, 360.0);
    totalRotation_.z = fmod(totalRotation_.z + rotationZ, 360.0);
    
    // Rotate along each axis in turn
    meshInstance_->setRotation(originalRotation_);
    meshInstance_->rotate(totalRotation_.x, xAxis_);
    meshInstance_->rotate(totalRotation_.y, yAxis_);
    meshInstance_->rotate(totalRotation_.z, zAxis_);
}
