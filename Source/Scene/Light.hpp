#pragma once

#include "Vector3.hpp"
#include "Object.hpp"

class Light : public Object
{
public:
    Light(Vector3 color, Vector3 ambient);
    
    Vector3 color() const { return color_; }
    Vector3 ambient() const { return ambient_; }
    
    void setColor(Vector3 color);
    void setAmbient(Vector3 ambient);
    
private:
    Vector3 color_;
    Vector3 ambient_;
};
