cmake_minimum_required(VERSION 3.8)
project(cs361_3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(cs361_3 ${SOURCE_FILES})

configure_file(raw/Esher.raw Esher.raw COPYONLY)
configure_file(raw/Chevalier_473.raw Chevalier_473.raw COPYONLY)