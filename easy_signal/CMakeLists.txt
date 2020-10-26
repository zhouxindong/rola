# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} easy_signal.hpp)
set (HDR_FILES ${HDR_FILES} signal_hub.hpp)
set (SRC_FILES ${SRC_FILES} signal_hub_main.cpp)
set (SRC_FILES ${SRC_FILES} easy_signal_main.cpp)

# Add source to this project's executable.
add_executable (easy_signal ${HDR_FILES} ${SRC_FILES})

target_include_directories(
	easy_signal
	PUBLIC
	${CMAKE_SOURCE_DIR})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_signal)

# TODO: Add tests and install targets if needed.
