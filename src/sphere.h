#pragma once
#include "hittable.h"

class sphere : public hittable {
public:
    vec3 center;
    double radius;
    color object_color;

    sphere() {}
    sphere(const vec3& c, double r, const color& col)
        : center(c), radius(r), object_color(col) {
    }

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec3 oc = r.origin() - center;

        double a = dot(r.direction(), r.direction());
        double b = 2.0 * dot(oc, r.direction());
        double c = dot(oc, oc) - radius * radius;

        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return false;
        }

        // Check if the nearest hit is within the valid range
        double t = (-b - std::sqrt(discriminant)) / (2.0 * a);
        if (t < t_min || t > t_max) {
            // Try the farther intersection point
            t = (-b + std::sqrt(discriminant)) / (2.0 * a);
            if (t < t_min || t > t_max) {
                return false;
            }
        }

        // Record the hit information
        rec.t = t;
        rec.point = r.at(t);
        rec.normal = unit_vector(rec.point - center);
        rec.object_color = object_color;
        return true;
    }
};