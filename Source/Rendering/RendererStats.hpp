#pragma once

#include <QGLWidget>
#include <QElapsedTimer>

class RendererStats
{
public:
    RendererStats();
    
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
    
    // Updates the stats with new measurements (in milliseconds)
    // The stat changes take a few frames to be shown in the averages
    void sample(int frameTime, int shadowRenderingTime, int shadowSamplingTime);
    
private:
    
    // The timer used for measuring rendering times
    QElapsedTimer timer_;
    
    // The last set of samples
    // These values are the ones currently displayed
    double avgFrameRate_;
    double avgFrameTime_;
    double avgShadowRenderingTime_;
    double avgShadowSamplingTime_;
    
    // The samples being gathered
    int samplesCount_;
    qint64 sampleStartTime_;
    qint64 shadowRenderingStartTime_;
    qint64 shadowSamplingStartTime_;
    qint64 shadowRenderingTime_;
    qint64 shadowSamplingTime_;
};
