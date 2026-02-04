#include <iostream>
#include <fstream>
#include "../src/vec3.h"
#include "../src/ray.h"
#include "../src/color.h"
#include "../src/sphere.h"
#include "../src/light.h"
#include "../src/scene.h"
#include "../src/material.h"
#include "../src/texture.h"

// FEATURE FLAGS - Change these to showcase different stages
const bool SHOW_SPHERE = true;
const bool SHOW_LIGHTING = true;
const bool SHOW_MULTIPLE = true;
const bool SHOW_SHADOWS = true;
const bool SHOW_MATERIALS = true;
const bool SHOW_ANTIALIASING = true;
const bool SHOW_TEXTURES = true;
const char* OUTPUT_FILE = "08_textures.ppm";

const point_light light(vec3(2, 3, 1), color(1, 1, 1));

color ray_color(const ray& r, const scene& world, int depth) {
    if (depth <= 0) {
        return color(0, 0, 0);
    }

    hit_record rec;

    if (world.hit(r, 0.001, 1e9, rec)) {
        // Materials and reflections
        if (SHOW_MATERIALS) {
            vec3 scattered_direction;
            if (rec.mat_ptr->scatter(r, rec.point, rec.normal, scattered_direction)) {
                ray scattered(rec.point, scattered_direction);
                return rec.mat_ptr->get_color(rec.point) * ray_color(scattered, world, depth - 1);
            }
        }

        // Lighting
        if (SHOW_LIGHTING) {
            bool in_shadow = false;

            // Shadow checking
            if (SHOW_SHADOWS) {
                vec3 light_dir = light.position - rec.point;
                double light_distance = light_dir.length();
                ray shadow_ray(rec.point, unit_vector(light_dir));
                hit_record shadow_rec;
                in_shadow = world.hit(shadow_ray, 0.001, light_distance, shadow_rec);
            }

            return calculate_diffuse(rec.point, rec.normal, light, rec.mat_ptr->get_color(rec.point), in_shadow);
        }
        else {
            // Just show normals
            vec3 normal = rec.normal;
            return 0.5 * color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
        }
    }

    // Sky gradient background
    vec3 unit_dir = unit_vector(r.direction());
    double t_sky = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - t_sky) * color(1.0, 1.0, 1.0) + t_sky * color(0.4, 0.68, 1.0);
}

int main() {
    const int image_width = 400;
    const int image_height = 400;
    const int max_depth = 10;
    const int samples_per_pixel = SHOW_ANTIALIASING ? 16 : 1;

    double viewport_height = 2.0;
    double viewport_width = 2.0;
    vec3 camera_origin(0, 0, 0);

    // Textures
    solid_color red_texture(0.8, 0.2, 0.3);
    solid_color gold_color(0.9, 0.7, 0.2);
    solid_color white(1.0, 1.0, 1.0);
    solid_color black(0.1, 0.1, 0.1);
    solid_color orange(1.0, 0.5, 0.0);
    solid_color purple(0.5, 0.0, 0.8);

    checker_texture ground_checker(&white, &black, 10.0);
    stripe_texture stripe_pattern(&orange, &purple, 5.0);

    // Materials
    lambertian mat_red(SHOW_TEXTURES ? (texture*)&red_texture : (texture*)&red_texture);
    metal mat_gold(&gold_color);
    lambertian mat_striped(SHOW_TEXTURES ? (texture*)&stripe_pattern : (texture*)&orange);
    lambertian mat_ground(SHOW_TEXTURES ? (texture*)&ground_checker : (texture*)&white);

    // Scene setup
    sphere s1(vec3(0, 0, -1), 0.5, &mat_red);
    sphere s2(vec3(-0.7, 0.3, -1.2), 0.3, SHOW_MATERIALS ? (material*)&mat_gold : (material*)&mat_red);
    sphere s3(vec3(0.7, -0.2, -0.8), 0.25, &mat_striped);
    sphere ground(vec3(0, -100.5, -1), 100, &mat_ground);

    scene world;

    if (SHOW_SPHERE) {
        world.add(&s1);
        if (SHOW_MULTIPLE) {
            world.add(&s2);
            world.add(&s3);
            world.add(&ground);
        }
    }

    std::ofstream img(OUTPUT_FILE);

    img << "P3\n";
    img << image_width << " " << image_height << "\n";
    img << "255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::cout << "Scanline: " << j << std::endl;
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; s++) {
                double u, v;
                if (SHOW_ANTIALIASING) {
                    u = (2.0 * (i + random_double()) / (image_width - 1)) - 1.0;
                    v = (2.0 * (j + random_double()) / (image_height - 1)) - 1.0;
                }
                else {
                    u = (2.0 * i / (image_width - 1)) - 1.0;
                    v = (2.0 * j / (image_height - 1)) - 1.0;
                }

                vec3 direction(u * viewport_width / 2.0, v * viewport_height / 2.0, -1.0);
                ray r(camera_origin, direction);

                pixel_color += ray_color(r, world, max_depth);
            }

            pixel_color = pixel_color / samples_per_pixel;

            write_color(img, pixel_color);
        }
    }

    img.close();
    std::cout << "Done! Saved to " << OUTPUT_FILE << std::endl;

    return 0;
}