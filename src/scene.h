#pragma once
#include <vector>
#include "hittable.h"

class scene {
public:
    std::vector<hittable*> objects;

    void add(hittable* object) {
        objects.push_back(object);
    }

    // Tests the ray against all objects, keeping only the closest hit
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
        hit_record temp_rec;
        bool hit_anything = false;
        double closest_so_far = t_max;

        for (const auto& object : objects) {
            if (object->hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};