#include "Light.hpp"

Light::Light(Vector3 color, Vector3 ambient)
    : color_(color),
    ambient_(ambient)
{
    
}

void Light::setColor(Vector3 color)
{
    color_ = color;
}

void Light::setAmbient(Vector3 ambient)
{
    ambient_ = ambient;
}
