# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (SRC_FILES ${SRC_FILES} )
set (HDR_FILES ${HDR_FILES} "assert.hpp")
set (SRC_FILES ${SRC_FILES} "assert_main.cpp")
set (HDR_FILES ${HDR_FILES} "byte.hpp")
set (SRC_FILES ${SRC_FILES} "byte_main.cpp")
set (HDR_FILES ${HDR_FILES} "pointers.hpp")
set (SRC_FILES ${SRC_FILES} "pointers_main.cpp")
set (HDR_FILES ${HDR_FILES} "util.hpp")
set (SRC_FILES ${SRC_FILES} "util_main.cpp")
set (HDR_FILES ${HDR_FILES} "span.hpp")
set (SRC_FILES ${SRC_FILES} "span_main.cpp")
set (HDR_FILES ${HDR_FILES} "span_ext.hpp")
set (HDR_FILES ${HDR_FILES} "string_span.hpp")

# Add source to this project's executable.
add_executable (gsl ${HDR_FILES} ${SRC_FILES} "span_ext_main.cpp")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/gsl)

# TODO: Add tests and install targets if needed.
set_property(TARGET gsl PROPERTY CXX_STANDARD 17)
set_property(TARGET gsl PROPERTY CXX_STANDARD_REQUIRED ON)