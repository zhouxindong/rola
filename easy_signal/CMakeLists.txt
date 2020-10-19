# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (SRC_FILES ${SRC_FILES} easy_signal_main.cpp)
set (SRC_FILES ${SRC_FILES} "easy_signal.hpp")

# Add source to this project's executable.
add_executable (easy_signal ${SRC_FILES})

target_include_directories(
	easy_signal
	PUBLIC
	${CMAKE_SOURCE_DIR})

# TODO: Add tests and install targets if needed.
