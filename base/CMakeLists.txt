# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} singlable.hpp)
set (SRC_FILES ${SRC_FILES} multi_include.cpp)
set (SRC_FILES ${SRC_FILES} singlable_main.cpp)
set (HDR_FILES ${HDR_FILES} "pimpl_base.hpp")
set (SRC_FILES ${SRC_FILES} "pimpl_base_main.cpp")
set (HDR_FILES ${HDR_FILES} "any_assert.hpp")
set (SRC_FILES ${SRC_FILES} "any_assert_main.cpp")
set (HDR_FILES ${HDR_FILES} "assignable.hpp")
set (SRC_FILES ${SRC_FILES} "assignable_main.cpp")
set (HDR_FILES ${HDR_FILES} "zval.hpp")

# Add source to this project's executable.
add_executable (base ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/base)

# TODO: Add tests and install targets if needed.
set_property(TARGET base PROPERTY CXX_STANDARD 17)
set_property(TARGET base PROPERTY CXX_STANDARD_REQUIRED ON)