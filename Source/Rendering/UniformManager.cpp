#include "UniformManager.hpp"

UniformManager::UniformManager()
{
    createBuffers();
}

UniformManager::~UniformManager()
{
    glDeleteBuffers(1, &perObjectBlockID_);
    glDeleteBuffers(1, &sceneBlockID_);
    glDeleteBuffers(1, &shadowBlockID_);
}

void UniformManager::updatePerObjectBuffer(const PerObjectUniformBuffer &buffer)
{
    glBindBuffer(GL_UNIFORM_BUFFER, perObjectBlockID_);
    GLvoid* map = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(map, &buffer, sizeof(PerObjectUniformBuffer));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void UniformManager::updateSceneBuffer(const SceneUniformBuffer &buffer)
{
    glBindBuffer(GL_UNIFORM_BUFFER, sceneBlockID_);
    GLvoid* map = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(map, &buffer, sizeof(SceneUniformBuffer));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void UniformManager::updateShadowBuffer(const ShadowUniformBuffer &buffer)
{
    glBindBuffer(GL_UNIFORM_BUFFER, shadowBlockID_);
    GLvoid* map = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(map, &buffer, sizeof(ShadowUniformBuffer));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void UniformManager::createBuffers()
{
    // Per object buffer
    glGenBuffers(1, &perObjectBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, perObjectBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, PerObjectUniformBuffer::BlockID, perObjectBlockID_);
    
    // Scene buffer
    glGenBuffers(1, &sceneBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, sceneBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, SceneUniformBuffer::BlockID, sceneBlockID_);
    
    // Shadow buffer
    glGenBuffers(1, &shadowBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, shadowBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShadowUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, ShadowUniformBuffer::BlockID, shadowBlockID_);
    
    // Unbind
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
