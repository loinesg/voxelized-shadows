#pragma once

#include "Vector3.hpp"
#include "Vector4.hpp"

class Bounds
{
public:
    Bounds(const Vector3 &min, const Vector3 &max);
    
    // Covered region
    Vector3 min() const { return min_; }
    Vector3 max() const { return max_; }
    Vector3 centre() const;
    
    // Size
    Vector3 size() const;
    
    // Expands the bounds to contain the given point
    void expandToCover(const Vector3 &point);
    
    // Creates a Bounds instance covering the given points
    static Bounds cover(const Vector4* points, int count);
    
private:
    Vector3 min_;
    Vector3 max_;
};
