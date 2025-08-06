# Leo Build System
## <!> WARNING: This is a work in progress. Don't use this build system in any real project! <!>
This is a simple build system written in C++ for C++. A sample project file ```Sample.xml``` is provided with this repository.
Simply pass it or any other project file in that format to the build system binary.

# Building
This project uses [CMake](https://cmake.org) to build programs. Clone this repository and build with ```cmake``` <br>
```
mkdir build
cmake -S . -B build
cmake --build build
```

### Dependencies:
- [tinyxml2](https://github.com/leethomason/tinyxml2) (already included in this repository)

# Running
To run this build system, simply pass your project file to the build system executable.
### Windows (buggy)
```
buildsystem.exe <project file>
```

### GNU/Linux
```
./buildsystem <project file>
```
Currently, it only builds the project file.
Specifying any other options is not supported. It will be added in future
