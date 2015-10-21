# cmake - geo2d library
project(GEO2D_TEST)

cmake_minimum_required(VERSION 2.6)

include_directories(${GEO2D_SOURCE_DIR})

add_executable(test main.cpp)
target_link_libraries(test geo2d)

