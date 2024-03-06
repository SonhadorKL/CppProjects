cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 20)
project(StringImpl)

add_library( 
    StringImpl
    src/String.cpp
)

target_include_directories(StringImpl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)