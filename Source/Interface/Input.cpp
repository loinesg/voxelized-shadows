#include "Input.hpp"

InputManager::InputManager()
    : pressedKeys_(IK_None)
{
    
}

bool InputManager::isKeyUp(InputKey key) const
{
    return !isKeyDown(key);
}

bool InputManager::isKeyDown(InputKey key) const
{
    return (pressedKeys_ & key) != 0;
}

bool InputManager::anyKeyDown() const
{
    return pressedKeys_ != 0;
}

float InputManager::getForwardsMovement() const
{
    return getAxisMovement(IK_MoveForward, IK_MoveBackwards);
}

float InputManager::getSidewaysMovement() const
{
    return getAxisMovement(IK_MoveRight, IK_MoveLeft);
}

float InputManager::getVerticalMovement() const
{
    return getAxisMovement(IK_MoveUp, IK_MoveDown);
}

float InputManager::getAxisMovement(InputKey positiveDir, InputKey negativeDir) const
{
    float val = 0.0;
    if (isKeyDown(positiveDir)) val += 1.0;
    if (isKeyDown(negativeDir)) val -= 1.0;
    
    return val;
}

void InputManager::keyPressed(InputKey key)
{
    pressedKeys_ |= key;
}

void InputManager::keyReleased(InputKey key)
{
    pressedKeys_ &= ~key;
}
