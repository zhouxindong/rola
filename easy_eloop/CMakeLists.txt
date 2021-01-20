# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "event_listener.hpp")
set (SRC_FILES ${SRC_FILES} "easy_eloop_main.cpp")

# Add source to this project's executable.
add_executable (easy_eloop ${HDR_FILES} ${SRC_FILES} "event_loop.hpp")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_eloop)

# TODO: Add tests and install targets if needed.
