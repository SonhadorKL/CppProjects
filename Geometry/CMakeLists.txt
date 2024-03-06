cmake_minimum_required(VERSION 3.10)

project(Geometry)

set (
    SOURCE
    src/General.cpp
    src/Primitives.cpp
    src/Polygons.cpp
    src/Ellipse.cpp
)

add_library( 
    Geometry
    ${SOURCE}
)

target_include_directories(Geometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)