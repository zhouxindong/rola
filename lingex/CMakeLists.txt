# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} array.hpp)
set (HDR_FILES ${HDR_FILES} "fluentx.hpp")
set (HDR_FILES ${HDR_FILES} "conditionable.hpp")
set (SRC_FILES ${SRC_FILES} "fluentx_main.cpp")
set (HDR_FILES ${HDR_FILES} "loopable.hpp")
set (HDR_FILES ${HDR_FILES} "curried.hpp")
set (SRC_FILES ${SRC_FILES} "curried_main.cpp")

# Add source to this project's executable.
add_executable (lingex ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/lingex)

# TODO: Add tests and install targets if needed.
set_property(TARGET lingex PROPERTY CXX_STANDARD 17)
set_property(TARGET lingex PROPERTY CXX_STANDARD_REQUIRED ON)