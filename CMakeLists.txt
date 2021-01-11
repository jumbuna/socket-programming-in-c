cmake_minimum_required(VERSION 3.0)

project(sock VERSION 1.0 LANGUAGES C)

add_executable(socket-client socket-client.c)

add_executable(socket-server socket-server.c)

install(TARGETS socket-client DESTINATION bin)

install(TARGETS socket-server DESTINATION bin)