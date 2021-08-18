# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "get_num.hpp")
set (HDR_FILES ${HDR_FILES} "ename.c.inc")
set (HDR_FILES ${HDR_FILES} "error_functions.hpp")
set (HDR_FILES ${HDR_FILES} "tlpi_hdr.h")
set (HDR_FILES ${HDR_FILES} "tlpi.h")
set (SRC_FILES ${SRC_FILES} "1_fileio_copy.cpp")
set (HDR_FILES ${HDR_FILES} "platform.h")
set (SRC_FILES ${SRC_FILES} "1_fileio_temp.cpp")
set (SRC_FILES ${SRC_FILES} "2_proc_env_disp.cpp")
set (SRC_FILES ${SRC_FILES} "2_proc_env_modify.cpp")
set (SRC_FILES ${SRC_FILES} "3_datetime_disp.cpp")
set (SRC_FILES ${SRC_FILES} "3_datetime_cpu.cpp")
set (SRC_FILES ${SRC_FILES} "4_posix_sysconf.cpp")
set (SRC_FILES ${SRC_FILES} "2_proc_pidmax.cpp")
set (SRC_FILES ${SRC_FILES} "5_signal_handler.cpp")
set (HDR_FILES ${HDR_FILES} "inet_sockets.hpp")

# Add source to this project's executable.
add_executable (tlpi ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/tlpi)

