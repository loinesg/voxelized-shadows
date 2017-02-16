#pragma once

#include <iostream>

using namespace std;

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Quaternion.hpp"

struct Matrix4x4
{
    float elements[16];

public:
    Matrix4x4();

    // Allows use of matrix[i][j] syntax
    float* operator[] (int i) { return elements + (i * 4); }
    const float* operator[] (int i) const { return elements + (i * 4); }

    static Matrix4x4 identity();
    static Matrix4x4 translation(const Vector3 &t);
    static Matrix4x4 rotation(const Quaternion &q);
    static Matrix4x4 scale(const Vector3 &s);

    // Constructs a matrix for performing a scale, then a rotation, then a translation.
    static Matrix4x4 trs(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale);

    // Constructs the inverse of a trs matrix for the given scale, rotation and translation.
    static Matrix4x4 trsInverse(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale);

    // Constructs an orthographic projection matrix
    static Matrix4x4 orthographic(float l, float r, float b, float t, float n, float f);
    
    // Constructs a perspective projection matrix
    static Matrix4x4 perspective(float fov, float aspect, float n, float f);
};

Matrix4x4 operator * (const Matrix4x4 &mat, float scalar);
Matrix4x4 operator * (float scalar, const Matrix4x4 &mat);

Matrix4x4 operator * (const Matrix4x4 &a, const Matrix4x4 &b);
Matrix4x4 operator + (const Matrix4x4 &a, const Matrix4x4 &b);
Matrix4x4 operator - (const Matrix4x4 &a, const Matrix4x4 &b);

Vector4 operator * (const Matrix4x4 &mat, const Vector4 &v);

ostream& operator << (ostream &os, Matrix4x4 &mat);
istream& operator >> (istream &is, Matrix4x4 &mat);
