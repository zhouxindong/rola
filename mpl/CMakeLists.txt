# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} easy_bind.hpp)
set (SRC_FILES ${SRC_FILES} easy_bind_main.cpp)
set (HDR_FILES ${HDR_FILES} type_list.hpp)
set (SRC_FILES ${SRC_FILES} type_list_main.cpp)
set (HDR_FILES ${HDR_FILES} value_list.hpp)
set (SRC_FILES ${SRC_FILES} value_list_main.cpp)
set (HDR_FILES ${HDR_FILES} "tuple.hpp")
set (SRC_FILES ${SRC_FILES} "tuple_main.cpp")
set (HDR_FILES ${HDR_FILES} "tuple2.hpp")
set (HDR_FILES ${HDR_FILES} "variant.hpp")
set (HDR_FILES ${HDR_FILES} "function.hpp")
set (SRC_FILES ${SRC_FILES} "function_main.cpp")
set (HDR_FILES ${HDR_FILES} "any_value.hpp")
set (SRC_FILES ${SRC_FILES} "any_value_main.cpp")

# Add source to this project's executable.
add_executable (mpl ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/mpl)

# TODO: Add tests and install targets if needed.

#need for static_assert()
set_property(TARGET mpl PROPERTY CXX_STANDARD 17)
set_property(TARGET mpl PROPERTY CXX_STANDARD_REQUIRED ON)