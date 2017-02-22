#include "Object.hpp"

Object::Object()
    : position_(Vector3::zero()),
    rotation_(Quaternion::identity()),
    scale_(Vector3::one()),
    worldToLocal_(Matrix4x4::identity()),
    localToWorld_(Matrix4x4::identity())
{
    
}

Vector4 Object::forward() const
{
    return Matrix4x4::rotation(rotation_) * Vector4(0.0, 0.0, 1.0, 0.0);
}

void Object::setPosition(const Vector3 &pos)
{
    position_ = pos;
    recreateTransformation();
}

void Object::setRotation(const Quaternion &rot)
{
    rotation_ = rot;
    recreateTransformation();
}

void Object::setScale(const Vector3 &scale)
{
    scale_ = scale;
    recreateTransformation();
}

void Object::recreateTransformation()
{
    localToWorld_ = Matrix4x4::trs(position_, rotation_, scale_);
    worldToLocal_ = Matrix4x4::trsInverse(position_, rotation_, scale_);
}
