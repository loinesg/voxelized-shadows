#include "Vector4.hpp"

#include <math.h>

Vector4::Vector4()
	: x(0.0), y(0.0), z(0.0), w(0.0)
{

}

Vector4::Vector4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{

}

Vector4::Vector4(const Vector3 &v, float w)
    : x(v.x), y(v.y), z(v.z), w(w)
{
    
}

Vector3 Vector4::vec3() const
{
    return Vector3(x, y, z);
}

Vector4 operator / (const Vector4 &v, float scalar)
{
    return Vector4(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
}

Vector4 operator / (float scalar, const Vector4 &v)
{
    return Vector4(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w);
}

Vector4 operator * (const Vector4 &v, float scalar)
{
    return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}

Vector4 operator * (float scalar, const Vector4 &v)
{
    return v * scalar;
}

Vector4 operator + (const Vector4 &a, const Vector4 &b)
{
    return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vector4 operator - (const Vector4 &a, const Vector4 &b)
{
    return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

ostream& operator << (ostream &os, Vector4 &vec)
{
    os << "( " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << " )";
    return os;
}

istream& operator >> (istream &is, Vector4 &vec)
{
    is >> vec.x;
    is >> vec.y;
    is >> vec.z;
    is >> vec.w;

    return is;
}
