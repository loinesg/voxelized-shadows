#pragma once

#include <QGLWidget>

using namespace std;

class Texture
{
public:
    Texture(GLuint id, int width, int height);
    ~Texture();
    
    GLuint id() const { return id_; }
    int width() const { return width_; }
    int height() const { return height_; }
    
    void bind();
    
    static Texture* load(const char* fileName);
    
private:
    GLuint id_;
    int width_;
    int height_;
};
