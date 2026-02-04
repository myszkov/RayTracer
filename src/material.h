#pragma once
/*
 * material.h - Material System
 *
 * Defines different material types that determine how surfaces interact with light.
 * Each material calculates how an incoming ray should scatter/reflect.
 *
 * Material types:
 * - lambertian: Matte/diffuse surfaces that scatter light evenly
 * - metal: Reflective surfaces that mirror incoming rays
 *
 * Materials now use textures to determine surface colour.
 */
#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "texture.h"

 // Reflects a vector v around a normal n
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

class material {
public:
    virtual ~material() = default;

    // Returns the material's colour at a given point
    virtual color get_color(const vec3& p) const = 0;

    // Returns true if the material reflects rays (like metal)
    // If true, also returns the reflected ray direction in scattered
    virtual bool scatter(const ray& r_in, const vec3& hit_point, const vec3& normal, vec3& scattered) const = 0;
};

class lambertian : public material {
public:
    const texture* albedo;

    lambertian(const texture* a) : albedo(a) {}

    color get_color(const vec3& p) const override {
        return albedo->value(p);
    }

    bool scatter(const ray& r_in, const vec3& hit_point, const vec3& normal, vec3& scattered) const override {
        // Lambertian materials don't reflect rays in a mirror-like way
        return false;
    }
};

class metal : public material {
public:
    const texture* albedo;
    double fuzz;  // 0 = perfect mirror, higher = more scattered reflections

    metal(const texture* a, double f = 0.0) : albedo(a), fuzz(f < 1 ? f : 1) {}

    color get_color(const vec3& p) const override {
        return albedo->value(p);
    }

    bool scatter(const ray& r_in, const vec3& hit_point, const vec3& normal, vec3& scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), normal);
        scattered = reflected;
        return true;
    }
};