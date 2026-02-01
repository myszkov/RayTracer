#pragma once
#include "vec3.h"
#include "ray.h"
#include "color.h"

// Stores information about a ray hit
struct hit_record {
    vec3 point;      // Where the ray hit
    vec3 normal;     // Surface normal at that point
    double t;        // Distance along the ray
    color object_color; // The colour of the object that was hit
};

// Any object in the scene inherits from this
class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual ~hittable() = default;
};