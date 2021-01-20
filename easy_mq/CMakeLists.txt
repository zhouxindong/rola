# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "message.hpp")
set (SRC_FILES ${SRC_FILES} "easy_mq_main.cpp")

# Add source to this project's executable.
add_executable (easy_mq ${HDR_FILES} ${SRC_FILES} "queue.hpp")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_mq)

# TODO: Add tests and install targets if needed.
