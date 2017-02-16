#include "Matrix4x4.hpp"

#include <math.h>

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

Matrix4x4 Matrix4x4::trsInverse(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale)
{
    Matrix4x4 translationMat = Matrix4x4::translation(-1.0 * translation);
    Matrix4x4 rotationMat = Matrix4x4::rotation(rotation.inverse());
    Matrix4x4 scaleMat = Matrix4x4::scale(1.0 / scale);

    // Opposite order to Matrix4x4::trs.
    // Translation, then rotation, then scale.
    return scaleMat * rotationMat * translationMat;
}

Matrix4x4 Matrix4x4::orthographic(float l, float r, float b, float t, float n, float f)
{
    // Negate n and f so we look down the negative z axis
    n *= -1.0;
    f *= -1.0;

    Matrix4x4 mat = Matrix4x4::identity();

    // translation
    mat[0][3] = - (r + l) / (r - l);
    mat[1][3] = - (t + b) / (t - b);
    mat[2][3] = - (f + n) / (f - n);

    // scale
    mat[0][0] = 2.0 / (r - l);
    mat[1][1] = 2.0 / (t - b);
    mat[2][2] = 2.0 / (f - n);

    return mat;
}

Matrix4x4 Matrix4x4::perspective(float fov, float aspect, float n, float f)
{
    // Compute the size of the image plane
    float halfFov = (fov / 2.0) * (M_PI / 180.0);
    float halfW = n * tan(halfFov);
    float halfH = halfW * aspect;
    
    // Determine the image plane corners
    float l = -halfW;
    float r = halfW;
    float t = halfH;
    float b = -halfH;
    
    // Create the perspective projection matrix
    Matrix4x4 mat = Matrix4x4::identity();
    mat[0][0] = (2.0 * n) / (r - l);
    mat[1][1] = (2.0 * n) / (t - b);
    mat[2][2] = - (f + n) / (f - n);
    mat[3][3] = 0.0;
    mat[0][2] = (r + l) / (r - l);
    mat[1][2] = (r + b) / (t - b);
    mat[3][2] = -1.0;
    mat[2][3] = - (2 * f * n) / (f - n);
    
    return mat;
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
