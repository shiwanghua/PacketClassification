cmake_minimum_required(VERSION 3.2)
project(PacketClassification)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -march=native -O3 -mavx2 ")
set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. DIRSRCS)

include_directories(./ BruteForceSearch HEMBitsetsSearch MultiLevelHash TamaSearch ./io)

add_subdirectory(BruteForceSearch)
add_subdirectory(HEMBitsetsSearch)
add_subdirectory(MultiLevelHash)
add_subdirectory(TamaSearch)
add_subdirectory(io)

add_executable(pcRun ${DIRSRCS})
target_link_libraries(pcRun BruteForceSearch HEMBitsetsSearch MultiLevelHash TamaSearch io)