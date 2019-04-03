cmake_minimum_required(VERSION 3.4)
project(TimeToImpact)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(task1 task1.cpp)
target_link_libraries(task1 ${OpenCV_LIBS})
