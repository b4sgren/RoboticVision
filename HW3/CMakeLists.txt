cmake_minimum_required(VERSION 3.4)
project(Homework3)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(calibration camera_calibration.cpp)
target_link_libraries(calibration ${OpenCV_LIBS})

add_executable(stereo stereo_calibration.cpp)
target_link_libraries(stereo ${OpenCV_LIBS})

add_executable(epipolar epipolar_geom.cpp)
target_link_libraries(epipolar ${OpenCV_LIBS})
