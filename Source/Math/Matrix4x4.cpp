#include "Matrix4x4.hpp"

#include <cstring>

Matrix4x4::Matrix4x4()
{
    for(int i = 0; i < 16; ++i)
        elements[i] = 0.0;
}

Matrix4x4 Matrix4x4::identity()
{
    Matrix4x4 mat;
    mat[0][0] = 1.0;
    mat[1][1] = 1.0;
    mat[2][2] = 1.0;
    mat[3][3] = 1.0;

    return mat;
}

Matrix4x4 Matrix4x4::translation(const Vector3 &t)
{
    Matrix4x4 mat = Matrix4x4::identity();
    mat[0][3] = t.x;
    mat[1][3] = t.y;
    mat[2][3] = t.z;

    return mat;
}

Matrix4x4 Matrix4x4::rotation(const Quaternion &q)
{
    float s = 2.0 / q.norm();
    Vector3 v = q.v;

    Matrix4x4 mat = Matrix4x4::identity();
    mat[0][0] = 1 - s * (v.y * v.y + v.z * v.z);
    mat[0][1] = s * (v.x * v.y - q.w * v.z);
    mat[0][2] = s * (v.x * v.z + q.w * v.y);
    mat[1][0] = s * (v.x * v.y + q.w * v.z);
    mat[1][1] = 1 - s * (v.x * v.x + v.z * v.z);
    mat[1][2] = s * (v.y * v.z - q.w * v.x);
    mat[2][0] = s * (v.x * v.z - q.w * v.y);
    mat[2][1] = s * (v.y * v.z + q.w * v.x);
    mat[2][2] = 1 - s * (v.x * v.x + v.y * v.y);

    return mat;
}

Matrix4x4 Matrix4x4::scale(const Vector3 &s)
{
    Matrix4x4 mat = Matrix4x4::identity();
    mat[0][0] = s.x;
    mat[1][1] = s.y;
    mat[2][2] = s.z;

    return mat;
}

Matrix4x4 Matrix4x4::trs(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale)
{
    Matrix4x4 translationMat = Matrix4x4::translation(translation);
    Matrix4x4 rotationMat = Matrix4x4::rotation(rotation);
    Matrix4x4 scaleMat = Matrix4x4::scale(scale);

    // Scale, then rotation, then translation
    return translationMat * rotationMat * scaleMat;
}

Matrix4x4 operator * (const Matrix4x4 &mat, float scalar)
{
    Matrix4x4 result;

    for(int i = 0; i < 16; ++i)
        result.elements[i] = mat.elements[i] * scalar;

    return result;
}

Matrix4x4 operator * (float scalar, const Matrix4x4 &mat)
{
    return mat * scalar;
}

Matrix4x4 operator * (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            result[i][j] = (a[i][0] * b[0][j])
                         + (a[i][1] * b[1][j])
                         + (a[i][2] * b[2][j])
                         + (a[i][3] * b[3][j]);

    return result;
}

Matrix4x4 operator + (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            result[i][j] = a[i][j] + b[i][j];

    return result;
}

Matrix4x4 operator - (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            result[i][j] = a[i][j] - b[i][j];

    return result;
}

Vector4 operator * (const Matrix4x4 &mat, const Vector4 &v)
{
    Vector4 result;
    result.x = (mat[0][0] * v.x) + (mat[0][1] * v.y) + (mat[0][2] * v.z) + (mat[0][3] * v.w);
    result.y = (mat[1][0] * v.x) + (mat[1][1] * v.y) + (mat[1][2] * v.z) + (mat[1][3] * v.w);
    result.z = (mat[2][0] * v.x) + (mat[2][1] * v.y) + (mat[2][2] * v.z) + (mat[2][3] * v.w);
    result.w = (mat[3][0] * v.x) + (mat[3][1] * v.y) + (mat[3][2] * v.z) + (mat[3][3] * v.w);
    return result;
}

ostream& operator << (ostream &os, Matrix4x4 &mat)
{
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            if(j == 0) cout << "( ";
            else cout << ", ";

            cout << mat[i][j];

            if(j == 3) cout << " )";
        }

        if(i != 3) cout << endl;
    }

    return os;
}

istream& operator >> (istream &is, Matrix4x4 &mat)
{
    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            is >> mat[i][j];

    return is;
}
