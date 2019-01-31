#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

int main()
{
  std::string path("../calibration_images/AR");
  std::string filetype(".jpg");

  cv::Mat img, g_img;
  std::vector<std::vector<cv::Point3f>> object_points; //vector of points of corners in 3D space. All of these are the same
  std::vector<std::vector<cv::Point2f>> image_points; //vector of vectors with corner locations
  cv::Size pattern_size(10, 7);
  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
  cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.01);

  //This will generate the object points which will be the same for all images
  std::vector<cv::Point3f> points_3d;
  for(int i(0); i < pattern_size.height; i++)
  {
    for(int j(0); j < pattern_size.width; j++)
    {
      //Assume the width of each square is 2 inches
      points_3d.push_back(cv::Point3f(2.0*j, 2.0*i, 0.0));
    }
  }

  for(int i(1); i<41; i++)
  {
    img = cv::imread(path + std::to_string(i) + filetype);
    cv::cvtColor(img, g_img, cv::COLOR_RGB2GRAY);

    std::vector<cv::Point2f> corners;
    corners.clear();
    bool pattern_found = findChessboardCorners(g_img, pattern_size, corners, flags);
    cornerSubPix(g_img, corners, cv::Size(3, 3), cv::Size(-1, -1), criteria);

    image_points.push_back(corners);
    object_points.push_back(points_3d);
  }

  return 0;
}
