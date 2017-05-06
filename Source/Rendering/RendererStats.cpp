#include "RendererStats.hpp"

RendererStats::RendererStats()
    : timer_(),
    avgFrameRate_(-1),
    avgFrameTime_(-1),
    avgShadowRenderingTime_(-1),
    avgShadowSamplingTime_(-1),
    samplesCount_(0),
    sampleStartTime_(0),
    shadowRenderingTime_(0),
    shadowSamplingTime_(0)
{
    // Start the frame time timer
    timer_.start();
    
    // Enable support for opengl 3.3 features
    initializeOpenGLFunctions();
    
    // Create the query objects
    glGenQueries(4, queries_);
}

RendererStats::~RendererStats()
{
    glDeleteQueries(4, queries_);
}

void RendererStats::frameStarted()
{
    // Add to the frame count
    samplesCount_ ++;
    
    // Get the times from the previous frame
    uint64_t renderingStart, renderingEnd, samplingStart, samplingEnd;
    glGetQueryObjectui64v(queries_[0], GL_QUERY_RESULT, &renderingStart);
    glGetQueryObjectui64v(queries_[1], GL_QUERY_RESULT, &renderingEnd);
    glGetQueryObjectui64v(queries_[2], GL_QUERY_RESULT, &samplingStart);
    glGetQueryObjectui64v(queries_[3], GL_QUERY_RESULT, &samplingEnd);
    
    // Add the rendering / sampling times to the total
    shadowRenderingTime_ += (renderingEnd - renderingStart);
    shadowSamplingTime_ += (samplingEnd - samplingStart);
    
    // Check if enough frames have been recorded to create new averages
    if(samplesCount_ > 200)
    {
        // Create the new averages
        quint64 time = timer_.elapsed() - sampleStartTime_;
        avgFrameRate_ = samplesCount_ / (time / 1000.0);
        avgFrameTime_ = time / (double)samplesCount_;
        avgShadowRenderingTime_ = shadowRenderingTime_ / (double)samplesCount_;
        avgShadowSamplingTime_ = shadowSamplingTime_ / (double)samplesCount_;
        
        // The rendering and sampling times are in nanoseconds
        avgShadowRenderingTime_ /= 1000000.0;
        avgShadowSamplingTime_ /= 1000000.0;
        
        // Reset the samples
        samplesCount_ = 0;
        sampleStartTime_ = timer_.elapsed();
        shadowRenderingTime_ = 0;
        shadowSamplingTime_ = 0;
    }
}

void RendererStats::shadowRenderingStarted()
{
    // Request the GPU timestamp at this point
    glQueryCounter(queries_[0], GL_TIMESTAMP);
}

void RendererStats::shadowRenderingFinished()
{
    // Request the GPU timestamp at this point
    glQueryCounter(queries_[1], GL_TIMESTAMP);
}

void RendererStats::shadowSamplingStarted()
{
    // Request the GPU timestamp at this point
    glQueryCounter(queries_[2], GL_TIMESTAMP);
}

void RendererStats::shadowSamplingFinished()
{
    // Request the GPU timestamp at this point
    glQueryCounter(queries_[3], GL_TIMESTAMP);
}
