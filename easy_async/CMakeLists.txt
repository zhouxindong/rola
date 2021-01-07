# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)

set (HDR_FILES ${HDR_FILES} "easy_async.hpp")
set (SRC_FILES ${SRC_FILES} "easy_async_main.cpp")
set (SRC_FILES ${SRC_FILES} "detail/scheduler.cpp")
set (SRC_FILES ${SRC_FILES} "detail/threadpool_scheduler.cpp")
set (SRC_FILES ${SRC_FILES} "task_main.cpp")
set (SRC_FILES ${SRC_FILES} "parallel_algo_main.cpp")

# Add source to this project's executable.
add_executable (easy_async ${HDR_FILES} ${SRC_FILES}  )

add_subdirectory ("include")
add_subdirectory ("detail")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_async)

# TODO: Add tests and install targets if needed.
