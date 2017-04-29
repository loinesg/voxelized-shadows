#include "Animation.hpp"

#include <math.h>

Animation::Animation(MeshInstance* meshInstance, float startTime, float resetInterval, Vector3 rotationSpeed, Vector3 translationSpeed)
    : meshInstance_(meshInstance),
    startTime_(startTime),
    resetInterval_(resetInterval),
    nextResetTime_(resetInterval),
    rotationSpeed_(rotationSpeed),
    translationSpeed_(translationSpeed)
{
    // Animated mesh instances are made non-static
    meshInstance->makeNonStatic();
    
    // Store the original rotation and position
    originalRotation_ = meshInstance_->rotation();
    originalPosition_ = meshInstance_->position();
    currentPosition_ = originalPosition_;
    
    // Get the axis in world space
    xAxis_ = meshInstance_->right().vec3();
    yAxis_ = meshInstance_->up().vec3();
    zAxis_ = meshInstance_->forward().vec3();
}

void Animation::update(float deltaTime)
{
    // Check if the animation has started yet
    startTime_ -= deltaTime;
    if(startTime_ > 0.0)
    {
        // Animation hasnt started yet
        return;
    }
    
    // Check if the animation should be reset
    nextResetTime_ -= deltaTime;
    if(resetInterval_ > 0.0 && nextResetTime_ < 0.0)
    {
        currentRotation_ = Vector3::zero();
        currentPosition_ = originalPosition_;
        nextResetTime_ = resetInterval_;
    }
    
    // Get the rotation angle along each axis
    float rotationX = rotationSpeed_.x * deltaTime;
    float rotationY = rotationSpeed_.y * deltaTime;
    float rotationZ = rotationSpeed_.z * deltaTime;
    
    // Add the new rotation.
    // Wrap around at 360 degrees so that the
    currentRotation_.x = fmod(currentRotation_.x + rotationX, 360.0);
    currentRotation_.y = fmod(currentRotation_.y + rotationY, 360.0);
    currentRotation_.z = fmod(currentRotation_.z + rotationZ, 360.0);
    
    // Rotate along each axis in turn
    meshInstance_->setRotation(originalRotation_);
    meshInstance_->rotate(currentRotation_.x, xAxis_);
    meshInstance_->rotate(currentRotation_.y, yAxis_);
    meshInstance_->rotate(currentRotation_.z, zAxis_);
    
    // Apply translation
    currentPosition_ = currentPosition_ + (translationSpeed_ * deltaTime);
    meshInstance_->setPosition(currentPosition_);
}
