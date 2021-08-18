# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "scpp_assert.hpp")
set (SRC_FILES ${SRC_FILES} "scpp_assert_main.cpp")
set (HDR_FILES ${HDR_FILES} "scpp_vector.hpp")
set (SRC_FILES ${SRC_FILES} "scpp_vector_main.cpp")
set (HDR_FILES ${HDR_FILES} "scpp_array.hpp")
set (SRC_FILES ${SRC_FILES} "scpp_array_main.cpp")
set (HDR_FILES ${HDR_FILES} "scpp_matrix.hpp")
set (SRC_FILES ${SRC_FILES} "scpp_matrix_main.cpp")
set (HDR_FILES ${HDR_FILES} "scpp_ptr.hpp")

# Add source to this project's executable.
add_executable (safecpp ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/safecpp)
