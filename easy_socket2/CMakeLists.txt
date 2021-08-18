# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (SRC_FILES ${SRC_FILES} "easy_socket2_main.cpp")
set (HDR_FILES ${HDR_FILES} "platform2.h")
set (HDR_FILES ${HDR_FILES} "socket_init.hpp")
set (HDR_FILES ${HDR_FILES} "socket_exception.hpp")
set (HDR_FILES ${HDR_FILES} "socket2.hpp")
set (HDR_FILES ${HDR_FILES} "connected_socket.hpp")
set (HDR_FILES ${HDR_FILES} "tcp_server.hpp")
set (HDR_FILES ${HDR_FILES} "tcp_client.hpp")
set (HDR_FILES ${HDR_FILES} "udp_server.hpp")
set (HDR_FILES ${HDR_FILES} "unconnected_socket.hpp")
set (HDR_FILES ${HDR_FILES} "udp_client.hpp")
set (HDR_FILES ${HDR_FILES} "multicast_send.hpp")
set (SRC_FILES ${SRC_FILES} "multicast_send_main.cpp")
set (HDR_FILES ${HDR_FILES} "multicast_recv.hpp")
set (SRC_FILES ${SRC_FILES} "multicast_recv_main.cpp")
set (HDR_FILES ${HDR_FILES} "broadcast_send.hpp")
set (SRC_FILES ${SRC_FILES} "broadcast_send_main.cpp")
set (HDR_FILES ${HDR_FILES} "broadcast_recv.hpp")
set (SRC_FILES ${SRC_FILES} "broadcast_recv_main.cpp")
set (HDR_FILES ${HDR_FILES} "stream_parser.hpp")
set (SRC_FILES ${SRC_FILES} "stream_parser_main.cpp")
set (HDR_FILES ${HDR_FILES} "tcp_connect.hpp")
set (SRC_FILES ${SRC_FILES} "tcp_connect_main.cpp")
set (HDR_FILES ${HDR_FILES} "heart_beater.hpp")
set (SRC_FILES ${SRC_FILES} "heart_beater_main.cpp")
set (HDR_FILES ${HDR_FILES} "tcp_file_server.hpp")
set (SRC_FILES ${SRC_FILES} "tcp_file_server_main.cpp")
set (HDR_FILES ${HDR_FILES} "file_server_protocol.hpp")
set (SRC_FILES ${SRC_FILES} "file_server_protocol_main.cpp")
set (HDR_FILES ${HDR_FILES} "tcp_file_client.hpp")
set (SRC_FILES ${SRC_FILES} "tcp_file_client_main.cpp")

# Add source to this project's executable.
add_executable (easy_socket2 ${HDR_FILES} ${SRC_FILES} )

target_include_directories(
	easy_socket2
	PUBLIC
	${CMAKE_SOURCE_DIR})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_socket2)

# TODO: Add tests and install targets if needed.
