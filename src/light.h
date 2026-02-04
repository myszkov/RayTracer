#pragma once
/*
 * light.h - Lighting Calculations
 *
 * Defines a point light source (position and intensity/color).
 *
 * calculate_diffuse() implements Lambertian diffuse shading - surfaces are
 * brighter when facing the light directly, darker at grazing angles.
 * Includes ambient lighting so shadowed areas aren't completely black.
 * The is_shadowed parameter allows shadow rays to block direct lighting.
 */
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
// is_shadowed - whether this point is blocked from the light
inline color calculate_diffuse(const vec3& hit_point, const vec3& normal,
    const point_light& light, const color& obj_color,
    bool is_shadowed) {
    vec3 light_dir = unit_vector(light.position - hit_point);
    double diffuse_amount = std::max(0.0, dot(normal, light_dir));

    double ambient = 0.1;

    // If in shadow, only use ambient lighting
    if (is_shadowed) {
        return obj_color * ambient * light.intensity;
    }

    return obj_color * (ambient + diffuse_amount * (1.0 - ambient)) * light.intensity;
}