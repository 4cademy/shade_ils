cmake_minimum_required(VERSION 3.27)
project(shade_ils)

set(CMAKE_CXX_STANDARD 17)

add_executable(shade_ils main.cpp
        shade_ils.h
        shade_ils.cpp
        objective_functions.h
        objective_functions.cpp
        population_utils.cpp
        population_utils.h)
