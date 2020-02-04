cmake_minimum_required(VERSION 3.16)
project(file_Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(file_Server main.cpp net/Socket.h net/Socket.cc net/Epoll.h Server/Server.h Server/Server.cc core/User.h core/User.cc)