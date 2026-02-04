#pragma once
/*
 * color.h - Color Handling and Output
 *
 * Defines color as an alias for vec3 (RGB values from 0.0 to 1.0).
 * Provides write_color() to convert floating-point colors to 8-bit integers
 * and write them to the PPM image file.
 */
#include "vec3.h"
#include <fstream>

// Color is just a vec3 with values 0.0 to 1.0 for R, G, B
using color = vec3;

inline void write_color(std::ofstream& out, color pixel_color) {
    // Convert from 0.0-1.0 range to 0-255 integer range
    int r = static_cast<int>(255.99 * pixel_color.x());
    int g = static_cast<int>(255.99 * pixel_color.y());
    int b = static_cast<int>(255.99 * pixel_color.z());

    out << r << " " << g << " " << b << "\n";
}