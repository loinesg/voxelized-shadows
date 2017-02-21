#include "Matrix4x4.hpp"

#include <math.h>

Matrix4x4::Matrix4x4()
{
    for(int i = 0; i < 16; ++i)
        elements[i] = 0.0;
}

float Matrix4x4::get(int row, int column) const
{
    return elements[(column * 4) + row];
}

void Matrix4x4::set(int row, int column, float value)
{
    elements[(column * 4) + row] = value;
}

void Matrix4x4::setRow(int row, float *values)
{
    elements[row] = values[0];
    elements[row + 4] = values[1];
    elements[row + 8] = values[2];
    elements[row + 12] = values[3];
}

void Matrix4x4::setCol(int col, float *values)
{
    elements[col*4] = values[0];
    elements[col*4 + 1] = values[1];
    elements[col*4 + 2] = values[2];
    elements[col*4 + 3] = values[3];
}

Matrix4x4 Matrix4x4::identity()
{
    Matrix4x4 mat;
    mat.set(0, 0, 1.0);
    mat.set(1, 1, 1.0);
    mat.set(2, 2, 1.0);
    mat.set(3, 3, 1.0);

    return mat;
}

Matrix4x4 Matrix4x4::translation(const Vector3 &t)
{
    Matrix4x4 mat = Matrix4x4::identity();
    mat.set(0, 3, t.x);
    mat.set(1, 3, t.y);
    mat.set(2, 3, t.z);

    return mat;
}

Matrix4x4 Matrix4x4::rotation(const Quaternion &q)
{
    float s = 2.0 / q.norm();
    Vector3 v = q.v;

    Matrix4x4 mat = Matrix4x4::identity();
    mat.set(0, 0, 1 - s * (v.y * v.y + v.z * v.z));
    mat.set(0, 1, s * (v.x * v.y - q.w * v.z));
    mat.set(0, 2, s * (v.x * v.z + q.w * v.y));
    mat.set(1, 0, s * (v.x * v.y + q.w * v.z));
    mat.set(1, 1, 1 - s * (v.x * v.x + v.z * v.z));
    mat.set(1, 2, s * (v.y * v.z - q.w * v.x));
    mat.set(2, 0, s * (v.x * v.z - q.w * v.y));
    mat.set(2, 1, s * (v.y * v.z + q.w * v.x));
    mat.set(2, 2, 1 - s * (v.x * v.x + v.y * v.y));

    return mat;
}

Matrix4x4 Matrix4x4::scale(const Vector3 &s)
{
    Matrix4x4 mat = Matrix4x4::identity();
    mat.set(0, 0, s.x);
    mat.set(1, 1, s.y);
    mat.set(2, 2, s.z);

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
    Matrix4x4 mat = Matrix4x4::identity();

    // translation
    mat.set(0, 3, - (r + l) / (r - l));
    mat.set(1, 3, - (t + b) / (t - b));
    mat.set(2, 3, - (f + n) / (f - n));

    // scale
    mat.set(0, 0, 2.0 / (r - l));
    mat.set(1, 1, 2.0 / (t - b));
    mat.set(2, 2, 2.0 / (f - n));

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
    mat.set(0, 0, (2.0 * n) / (r - l));
    mat.set(1, 1, (2.0 * n) / (t - b));
    mat.set(2, 2, (f + n) / (f - n));
    mat.set(3, 3, 0.0);
    mat.set(0, 2, -(r + l) / (r - l));
    mat.set(1, 2, -(t + b) / (t - b));
    mat.set(3, 2, 1.0);
    mat.set(2, 3, -(2.0 * f * n) / (f - n));
    
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
            result.set(i, j, a.get(i, 0) * b.get(0, j)
                           + a.get(i, 1) * b.get(1, j)
                           + a.get(i, 2) * b.get(2, j)
                           + a.get(i, 3) * b.get(3, j));

    return result;
}

Matrix4x4 operator + (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;

    for(int i = 0; i < 16; ++i)
        result.elements[i] = a.elements[i] + b.elements[i];
    
    return result;
}

Matrix4x4 operator - (const Matrix4x4 &a, const Matrix4x4 &b)
{
    Matrix4x4 result;
    
    for(int i = 0; i < 16; ++i)
        result.elements[i] = a.elements[i] - b.elements[i];
    
    return result;
}

Vector4 operator * (const Matrix4x4 &mat, const Vector4 &v)
{
    Vector4 result;
    result.x = (mat.get(0, 0) * v.x) + (mat.get(0, 1) * v.y) + (mat.get(0, 2) * v.z) + (mat.get(0, 3) * v.w);
    result.y = (mat.get(1, 0) * v.x) + (mat.get(1, 1) * v.y) + (mat.get(1, 2) * v.z) + (mat.get(1, 3) * v.w);
    result.z = (mat.get(2, 0) * v.x) + (mat.get(2, 1) * v.y) + (mat.get(2, 2) * v.z) + (mat.get(2, 3) * v.w);
    result.w = (mat.get(3, 0) * v.x) + (mat.get(3, 1) * v.y) + (mat.get(3, 2) * v.z) + (mat.get(3, 3) * v.w);
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

            cout << mat.get(i, j);

            if(j == 3) cout << " )";
        }

        if(i != 3) cout << endl;
    }

    return os;
}
