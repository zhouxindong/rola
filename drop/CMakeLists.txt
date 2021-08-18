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
set (HDR_FILES ${HDR_FILES} "thread.h")
set (SRC_FILES ${SRC_FILES} "tuple_main.cpp")
set (HDR_FILES ${HDR_FILES} "container_demo.h")
set (HDR_FILES ${HDR_FILES} "iterator_demo.h")
set (HDR_FILES ${HDR_FILES} "cpp17.h")
set (SRC_FILES ${SRC_FILES} "char_traits_main.cpp")
set (SRC_FILES ${SRC_FILES} "variant_main.cpp")
set (SRC_FILES ${SRC_FILES} "auto_parall.cpp")
set (HDR_FILES ${HDR_FILES} "char_traits.hpp")
set (HDR_FILES ${HDR_FILES} "inherience.h")
set (HDR_FILES ${HDR_FILES} "for_range.h")
set (HDR_FILES ${HDR_FILES} "user_literals.h")
set (SRC_FILES ${SRC_FILES} "preprocessor.cpp")
set (SRC_FILES ${SRC_FILES} "monads.cpp")
set (SRC_FILES ${SRC_FILES} "memory.cpp")
set (HDR_FILES ${HDR_FILES} "template.h")
set (HDR_FILES ${HDR_FILES} "header.h")
set (SRC_FILES ${SRC_FILES} "promise.cpp")

# Add source to this project's executable.
add_executable (drop ${HDR_FILES} ${SRC_FILES})

add_subdirectory ("trick")
set_property(TARGET drop PROPERTY CXX_STANDARD 17)
set_property(TARGET drop PROPERTY CXX_STANDARD_REQUIRED ON)
# TODO: Add tests and install targets if needed.
