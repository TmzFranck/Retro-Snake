cmake_minimum_required(VERSION 3.10.0)
project(snake VERSION 0.1.0 LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/arm64-osx")

find_package(raylib CONFIG REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(snake main.cpp)
target_link_libraries(snake PRIVATE raylib glfw)

