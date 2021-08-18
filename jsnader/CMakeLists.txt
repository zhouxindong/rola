# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} "skel.h")
set (HDR_FILES ${HDR_FILES} "etcp.hpp")
set (SRC_FILES ${SRC_FILES} "wincompat.c")
set (SRC_FILES ${SRC_FILES} "tcpserver_skel.cpp")
set (SRC_FILES ${SRC_FILES} "tcpclient_skel.cpp")
set (SRC_FILES ${SRC_FILES} "udpserver_skel.cpp")
set (SRC_FILES ${SRC_FILES} "udpclient_skel.cpp")
set (SRC_FILES ${SRC_FILES} "tcpserver_vrec.cpp")
set (SRC_FILES ${SRC_FILES} "tcpclient_vrec.cpp")
set (SRC_FILES ${SRC_FILES} "udpclient_perf.cpp")
set (SRC_FILES ${SRC_FILES} "udpserver_perf.cpp")
set (SRC_FILES ${SRC_FILES} "peercrash_client.cpp")
set (SRC_FILES ${SRC_FILES} "peercrash_server.cpp")
set (HDR_FILES ${HDR_FILES} "heartbeat.h")
set (SRC_FILES ${SRC_FILES} "heartbeat_client.cpp")

# Add source to this project's executable.
add_executable (jsnader ${HDR_FILES} ${SRC_FILES} "heartbeat_server.cpp")

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/jsnader)

