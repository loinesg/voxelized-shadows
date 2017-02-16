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
