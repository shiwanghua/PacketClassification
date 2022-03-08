cmake_minimum_required(VERSION 3.11)
#set(CMAKE_C_COMPILER /usr/bin/gcc)
project(hello)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "-O3")
#set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#find_package(Threads REQUIRED)

#aux_source_directory(. SRC_LIST)
file(GLOB_RECURSE SRC_LIST "src/*.c" "src/*.h")
add_executable(hello ${SRC_LIST})
#add_executable(hello src/main.c src/read.c)