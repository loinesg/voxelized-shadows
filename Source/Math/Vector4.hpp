#pragma once

#include <iostream>

using namespace std;

#include "Vector3.hpp"

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    Vector4();
    Vector4(float x, float y, float z, float w);
    Vector4(const Vector3 &v, float w);
    
    // Convert to Vector3
    Vector3 vec3() const;
};

Vector4 operator / (const Vector4 &v, float scalar);
Vector4 operator / (float scalar, const Vector4 &v);
Vector4 operator * (const Vector4 &v, float scalar);
Vector4 operator * (float scalar, const Vector4 &v);
Vector4 operator + (const Vector4 &a, const Vector4 &b);
Vector4 operator - (const Vector4 &a, const Vector4 &b);

ostream& operator << (ostream &os, Vector4 &vec);
istream& operator >> (istream &is, Vector4 &vec);
