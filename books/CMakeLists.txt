# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)

# Add source to this project's executable.
add_executable (books ${HDR_FILES} ${SRC_FILES})
