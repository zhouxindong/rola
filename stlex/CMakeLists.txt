# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} container.hpp)
set (SRC_FILES ${SRC_FILES} container_main.cpp)
set (HDR_FILES ${HDR_FILES} "chrono_ex.hpp")
set (SRC_FILES ${SRC_FILES} "chrono_ex_main.cpp")

# Add source to this project's executable.
add_executable (stlex ${HDR_FILES} ${SRC_FILES} )

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/stlex)

# TODO: Add tests and install targets if needed.
