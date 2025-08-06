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

target_link_libraries(BuildSystem
    PUBLIC $<IF:$<PLATFORM_ID:Windows,CYGWIN>,user32 kernel32 shell32,>)