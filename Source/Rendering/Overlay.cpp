#include "Overlay.hpp"

Overlay::Overlay(const string &name, const string &shaderName, ShaderFeatureList shaderFeatures)
    : name_(name),
    fullScreen_(true),
    useBlending_(false)
{
    shader_ = new Shader(shaderName, shaderFeatures);
    mesh_ = Mesh::fullScreenQuad();
}

Overlay::~Overlay()
{
    delete shader_;
    delete mesh_;
}

void Overlay::setFullScreen(bool fullscreen)
{
    fullScreen_ = fullscreen;
}

void Overlay::setUseBlending(bool blending)
{
    useBlending_ = blending;
}

void Overlay::setTexture(Texture* texture)
{
    texture_ = texture;
}

void Overlay::draw(const Camera* camera)
{
    if(useBlending_)
    {
        // Use blending, if set
        glEnable(GL_BLEND);
        
        // Blending controlled by overlay shader alpha
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    // Write colour only to the default framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthMask(false);
    glColorMask(true, true, true, true);
    
    // Disable depth testing
    glDisable(GL_DEPTH_TEST);
    
    if(fullScreen_)
    {
        // Draw to the entire screen
        glViewport(0, 0, camera->pixelWidth(), camera->pixelHeight());
    }
    else
    {
        int padding = 10;
        int size = 300;
        
#ifdef __APPLE__
        // Display scaling - Use larger padding and size
        padding *= 2;
        size *= 2;
#endif
        
        // Determine texture width and height
        float aspect = texture_->width() / (float)texture_->height();
        float height = size;
        float width = height * aspect;
        
        // Scale the image if too wide.
        if(width > 800.0)
        {
            // Scale down the height and width
            float scale = (800.0 / width);
            height *= scale;
            width *= scale;
        }
        
        // Draw to the overlay's section of the screen only
        glViewport(camera->pixelWidth() - width - padding,
                   camera->pixelHeight() - height - padding,
                   width, height);
    }
    
    // Use quad mesh and overlay shader
    mesh_->bind();
    shader_->bind();
    texture_->bind(GL_TEXTURE0);
    
    // Draw the quad mesh
    glDrawElements(GL_TRIANGLES, mesh_->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    
    // Clean up blending changes
    glDisable(GL_BLEND);
}
