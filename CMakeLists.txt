cmake_minimum_required(VERSION 3.0)

project(sock VERSION 1.0 LANGUAGES C)

add_executable(socket socket.c)

install(TARGETS socket DESTINATION bin)