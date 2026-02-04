#pragma once
/*
 * texture.h - Texture System
 *
 * Defines textures that determine surface colour based on position.
 * Textures can be solid colours, procedural patterns (checkerboard, stripes),
 * or eventually image-based textures.
 *
 * The value() function returns a colour for a given 3D point in space.
 */
#include "vec3.h"
#include "color.h"
#include <cmath>

class texture {
public:
    virtual ~texture() = default;
    virtual color value(const vec3& p) const = 0;
};

// Solid colour texture - always returns the same colour
class solid_color : public texture {
public:
    color color_value;

    solid_color() {}
    solid_color(const color& c) : color_value(c) {}
    solid_color(double r, double g, double b) : color_value(r, g, b) {}

    color value(const vec3& p) const override {
        return color_value;
    }
};

// Checkerboard pattern texture - alternates between two colours
class checker_texture : public texture {
public:
    const texture* odd;
    const texture* even;
    double scale;  // Size of the checks

    checker_texture(const texture* o, const texture* e, double s = 1.0)
        : odd(o), even(e), scale(s) {
    }

    color value(const vec3& p) const override {
        // Use sine functions to create alternating pattern
        double sines = sin(scale * p.x()) * sin(scale * p.y()) * sin(scale * p.z());

        if (sines < 0) {
            return odd->value(p);
        }
        else {
            return even->value(p);
        }
    }
};

// Stripe texture - vertical stripes along the x-axis
class stripe_texture : public texture {
public:
    const texture* color1;
    const texture* color2;
    double scale;

    stripe_texture(const texture* c1, const texture* c2, double s = 1.0)
        : color1(c1), color2(c2), scale(s) {
    }

    color value(const vec3& p) const override {
        // Use the x coordinate to determine stripe
        if (static_cast<int>(floor(p.x() * scale)) % 2 == 0) {
            return color1->value(p);
        }
        else {
            return color2->value(p);
        }
    }
};