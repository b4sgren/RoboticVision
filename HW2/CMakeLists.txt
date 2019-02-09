cmake_minimum_required(VERSION 3.4)
project(Homework2)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(detectCorners corner_detection.cpp)
target_link_libraries(detectCorners ${OpenCV_LIBS})

add_executable(intrinsics intrinsic_params.cpp)
target_link_libraries(intrinsics ${OpenCV_LIBS})

add_executable(distortion distortion.cpp)
target_link_libraries(distortion ${OpenCV_LIBS})

add_executable(object objectDetect.cpp)
target_link_libraries(object ${OpenCV_LIBS})

add_executable(my_distort my_distortion.cpp)
target_link_libraries(my_distort ${OpenCV_LIBS})
