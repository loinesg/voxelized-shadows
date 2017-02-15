#include "Vector2.hpp"

#include <math.h>

Vector2::Vector2()
	: x(0.0), y(0.0)
{

}

Vector2::Vector2(float x, float y)
	: x(x), y(y)
{

}

float Vector2::maxComponent() const
{
    return std::max(x, y);
}

float Vector2::sqrMagnitude() const
{
	return dot(*this, *this);
}

float Vector2::magnitude() const
{
	return (float)sqrt(sqrMagnitude());
}

Vector2 Vector2::normalized() const
{
    float len = magnitude();
	return Vector2(x / len, y / len);
}

Vector2 Vector2::operator * (float scalar) const
{
	return Vector2(x * scalar, y * scalar);
}

float Vector2::dot(const Vector2 &a, const Vector2 &b)
{
	return (a.x * b.x) + (a.y * b.y);
}

Vector2 operator + (const Vector2 &a, const Vector2 &b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator - (const Vector2 &a, const Vector2 &b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

ostream& operator << (ostream &os, Vector2 &vec)
{
    os << "( " << vec.x << ", " << vec.y << ", " << " )";
    return os;
}

istream& operator >> (istream &is, Vector2 &vec)
{
    is >> vec.x;
    is >> vec.y;

    return is;
}
