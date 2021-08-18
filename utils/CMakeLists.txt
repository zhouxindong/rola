# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} guid.hpp)
set (SRC_FILES ${SRC_FILES} guid_main.cpp)
set (HDR_FILES ${HDR_FILES} path.hpp)
set (HDR_FILES ${HDR_FILES} resolver.hpp)
set (SRC_FILES ${SRC_FILES} path_main.cpp)
set (HDR_FILES ${HDR_FILES} local_time.hpp)
set (HDR_FILES ${HDR_FILES} "tinyxml2.hpp")
set (SRC_FILES ${SRC_FILES} "tinyxml2_main.cpp")
set (HDR_FILES ${HDR_FILES} "json11.hpp")
set (SRC_FILES ${SRC_FILES} "json11_main.cpp")
set (HDR_FILES ${HDR_FILES} "easy_ini.hpp")
set (SRC_FILES ${SRC_FILES} "easy_ini_main.cpp")
set (HDR_FILES ${HDR_FILES} "byte_order.hpp")
set (HDR_FILES ${HDR_FILES} "ntp_client.hpp")
set (SRC_FILES ${SRC_FILES} "byte_order_main.cpp")
set (SRC_FILES ${SRC_FILES} "ntp_client_main.cpp")
set (HDR_FILES ${HDR_FILES} "hash_val.hpp")
set (HDR_FILES ${HDR_FILES} "predicator.hpp")
set (HDR_FILES ${HDR_FILES} "duplix.hpp")
set (HDR_FILES ${HDR_FILES} "stack_tracer.hpp")
set (SRC_FILES ${SRC_FILES} "stack_tracer_main.cpp")
set (HDR_FILES ${HDR_FILES} "serialize2.hpp")
set (SRC_FILES ${SRC_FILES} "serialize_main.cpp")
set (HDR_FILES ${HDR_FILES} "serialize.hpp")

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# Add source to this project's executable.
add_executable (utils ${HDR_FILES} ${SRC_FILES})

target_include_directories(
	utils
	PUBLIC
	${CMAKE_SOURCE_DIR})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/utils)

# TODO: Add tests and install targets if needed.
