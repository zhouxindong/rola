# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} guid.hpp)
set (SRC_FILES ${SRC_FILES} guid_main.cpp)
set (HDR_FILES ${HDR_FILES} path.hpp)
set (HDR_FILES ${HDR_FILES} resolver.hpp)
set (SRC_FILES ${SRC_FILES} path_main.cpp)

# Add source to this project's executable.
add_executable (utils ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/utils)

# TODO: Add tests and install targets if needed.
