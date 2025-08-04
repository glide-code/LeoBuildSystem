cmake_minimum_required(VERSION 3.15)

project(BuildSystem)

add_executable(BuildSystem
    Main.cpp
    src/BuildSystem.cpp
    src/Compilers.cpp
    src/Utils.cpp
    ext/tinyxml2/tinyxml2.cpp
    )

target_include_directories(BuildSystem
    PUBLIC . include)
