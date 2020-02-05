cmake_minimum_required(VERSION 3.16)
project(file_monitor_deepin)

set(CMAKE_CXX_STANDARD 14)

include_directories(src  config )

aux_source_directory(src/hook SOURCE_HOOK)
aux_source_directory(src/client SOURCE_CLIENT)
aux_source_directory(config SOURCE_CONFIG)

LINK_LIBRARIES(pthread)

add_definitions("--std=c++14")
add_definitions("-g")
add_executable(file_monitor_deepin main.cc ${SOURCE_CONFIG}
        ${SOURCE_CLIENT} ${SOURCE_HOOK} )