#pragma once

#include <iostream>

using namespace std;

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    Vector4();
    Vector4(float x, float y, float z, float w);
};

ostream& operator << (ostream &os, Vector4 &vec);
istream& operator >> (istream &is, Vector4 &vec);
