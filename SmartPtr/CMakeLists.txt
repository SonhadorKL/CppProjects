cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 20)

project(SmartPtr)

add_library(SmartPtr INTERFACE)

target_include_directories(SmartPtr INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)