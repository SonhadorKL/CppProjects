cmake_minimum_required(VERSION 3.10)

project(BigInteger)

add_library( 
    BigInteger
    src/BigInteger.cpp
    src/Rational.cpp
)

target_include_directories(BigInteger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)