# CMakeList.txt : CMake project for rola, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (SRC_FILES)
set (HDR_FILES)
set (HDR_FILES ${HDR_FILES} platform.h)
set (HDR_FILES ${HDR_FILES} sock_address.hpp)
set (SRC_FILES ${SRC_FILES} udp_socket_main.cpp)
set (HDR_FILES ${HDR_FILES} "socket.hpp")
set (HDR_FILES ${HDR_FILES} "exception.hpp")
set (HDR_FILES ${HDR_FILES} "inet_address.hpp")
set (HDR_FILES ${HDR_FILES} "datagram_socket.hpp")
set (HDR_FILES ${HDR_FILES} "udp_socket.h")
set (SRC_FILES ${SRC_FILES} "datagram_socket_main.cpp")
set (SRC_FILES ${SRC_FILES} "socket_main.cpp")
set (HDR_FILES ${HDR_FILES} "stream_socket.hpp")
set (HDR_FILES ${HDR_FILES} "acceptor.hpp")
set (HDR_FILES ${HDR_FILES} "connector.hpp")
set (HDR_FILES ${HDR_FILES} "tcp_socket.h")
set (HDR_FILES ${HDR_FILES} "tcp_acceptor.h")
set (HDR_FILES ${HDR_FILES} "tcp_connector.h")
set (HDR_FILES ${HDR_FILES} "inet6_address.hpp")
set (HDR_FILES ${HDR_FILES} "tcp6_acceptor.h")
set (HDR_FILES ${HDR_FILES} "tcp6_socket.h")
set (HDR_FILES ${HDR_FILES} "udp6_socket.h")
set (HDR_FILES ${HDR_FILES} "unix_address.hpp")
set (HDR_FILES ${HDR_FILES} "unix_dgram_socket.h")
set (HDR_FILES ${HDR_FILES} "unix_stream_socket.h")
set (HDR_FILES ${HDR_FILES} "unix_acceptor.hpp")
set (HDR_FILES ${HDR_FILES} "unix_connector.h")
set (SRC_FILES ${SRC_FILES} "inet_address_main.cpp")
set (SRC_FILES ${SRC_FILES} "stream_socket_main.cpp")
set (SRC_FILES ${SRC_FILES} "acceptor_main.cpp")
set (SRC_FILES ${SRC_FILES} "tcp_socket_main.cpp")
set (HDR_FILES ${HDR_FILES} "ftp_client.hpp")
set (SRC_FILES ${SRC_FILES} "ftp_client_main.cpp")

# Add source to this project's executable.
add_executable (easy_socket ${HDR_FILES} ${SRC_FILES})

install (FILES ${HDR_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/easy_socket)

# TODO: Add tests and install targets if needed.
