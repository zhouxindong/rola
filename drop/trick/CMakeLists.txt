﻿# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (SRC_FILES ${SRC_FILES} trick_main.cpp)
set (HDR_FILES ${HDR_FILES} except_safe.hpp)
set (HDR_FILES ${HDR_FILES} "proxy.hpp")
set (HDR_FILES ${HDR_FILES} "recursion.hpp")

# Add source to this project's executable.
add_executable (trick ${HDR_FILES} ${SRC_FILES} "monad.hpp")

target_include_directories(
	trick
	PUBLIC
	${CMAKE_SOURCE_DIR})

# TODO: Add tests and install targets if needed.
set_property(TARGET trick PROPERTY CXX_STANDARD 17)
set_property(TARGET trick PROPERTY CXX_STANDARD_REQUIRED ON)
