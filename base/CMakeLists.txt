# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (SRC_FILES ${SRC_FILES} singlable.hpp)
set (SRC_FILES ${SRC_FILES} multi_include.cpp)
set (SRC_FILES ${SRC_FILES} singlable_main.cpp)

# Add source to this project's executable.
add_executable (base ${SRC_FILES})

# TODO: Add tests and install targets if needed.
