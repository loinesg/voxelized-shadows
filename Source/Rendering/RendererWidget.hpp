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
    
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    
    Camera* camera() const { return scene_->mainCamera(); }

private:
    Scene* scene_;
    UniformManager* uniformManager_;
    RenderPass* forwardPass_;

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    
    void createRenderPasses();
    void createScene();
};
