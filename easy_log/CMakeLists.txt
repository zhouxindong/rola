# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} log_level.hpp)
set (SRC_FILES ${SRC_FILES} log_level_main.cpp)
set (HDR_FILES ${HDR_FILES} log_item.hpp)
set (SRC_FILES ${SRC_FILES} log_item_main.cpp)
set (HDR_FILES ${HDR_FILES} log_formatter.hpp)
set (SRC_FILES ${SRC_FILES} log_formatter_main.cpp)
set (HDR_FILES ${HDR_FILES} log_channel.hpp)
set (SRC_FILES ${SRC_FILES} log_channel_main.cpp)
set (HDR_FILES ${HDR_FILES} log_filter.hpp)
set (SRC_FILES ${SRC_FILES} log_filter_main.cpp)
set (HDR_FILES ${HDR_FILES} basic_logger.hpp)
set (SRC_FILES ${SRC_FILES} basic_logger_main.cpp)
set (HDR_FILES ${HDR_FILES} compose_logger.hpp)
set (SRC_FILES ${SRC_FILES} compose_logger_main.cpp)
set (HDR_FILES ${HDR_FILES}  "easy_log_qt.hpp")

# Add source to this project's executable.
add_executable (easy_log ${HDR_FILES} ${SRC_FILES})

target_include_directories(
	easy_log
	PUBLIC
	${CMAKE_SOURCE_DIR})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_log)

# TODO: Add tests and install targets if needed.
