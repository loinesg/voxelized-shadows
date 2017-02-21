#pragma once

#include <iostream>

using namespace std;

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Quaternion.hpp"

struct Matrix4x4
{
    // Elements stored in column-major order
    float elements[16];

public:
    Matrix4x4();
    
    float get(int row, int column) const;
    
    void set(int row, int column, float value);
    void setRow(int row, float* values);
    void setCol(int col, float* values);

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
