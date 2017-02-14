#include "Vector3.hpp"

#include <math.h>

Vector3::Vector3()
	: x(0.0), y(0.0), z(0.0)
{

}

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z)
{

}

float Vector3::maxComponent() const
{
    return std::max(x, std::max(y, z));
}

float Vector3::sqrMagnitude() const
{
	return dot(*this, *this);
}

float Vector3::magnitude() const
{
	return (float)sqrt(sqrMagnitude());
}

Vector3 Vector3::normalized() const
{
    float len = magnitude();
	return Vector3(x / len, y / len, z / len);
}

Vector3 Vector3::operator * (float scalar) const
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

float Vector3::dot(const Vector3 &a, const Vector3 &b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vector3 Vector3::cross(const Vector3 &a, const Vector3 &b)
{
    float x = (a.y * b.z) - (a.z * b.y);
    float y = (a.z * b.x) - (a.x * b.z);
    float z = (a.x * b.y) - (a.y * b.x);

    return Vector3(x, y, z);
}

Vector3 operator + (const Vector3 &a, const Vector3 &b)
{
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator - (const Vector3 &a, const Vector3 &b)
{
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

istream& operator >> (istream &is, Vector3 &vec)
{
    is >> vec.x;
    is >> vec.y;
    is >> vec.z;

    return is;
}
