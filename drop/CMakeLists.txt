# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (SRC_FILES ${SRC_FILES} drop_main.cpp)
set (HDR_FILES ${HDR_FILES} "class.h")

# Add source to this project's executable.
add_executable (drop ${HDR_FILES} ${SRC_FILES})

add_subdirectory ("trick")

# TODO: Add tests and install targets if needed.
