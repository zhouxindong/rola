# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} lazy_val.hpp)
set (SRC_FILES ${SRC_FILES} lazy_val_main.cpp)

# Add source to this project's executable.
add_executable (optimize ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/optimize)

# TODO: Add tests and install targets if needed.
