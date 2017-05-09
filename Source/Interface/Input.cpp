#include "Input.hpp"

InputManager::InputManager()
    : pressedKeys_()
{
    
}

bool InputManager::isKeyUp(InputKey key) const
{
    return !isKeyDown(key);
}

bool InputManager::isKeyDown(InputKey key) const
{
    return find(pressedKeys_.begin(), pressedKeys_.end(), key) != pressedKeys_.end();
}

bool InputManager::anyKeyDown() const
{
    return pressedKeys_.size() > 0;
}

float InputManager::getForwardsMovement() const
{
    return getAxisMovement(InputKey::Key_W, InputKey::Key_S);
}

float InputManager::getSidewaysMovement() const
{
    return getAxisMovement(InputKey::Key_D, InputKey::Key_A);
}

float InputManager::getVerticalMovement() const
{
    return getAxisMovement(InputKey::Key_E, InputKey::Key_Q);
}

float InputManager::getMovementSpeed() const
{
    return isKeyDown(InputKey::Key_Shift) ? 20.0 : 5.0;
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
    if(isKeyUp(key))
    {
        pressedKeys_.push_back(key);
    }
}

void InputManager::keyReleased(InputKey key)
{
    // See if the key is inside the pressed keys vector
    auto pos = find(pressedKeys_.begin(), pressedKeys_.end(), key);
    
    // Remove the key if it was found
    if(pos != pressedKeys_.end())
    {
        pressedKeys_.erase(pos);
    }
}
