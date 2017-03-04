#include "Bounds.hpp"

#include <assert.h>
#include <math.h>

Bounds::Bounds(const Vector3 &min, const Vector3 &max)
    : min_(min),
    max_(max)
{
    assert(min_.x <= max_.x);
    assert(min_.y <= max_.y);
    assert(min_.z <= max_.z);
}

Vector3 Bounds::centre() const
{
    return (min_ + max_) * 0.5;
}

Vector3 Bounds::size() const
{
    return max_ - min_;
}

void Bounds::expandToCover(const Vector3 &point)
{
    // Reduce min if needed
    min_.x = std::min(min_.x, point.x);
    min_.y = std::min(min_.y, point.y);
    min_.z = std::min(min_.z, point.z);
    
    // Increase max if needed
    max_.x = std::max(max_.x, point.x);
    max_.y = std::max(max_.y, point.y);
    max_.z = std::max(max_.z, point.z);
}

Bounds Bounds::cover(const Vector4* points, int count)
{
    assert(count > 0);
    
    // Cover the first point initially
    Bounds b(points[0].vec3(), points[0].vec3());
    
    // Expand to cover each additional point
    for(int i = 1; i < count; ++i)
    {
        b.expandToCover(points[i].vec3());
    }
    
    return b;
}
