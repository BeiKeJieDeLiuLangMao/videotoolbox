cmake_minimum_required(VERSION 3.10)
project(videotoolbox)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-pthread -lz -ldl -ltheoraenc -ltheoradec")

include(./CMakeModules/FindFFMPEG.cmake)
add_executable(videotoolbox main.cpp)
target_include_directories(videotoolbox PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(videotoolbox PRIVATE ${FFMPEG_LIBRARIES})
