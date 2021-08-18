# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "serialize.hpp")
set (SRC_FILES ${SRC_FILES} "serialize_main.cpp")
set (HDR_FILES ${HDR_FILES} "message.hpp")
set (SRC_FILES ${SRC_FILES} "message_main.cpp")
set (HDR_FILES ${HDR_FILES} "message_box.hpp")
set (HDR_FILES ${HDR_FILES} "app_base.hpp")
set (HDR_FILES ${HDR_FILES} "message_demo.hpp")
set (HDR_FILES ${HDR_FILES} "message_dispatcher.hpp")
set (SRC_FILES ${SRC_FILES} "easy_bus_app1.cpp")
set (SRC_FILES ${SRC_FILES} "easy_bus_app2.cpp")

# Add source to this project's executable.
add_executable (easy_bus ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_bus)

# TODO: Add tests and install targets if needed.
set_property(TARGET easy_bus PROPERTY CXX_STANDARD 17)
set_property(TARGET easy_bus PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(
	easy_bus
	PUBLIC
	${CMAKE_SOURCE_DIR})