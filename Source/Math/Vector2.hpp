#pragma once

#include <iostream>

using namespace std;

struct Vector2
{
    float x;
    float y;

    Vector2();
    Vector2(float x, float y);

    float maxComponent() const;
    float sqrMagnitude() const;
    float magnitude() const;

    Vector2 normalized() const;
    Vector2 operator* (float scalar) const;
};

Vector2 operator+ (const Vector2 &a, const Vector2 &b);
Vector2 operator- (const Vector2 &a, const Vector2 &b);

istream& operator >> (istream &is, Vector2 &vec);
