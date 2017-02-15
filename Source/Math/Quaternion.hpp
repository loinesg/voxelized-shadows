#pragma once

#include <iostream>

using namespace std;

#include "Vector3.hpp"

struct Quaternion
{
    Vector3 v;
    float w;

    Quaternion();
    Quaternion(const Vector3 &v, float w);
    Quaternion(float x, float y, float z, float w);

    float sqrNorm() const;
    float norm() const;
    Quaternion conjugate() const;

    // Creates a rotation for the specified angle (degrees) around the given axis
    static Quaternion rotation(float angle, const Vector3 &axis);

    // Creates a rotation of z degrees around the z axis, followed by y degrees
    // around the y axis, and then x degrees around the x axis.
    static Quaternion euler(float x, float y, float z);
    static Quaternion euler(Vector3 &euler);
};

Quaternion operator * (const Quaternion &a, const Quaternion &b);
Quaternion operator + (const Quaternion &a, const Quaternion &b);

ostream& operator << (ostream &os, Quaternion &quat);
istream& operator >> (istream &is, Quaternion &quat);
