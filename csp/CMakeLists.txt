# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} message.hpp)
set (SRC_FILES ${SRC_FILES} message_main.cpp)
set (HDR_FILES ${HDR_FILES} "sender.hpp")
set (HDR_FILES ${HDR_FILES} "receiver.hpp")
set (HDR_FILES ${HDR_FILES} "dispatcher.hpp")

# Add source to this project's executable.
add_executable (csp ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/csp)

# TODO: Add tests and install targets if needed.
