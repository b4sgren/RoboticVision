cmake_minimum_required(VERSION 3.4)
project(structureFromMotion)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(unknownParams unknownParams.cpp)
target_link_libraries(unknownParams ${OpenCV_LIBS})

add_executable(knownParams knownParams.cpp)
target_link_libraries(knownParams ${OpenCV_LIBS})

add_executable(task3 task3.cpp)
target_link_libraries(task3 ${OpenCV_LIBS})
