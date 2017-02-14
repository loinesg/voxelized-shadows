#pragma once

#include <QGLWidget>

class RendererWidget : public QGLWidget
{
public:
    RendererWidget();
    ~RendererWidget();

private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();    
};
