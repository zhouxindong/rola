# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "fifo_queue.h")
set (HDR_FILES ${HDR_FILES} "internal.h")
set (HDR_FILES ${HDR_FILES} "singleton.h")
set (HDR_FILES ${HDR_FILES} "task_wait_event.h")
set (HDR_FILES ${HDR_FILES} "work_steal_queue.h")

set (SRC_FILES ${SRC_FILES} "scheduler.cpp")
set (SRC_FILES ${SRC_FILES} "threadpool_scheduler.cpp")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_async/detail)
install (FILES ${SRC_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_async/detail)

# TODO: Add tests and install targets if needed.
