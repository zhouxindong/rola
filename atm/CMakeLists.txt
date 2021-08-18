# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} messages.hpp)
set (SRC_FILES ${SRC_FILES} atm_main.cpp)
set (HDR_FILES ${HDR_FILES} "atm_busi.hpp")
set (HDR_FILES ${HDR_FILES} "bank_machine.hpp")
set (HDR_FILES ${HDR_FILES} "interface_machine.hpp")

# Add source to this project's executable.
add_executable (atm ${HDR_FILES} ${SRC_FILES})

target_include_directories(
	atm
	PUBLIC
	${CMAKE_SOURCE_DIR})

# TODO: Add tests and install targets if needed.
