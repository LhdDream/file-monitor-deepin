cmake_minimum_required(VERSION 3.16)
project(file_Server)

set(CMAKE_CXX_STANDARD 14)

include_directories(core  net Server thread_pool log )

aux_source_directory(core SOURCE_CORE)
aux_source_directory(net SOURCE_NET)
aux_source_directory(Server SOURCE_SERVER)
aux_source_directory(thread_pool SOURCE_POOL)
aux_source_directory(log SOURCE_LOG)

LINK_LIBRARIES(pthread)

add_definitions("--std=c++14")
add_definitions("-g")


add_executable(file_Server main.cpp
        ${SOURCE_CORE}
        ${SOURCE_NET}
        ${SOURCE_SERVER}
        ${SOURCE_POOL}
         ${SOURCE_LOG})