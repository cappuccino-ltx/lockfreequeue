

cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(simple)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR}/../include)

add_subdirectory(${CMAKE_SOURCE_DIR}/queue)
add_subdirectory(${CMAKE_SOURCE_DIR}/spsc)
add_subdirectory(${CMAKE_SOURCE_DIR}/spmc)
add_subdirectory(${CMAKE_SOURCE_DIR}/mpsc)
add_subdirectory(${CMAKE_SOURCE_DIR}/mpmc)
add_subdirectory(${CMAKE_SOURCE_DIR}/concurrentqueue)