#pragma once
#include "vec3.h"
#include "color.h"

struct point_light {
    vec3 position;
    color intensity;

    point_light() {}
    point_light(const vec3& pos, const color& col)
        : position(pos), intensity(col) {
    }
};

// Calculates diffuse lighting at a point on a surface
// hit_point  - where the ray hit the surface
// normal     - the surface normal at that point
// light      - the light source
// obj_color  - the base color of the object
inline color calculate_diffuse(const vec3& hit_point, const vec3& normal,
    const point_light& light, const color& obj_color) {
    // Direction from hit point to light
    vec3 light_dir = unit_vector(light.position - hit_point);

    // How much light hits this surface
    // Clamped to 0 if negative (light is behind the surface)
    double diffuse_amount = std::max(0.0, dot(normal, light_dir));

    // Add a small ambient term so nothing is completely black
    double ambient = 0.1;

    return obj_color * (ambient + diffuse_amount * (1.0 - ambient)) * light.intensity;
}