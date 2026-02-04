#pragma once
/*
 * ray.h - Ray Representation
 *
 * Defines a ray as an origin point and a direction vector.
 * Rays are cast from the camera through each pixel to determine what objects
 * are visible. The at(t) function returns points along the ray, used for
 * intersection testing and finding hit points.
 */
#include "vec3.h"


class ray {
public:
    vec3 orig;
    vec3 dir;

    ray() {}
    ray(const vec3& origin, const vec3& direction)
        : orig(origin), dir(direction) {
    }

    vec3 origin() const { return orig; }
    vec3 direction() const { return dir; }

    // Returns the point at distance t along the ray
    vec3 at(double t) const {
        return orig + t * dir;
    }
};