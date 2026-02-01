#include <iostream>
#include "vec3.h"
#include "ray.h"

int main() {
    vec3 origin(0, 0, 0);
    vec3 direction(1, 2, 3);
    ray r(origin, direction);

    std::cout << "Origin: " << r.origin().x() << " " << r.origin().y() << " " << r.origin().z() << std::endl;
    std::cout << "Direction: " << r.direction().x() << " " << r.direction().y() << " " << r.direction().z() << std::endl;
    std::cout << "Point at t=2: " << r.at(2).x() << " " << r.at(2).y() << " " << r.at(2).z() << std::endl;

    return 0;
}