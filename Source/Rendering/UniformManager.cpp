#include "UniformManager.hpp"

UniformManager::UniformManager()
{
    createBuffers();
}

UniformManager::~UniformManager()
{
    glDeleteBuffers(1, &perObjectBlockID_);
}

void UniformManager::updatePerObjectBuffer(const PerObjectUniformBuffer &buffer)
{
    glBindBuffer(GL_UNIFORM_BUFFER, perObjectBlockID_);
    GLvoid* map = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(map, &buffer, sizeof(PerObjectUniformBuffer));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void UniformManager::createBuffers()
{
    glGenBuffers(1, &perObjectBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, perObjectBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, PerObjectUniformBuffer::BlockID, perObjectBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
