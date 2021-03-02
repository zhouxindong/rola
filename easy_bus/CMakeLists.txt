# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "serialize.hpp")
set (SRC_FILES ${SRC_FILES} "serialize_main.cpp")

# Add source to this project's executable.
add_executable (easy_bus ${HDR_FILES} ${SRC_FILES} "message.hpp" "message_main.cpp" "message_box.hpp")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_bus)

# TODO: Add tests and install targets if needed.
set_property(TARGET easy_bus PROPERTY CXX_STANDARD 17)
set_property(TARGET easy_bus PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(
	easy_bus
	PUBLIC
	${CMAKE_SOURCE_DIR})