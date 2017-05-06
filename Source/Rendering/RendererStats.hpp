#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <QElapsedTimer>
#include <QOpenGLFunctions_3_3_Core>

class RendererStats : protected QOpenGLFunctions_3_3_Core
{
public:
    RendererStats();
    ~RendererStats();
    
    // Get the averaged results from the last samples
    double currentFrameRate() const { return avgFrameRate_; }
    double currentFrameTime() const { return avgFrameTime_; }
    double currentShadowRenderingTime() const { return avgShadowRenderingTime_; }
    double currentShadowSamplingTime() const { return avgShadowSamplingTime_; }
    
    // These methods are called at certain points in a frame by RendererWidget
    void frameStarted();
    void shadowRenderingStarted();
    void shadowRenderingFinished();
    void shadowSamplingStarted();
    void shadowSamplingFinished();
    
private:
    
    // The timer used for measuring rendering times
    QElapsedTimer timer_;
    
    // The queries used for measuring shadow rendering and sampling times
    GLuint queries_[4];
    
    // The last set of samples
    // These values are the ones currently displayed
    double avgFrameRate_;
    double avgFrameTime_;
    double avgShadowRenderingTime_;
    double avgShadowSamplingTime_;
    
    // The samples being gathered
    int samplesCount_;
    qint64 sampleStartTime_;
    qint64 shadowRenderingTime_;
    qint64 shadowSamplingTime_;
};
