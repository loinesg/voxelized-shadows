#include "RendererWidget.hpp"

#include <assert.h>
#include <cstdio>

RendererWidget::RendererWidget()
{

}

RendererWidget::~RendererWidget()
{

}

void RendererWidget::initializeGL()
{
    printf("Initializing OpenGL %s \n", glGetString(GL_VERSION));

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);
}

void RendererWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void RendererWidget::paintGL()
{
    // Clear Screen
    glClearColor(0.5, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}
