#pragma once

enum InputKey
{
    IK_None = 0,
    IK_MoveForward = 1,
    IK_MoveBackwards = 2,
    IK_MoveLeft = 4,
    IK_MoveRight = 8,
    IK_MoveUp = 16,
    IK_MoveDown = 32,
    IK_MoveFast = 64
};

class InputManager
{
public:
    InputManager();
    
    // Key states
    bool isKeyUp(InputKey key) const;
    bool isKeyDown(InputKey key) const;
    bool anyKeyDown() const;
    
    // Movement axis
    float getForwardsMovement() const;
    float getSidewaysMovement() const;
    float getVerticalMovement() const;
    float getAxisMovement(InputKey positiveDir, InputKey negativeDir) const;
    
    void keyPressed(InputKey key);
    void keyReleased(InputKey key);
    
private:
    unsigned int pressedKeys_;
};
