# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (SRC_FILES ${SRC_FILES} "callable_main.cpp")
set (HDR_FILES ${HDR_FILES} "class.h")
set (HDR_FILES ${HDR_FILES} "callable.h")
set (HDR_FILES ${HDR_FILES} "forward.h")
set (HDR_FILES ${HDR_FILES} "compile_time.h")
set (HDR_FILES ${HDR_FILES} SFINAE.h)
set (SRC_FILES ${SRC_FILES} "SFINAE.cpp")
set (HDR_FILES ${HDR_FILES} "debug_template.h")

# Add source to this project's executable.
add_executable (drop ${HDR_FILES} ${SRC_FILES} "thread.h" "tuple_main.cpp" "container_demo.h" "iterator_demo.h" "cpp17.h")

add_subdirectory ("trick")
set_property(TARGET drop PROPERTY CXX_STANDARD 17)
set_property(TARGET drop PROPERTY CXX_STANDARD_REQUIRED ON)
# TODO: Add tests and install targets if needed.
