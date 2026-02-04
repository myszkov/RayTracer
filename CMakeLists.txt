cmake_minimum_required(VERSION 3.20)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Comment/uncomment these lines to switch between versions:

# Normal version (default)
# add_executable(RayTracer src/main.cpp)

# Showcase version (uncomment this and comment the line above)
add_executable(RayTracer stages/showcase_main.cpp)