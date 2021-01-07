# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)

set (HDR_FILES ${HDR_FILES} "aligned_alloc.h")
set (HDR_FILES ${HDR_FILES} "cancel.h")
set (HDR_FILES ${HDR_FILES} "continuation_vector.h")
set (HDR_FILES ${HDR_FILES} "parallel_for.h")
set (HDR_FILES ${HDR_FILES} "parallel_invoke.h")
set (HDR_FILES ${HDR_FILES} "parallel_reduce.h")
set (HDR_FILES ${HDR_FILES} "partitioner.h")
set (HDR_FILES ${HDR_FILES} "range.h")
set (HDR_FILES ${HDR_FILES} "ref_count.h")
set (HDR_FILES ${HDR_FILES} "scheduler.h")
set (HDR_FILES ${HDR_FILES} "scheduler_fwd.h")
set (HDR_FILES ${HDR_FILES} "task.h")
set (HDR_FILES ${HDR_FILES} "task_base.h")
set (HDR_FILES ${HDR_FILES} "traits.h")
set (HDR_FILES ${HDR_FILES} "when_all_any.h")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_async/include)

# TODO: Add tests and install targets if needed.
