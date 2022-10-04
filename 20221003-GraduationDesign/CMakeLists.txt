cmake_minimum_required(VERSION 3.2)
project(PacketClassification)

aux_source_directory(. DIRSRCS)

include_directories(./ ./HEMBitsetsMatch ./MultiLevelHash ./io)

add_subdirectory(HEMBitsetsMatch)
add_subdirectory(MultiLevelHash)
add_subdirectory(io)

add_executable(pcRun ${DIRSRCS})
target_link_libraries(pcRun HEMBitsetsMatch MultiLevelHash io)