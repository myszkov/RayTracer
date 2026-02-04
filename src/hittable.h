#pragma once
/*
 * hittable.h - Abstract Base Class for Scene Objects
 *
 * Defines the interface that all objects in the scene must implement.
 * The hit() function tests whether a ray intersects the object within
 * a given range and records information about the intersection.
 *
 * hit_record stores: intersection point, surface normal, distance along ray,
 * and a pointer to the object's material.
 */
#include "vec3.h"
#include "ray.h"
#include "color.h"

class material;  // Forward declaration

// Stores information about a ray hit
struct hit_record {
    vec3 point;      // Where the ray hit
    vec3 normal;     // Surface normal at that point
    double t;        // Distance along the ray
    const material* mat_ptr;  // The material of the object that was hit
};

// Any object in the scene inherits from this
class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual ~hittable() = default;
};