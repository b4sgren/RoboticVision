cmake_minimum_required(VERSION 3.4)
project(Homework2)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(detectCorners corner_detection.cpp)
target_link_libraries(detectCorners ${OpenCV_LIBS})
