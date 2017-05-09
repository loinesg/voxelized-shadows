#pragma once

#include <QEvent>
#include <QKeyEvent>

#include <vector>

using namespace std;

// Use Qt keys for input keys
typedef Qt::Key InputKey;

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
    float getMovementSpeed() const;
    
    // Updates key press state
    void keyPressed(InputKey key);
    void keyReleased(InputKey key);
    
private:
    vector<InputKey> pressedKeys_;
    
    float getAxisMovement(InputKey positiveDir, InputKey negativeDir) const;
};
