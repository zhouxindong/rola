# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} container.hpp)
set (SRC_FILES ${SRC_FILES} container_main.cpp)
set (HDR_FILES ${HDR_FILES} "chrono_ex.hpp")
set (SRC_FILES ${SRC_FILES} "chrono_ex_main.cpp")
set (HDR_FILES ${HDR_FILES} "string.hpp")
set (HDR_FILES ${HDR_FILES} "thread.hpp")
set (SRC_FILES ${SRC_FILES} "thread_main.cpp")
set (HDR_FILES ${HDR_FILES} "easy_range.hpp")
set (HDR_FILES ${HDR_FILES} "rx_range.hpp")
set (SRC_FILES ${SRC_FILES} "rx_range_main.cpp")
set (HDR_FILES ${HDR_FILES} "thread_safe_stack.hpp")
set (HDR_FILES ${HDR_FILES} "thread_safe_queue.hpp")
set (HDR_FILES ${HDR_FILES} "thread_safe_lookup_table.hpp")
set (HDR_FILES ${HDR_FILES} "thread_safe_list.hpp")
set (HDR_FILES ${HDR_FILES} "thread_pool.hpp")
set (HDR_FILES ${HDR_FILES} "interruptible_thread.hpp")
set (SRC_FILES ${SRC_FILES} "interruptible_thread_main.cpp")

# Add source to this project's executable.
add_executable (stlex ${HDR_FILES} ${SRC_FILES} "algorithm.hpp")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/stlex)

# TODO: Add tests and install targets if needed.
set_property(TARGET stlex PROPERTY CXX_STANDARD 17)
set_property(TARGET stlex PROPERTY CXX_STANDARD_REQUIRED ON)