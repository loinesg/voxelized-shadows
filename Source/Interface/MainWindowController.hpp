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
    
protected:
    
    // Intercepts events for the renderer widget
    bool eventFilter(QObject* obj, QEvent* event);
    
public slots:
    
    // Triggered when a shader feature is toggled via a checkbox
    // The sender is a FeatureToggle widget.
    void shaderFeatureToggled();
    
    // Shadow method radio buttons
    void shadowMappingMethodToggled();
    void voxelTreeMethodToggled();
    
    // Debug overlay radio buttons
    void noOverlayToggled();
    void shadowMapOverlayToggled();
    void sceneDepthOverlayToggled();
    void shadowMaskOverlayToggled();
    void cascadeSplitsOverlayToggled();
    void projectedShadowMapOverlayToggled();
    void voxelTreeDepthOverlayToggled();
    
    // Shadow map resoluton radio buttons
    void shadowResolution512Toggled();
    void shadowResolution1024Toggled();
    void shadowResolution2048Toggled();
    void shadowResolution4096Toggled();
    
    // Shadow map cascades radio buttons
    void shadowCascades1Toggled();
    void shadowCascades2Toggled();
    void shadowCascades3Toggled();
    void shadowCascades4Toggled();

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
};
