#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "sphere.h"
#include "light.h"
#include "scene.h"
#include "material.h"
#include "texture.h"

color ray_color(const ray& r, const scene& world, const point_light& light, int depth) {
    if (depth <= 0) return color(0, 0, 0);

    hit_record rec;

    if (world.hit(r, 0.001, 1e9, rec)) {
        vec3 scattered_direction;
        if (rec.mat_ptr->scatter(r, rec.point, rec.normal, scattered_direction)) {
            ray scattered(rec.point, scattered_direction);
            return rec.mat_ptr->get_color(rec.point) * ray_color(scattered, world, light, depth - 1);
        }

        vec3 light_dir = light.position - rec.point;
        double light_distance = light_dir.length();
        ray shadow_ray(rec.point, unit_vector(light_dir));

        hit_record shadow_rec;
        bool in_shadow = world.hit(shadow_ray, 0.001, light_distance, shadow_rec);

        return calculate_diffuse(rec.point, rec.normal, light, rec.mat_ptr->get_color(rec.point), in_shadow);
    }

    vec3 unit_dir = unit_vector(r.direction());
    double t_sky = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - t_sky) * color(1.0, 1.0, 1.0) + t_sky * color(0.4, 0.68, 1.0);
}

void render(sf::Uint8* pixels, const scene& world, const point_light& light,
            int image_width, int image_height, int samples_per_pixel, int max_depth) {
    const double viewport_height = 2.0;
    const double viewport_width  = 2.0;
    const vec3 camera_origin(0, 0, 0);

    for (int j = image_height - 1; j >= 0; j--) {
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; s++) {
                double u = (2.0 * (i + random_double()) / (image_width  - 1)) - 1.0;
                double v = (2.0 * (j + random_double()) / (image_height - 1)) - 1.0;

                vec3 direction(u * viewport_width / 2.0, v * viewport_height / 2.0, -1.0);
                ray r(camera_origin, direction);
                pixel_color += ray_color(r, world, light, max_depth);
            }

            pixel_color = pixel_color / samples_per_pixel;

            // j=image_height-1 is the top of the scene; SFML row 0 is screen top
            int screen_row = image_height - 1 - j;
            int idx = (screen_row * image_width + i) * 4;

            auto clamp01 = [](double x) { return x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 : x); };
            pixels[idx + 0] = static_cast<sf::Uint8>(255.99 * clamp01(pixel_color.x()));
            pixels[idx + 1] = static_cast<sf::Uint8>(255.99 * clamp01(pixel_color.y()));
            pixels[idx + 2] = static_cast<sf::Uint8>(255.99 * clamp01(pixel_color.z()));
            pixels[idx + 3] = 255;
        }
    }
}

int main() {
    const int image_width       = 400;
    const int image_height      = 400;
    const int max_depth         = 10;
    const int samples_per_pixel = 16;

    // Textures
    solid_color red_texture(0.8, 0.2, 0.3);
    solid_color gold_color(0.9, 0.7, 0.2);
    solid_color white(1.0, 1.0, 1.0);
    solid_color black(0.1, 0.1, 0.1);
    checker_texture ground_checker(&white, &black, 10.0);
    solid_color orange(1.0, 0.5, 0.0);
    solid_color purple(0.5, 0.0, 0.8);
    stripe_texture stripe_pattern(&orange, &purple, 5.0);

    // Materials
    lambertian mat_diffuse_red(&red_texture);
    metal      mat_metal_gold(&gold_color);
    lambertian mat_striped(&stripe_pattern);
    lambertian mat_ground(&ground_checker);

    // Scene
    sphere s1(vec3(0, 0, -1), 0.5, &mat_diffuse_red);
    sphere s2(vec3(-0.7, 0.3, -1.2), 0.3, &mat_metal_gold);
    sphere s3(vec3(0.7, -0.2, -0.8), 0.25, &mat_striped);
    sphere ground_sphere(vec3(0, -100.5, -1), 100, &mat_ground);

    scene world;
    world.add(&s1);
    world.add(&s2);
    world.add(&s3);
    world.add(&ground_sphere);

    // Light starts at its original position
    point_light light(vec3(2, 3, 1), color(1, 1, 1));

    // SFML window and texture
    sf::RenderWindow window(sf::VideoMode(image_width, image_height), "RayTracer - Left click to move light");
    sf::Texture texture;
    texture.create(image_width, image_height);
    sf::Sprite sprite(texture);

    std::vector<sf::Uint8> pixels(image_width * image_height * 4);

    std::cout << "Rendering initial frame..." << std::endl;
    render(pixels.data(), world, light, image_width, image_height, samples_per_pixel, max_depth);
    texture.update(pixels.data());
    std::cout << "Done. Left-click anywhere in the window to move the light." << std::endl;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {

                int px = event.mouseButton.x;
                int py = event.mouseButton.y;

                // Map click X -> light world X  (-3 to +3)
                // Map click Y -> light world Z  (+2 at top, -3 at bottom)
                // Light Y is fixed at 3.0 to stay above the scene
                double light_x = (px / (double)image_width)  * 6.0 - 3.0;
                double light_z = 2.0 - (py / (double)image_height) * 5.0;
                light = point_light(vec3(light_x, 3.0, light_z), color(1, 1, 1));

                std::cout << "Light -> (" << light_x << ", 3.0, " << light_z << ")  Rendering..." << std::endl;
                render(pixels.data(), world, light, image_width, image_height, samples_per_pixel, max_depth);
                texture.update(pixels.data());
                std::cout << "Done." << std::endl;
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
