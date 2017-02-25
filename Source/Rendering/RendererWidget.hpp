#pragma once

#include <QGLWidget>

#include "Scene.hpp"
#include "RenderPass.hpp"
#include "UniformManager.hpp"

class RendererWidget : public QGLWidget
{
public:
    RendererWidget(const QGLFormat &format);
    ~RendererWidget();
    
    Scene* scene() { return scene_; }
    UniformManager* uniformManager() { return uniformManager_; }
    
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    
    Camera* camera() const { return scene_->mainCamera(); }

private:
    Scene* scene_;
    UniformManager* uniformManager_;
    RenderPass* forwardPass_;

    // QGLWidget override methods
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    
    // Setup
    void createRenderPasses();
    void createScene();
    
    // Render passes
    void renderForward();
    
    // Applies camera properties to the opengl state
    void useCamera(Camera* camera);
};
