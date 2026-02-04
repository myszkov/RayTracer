#include <iostream>
#include <fstream>
#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "sphere.h"
#include "light.h"
#include "scene.h"
#include "material.h"

const point_light light(vec3(2, 3, 1), color(1, 1, 1));

color ray_color(const ray& r, const scene& world, int depth) {
    // If we've bounced too many times, stop
    if (depth <= 0) {
        return color(0, 0, 0);
    }

    hit_record rec;

    if (world.hit(r, 0.001, 1e9, rec)) {
        // Check if material reflects rays
        vec3 scattered_direction;
        if (rec.mat_ptr->scatter(r, rec.point, rec.normal, scattered_direction)) {
            // This is a reflective material - shoot a new ray in the reflected direction
            ray scattered(rec.point, scattered_direction);
            return rec.mat_ptr->get_color() * ray_color(scattered, world, depth - 1);
        }

        // Otherwise it's a lambertian material - do normal diffuse shading
        vec3 light_dir = light.position - rec.point;
        double light_distance = light_dir.length();
        ray shadow_ray(rec.point, unit_vector(light_dir));

        hit_record shadow_rec;
        bool in_shadow = world.hit(shadow_ray, 0.001, light_distance, shadow_rec);

        return calculate_diffuse(rec.point, rec.normal, light, rec.mat_ptr->get_color(), in_shadow);
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
    const int samples_per_pixel = 16;  // Number of rays per pixel for anti-aliasing

    double viewport_height = 2.0;
    double viewport_width = 2.0;
    vec3 camera_origin(0, 0, 0);

    // Create materials
    lambertian mat_diffuse_red(color(0.8, 0.2, 0.3));
    metal mat_metal_blue(color(0.6, 0.7, 0.9));
    metal mat_metal_green(color(0.2, 0.8, 0.3));
    lambertian mat_ground(color(0.5, 0.5, 0.5));

    // Set up the scene
    sphere s1(vec3(0, 0, -1), 0.5, &mat_diffuse_red);
    sphere s2(vec3(-0.7, 0.3, -1.2), 0.3, &mat_metal_blue);
    sphere s3(vec3(0.7, -0.2, -0.8), 0.25, &mat_metal_green);
    sphere ground(vec3(0, -100.5, -1), 100, &mat_ground);

    scene world;
    world.add(&s1);
    world.add(&s2);
    world.add(&s3);
    world.add(&ground);

    std::ofstream img("output.ppm");

    img << "P3\n";
    img << image_width << " " << image_height << "\n";
    img << "255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::cout << "Scanline: " << j << std::endl;
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);

            // Shoot multiple rays per pixel and average the results
            for (int s = 0; s < samples_per_pixel; s++) {
                // Add random offset within the pixel
                double u = (2.0 * (i + random_double()) / (image_width - 1)) - 1.0;
                double v = (2.0 * (j + random_double()) / (image_height - 1)) - 1.0;

                vec3 direction(u * viewport_width / 2.0, v * viewport_height / 2.0, -1.0);
                ray r(camera_origin, direction);

                pixel_color += ray_color(r, world, max_depth);
            }

            // Average the samples
            pixel_color = pixel_color / samples_per_pixel;

            write_color(img, pixel_color);
        }
    }

    img.close();
    std::cout << "Done! Output saved to output.ppm" << std::endl;

    return 0;
}