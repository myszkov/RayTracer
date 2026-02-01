#include <iostream>
#include <fstream>
#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "sphere.h"
#include "light.h"
#include "scene.h"

const point_light light(vec3(2, 3, 1), color(1, 1, 1));

color ray_color(const ray& r, const scene& world) {
    hit_record rec;

    if (world.hit(r, 0.001, 1e9, rec)) {
        return calculate_diffuse(rec.point, rec.normal, light, rec.object_color);
    }

    // Sky gradient background
    vec3 unit_dir = unit_vector(r.direction());
    double t_sky = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - t_sky) * color(1.0, 1.0, 1.0) + t_sky * color(0.4, 0.68, 1.0);
}

int main() {
    const int image_width = 256;
    const int image_height = 256;

    double viewport_height = 2.0;
    double viewport_width = 2.0;
    vec3 camera_origin(0, 0, 0);

    // Set up the scene with multiple spheres
    sphere s1(vec3(0, 0, -1), 0.5, color(0.8, 0.2, 0.3));      // Red sphere in the centre
    sphere s2(vec3(-0.7, 0.3, -1.2), 0.3, color(0.2, 0.5, 0.8)); // Blue sphere to the left
    sphere s3(vec3(0.7, -0.2, -0.8), 0.25, color(0.2, 0.8, 0.3)); // Green sphere to the right

    scene world;
    world.add(&s1);
    world.add(&s2);
    world.add(&s3);

    std::ofstream img("output.ppm");

    img << "P3\n";
    img << image_width << " " << image_height << "\n";
    img << "255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::cout << "Scanline: " << j << std::endl;
        for (int i = 0; i < image_width; i++) {
            double u = (2.0 * i / (image_width - 1)) - 1.0;
            double v = (2.0 * j / (image_height - 1)) - 1.0;

            vec3 direction(u * viewport_width / 2.0, v * viewport_height / 2.0, -1.0);
            ray r(camera_origin, direction);

            color pixel_color = ray_color(r, world);
            write_color(img, pixel_color);
        }
    }

    img.close();
    std::cout << "Done! Output saved to output.ppm" << std::endl;

    return 0;
}