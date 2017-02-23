#pragma once

#include <QEvent>
#include <QKeyEvent>

#include "MainWindow.hpp"
#include "Input.hpp"
#include "Vector2.hpp"

class MainWindowController : public QObject
{
    Q_OBJECT

public:
    MainWindowController(MainWindow* window);
    ~MainWindowController();

protected:
    
    // Intercepts events for the renderer widget
    bool eventFilter(QObject* obj, QEvent* event);
    
public slots:
    void textureFeatureToggled(int state);
    void specularFeatureToggled(int state);
    void normalMapsFeatureToggled(int state);
    void cutoutFeatureToggled(int state);

private:
    MainWindow* window_;
    InputManager inputManager_;
    
    bool mouseDragging_;
    Vector2 mousePosition_;
    
    // Called each frame
    void update(float deltaTime);
    void applyCameraMovement();
    
    // Qt event handling
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    
    // Shader features
    void updateShaderFeature(ShaderFeature feature, int state);
};
