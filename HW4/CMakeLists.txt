cmake_minimum_required(VERSION 3.4)
project(Homework4)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(3dmes 3dmes.cpp)
target_link_libraries(3dmes ${OpenCV_LIBS})

add_executable(tracker tracker.cpp)
target_link_libraries(tracker ${OpenCV_LIBS})
