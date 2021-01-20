# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} error_handle.hpp)
set (HDR_FILES ${HDR_FILES} "condition.hpp")
set (SRC_FILES ${SRC_FILES} "easy_actor_main.cpp")
set (HDR_FILES ${HDR_FILES} "block_queue.hpp")
set (HDR_FILES ${HDR_FILES} "spin_lock.hpp")
set (HDR_FILES ${HDR_FILES} "thread_pool.hpp")

# Add source to this project's executable.
add_executable (easy_actor ${HDR_FILES} ${SRC_FILES} "base.h")

target_include_directories(
	easy_actor
	PUBLIC
	${CMAKE_SOURCE_DIR})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_actor)

# TODO: Add tests and install targets if needed.
