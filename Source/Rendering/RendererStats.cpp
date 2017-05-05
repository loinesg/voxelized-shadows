#include "RendererStats.hpp"

RendererStats::RendererStats()
    : timer_(),
    avgFrameRate_(-1),
    avgFrameTime_(-1),
    avgShadowRenderingTime_(-1),
    avgShadowSamplingTime_(-1),
    samplesCount_(0),
    sampleStartTime_(0),
    shadowRenderingStartTime_(0),
    shadowSamplingStartTime_(0),
    shadowRenderingTime_(0),
    shadowSamplingTime_(0)
{
    timer_.start();
}

void RendererStats::frameStarted()
{
    // Add to the frame count
    samplesCount_ ++;
    
    // Check if enough frames have been recorded to create new averages
    if(samplesCount_ > 20)
    {
        // Create the new averages
        quint64 time = timer_.elapsed() - sampleStartTime_;
        avgFrameRate_ = samplesCount_ / (time / 1000.0);
        avgFrameTime_ = time / (double)samplesCount_;
        avgShadowRenderingTime_ = shadowRenderingTime_ / (double)samplesCount_;
        avgShadowSamplingTime_ = shadowSamplingTime_ / (double)samplesCount_;
        
        // Reset the samples
        samplesCount_ = 0;
        sampleStartTime_ = timer_.elapsed();
        shadowRenderingTime_ = 0;
        shadowSamplingTime_ = 0;
    }
}

void RendererStats::shadowRenderingStarted()
{
    // Flush existing commands
    glFinish();
    
    // Record the current time
    shadowRenderingStartTime_ = timer_.elapsed();
}

void RendererStats::shadowRenderingFinished()
{
    // Flush existing commands
    glFinish();
    
    // Add the interval to the stats
    qint64 interval = timer_.elapsed() - shadowRenderingStartTime_;
    shadowRenderingTime_ += interval;
}

void RendererStats::shadowSamplingStarted()
{
    // Flush existing commands
    glFinish();
    
    // Record the current time
    shadowSamplingStartTime_ = timer_.elapsed();
}

void RendererStats::shadowSamplingFinished()
{
    // Flush existing commands
    glFinish();
    
    // Add the interval to the stats
    qint64 interval = timer_.elapsed() - shadowSamplingStartTime_;
    shadowSamplingTime_ += interval;
}
