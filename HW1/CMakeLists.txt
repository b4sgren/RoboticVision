cmake_minimum_required(VERSION 3.4)
project(Homework1)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(liveVideo liveVideo.cpp)
target_link_libraries(liveVideo ${OpenCV_LIBS})

add_executable(featureDetect featureDetection.cpp)
target_link_libraries(featureDetect ${OpenCV_LIBS})
