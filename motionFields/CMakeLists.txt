cmake_minimum_required(VERSION 3.4)
project(Homework5)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(opticalFlow opticalFlow.cpp)
target_link_libraries(opticalFlow ${OpenCV_LIBS})

add_executable(featureMatch featureMatching.cpp)
target_link_libraries(featureMatch ${OpenCV_LIBS})
