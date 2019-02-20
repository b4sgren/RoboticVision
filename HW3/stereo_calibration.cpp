#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

int main()
{
  std::string left_imgs("../calibration_imgs/combinedCalibrate/aL");
  std::string right_imgs("../calibration_imgs/combinedCalibrate/aR");

  std::string filetype(".bmp");
  int num_pics(100);

  cv::FileStorage fin("leftIntrinsics.txt", cv::FileStorage::READ);
  cv::Mat camera_matrixL, dst_coeffsL;
  fin["Camera_Matrix"] >> camera_matrixL;
  fin["Distortion_Params"] >> dst_coeffsL;
  fin.release();

  fin.open("rightIntrinsics.txt", cv::FileStorage::READ);
  cv::Mat camera_matrixR, dst_coeffsR;
  fin["Camera_Matrix"] >> camera_matrixR;
  fin["Distortion_Params"] >> dst_coeffsR;
  fin.release();

  cv::Size pattern_size(9, 7);
  double size = 3.88636;

  //This will generate the object points which will be the same for all images
  std::vector<cv::Point3f> points_3d;
  for(int i(0); i < pattern_size.height; i++)
  {
    for(int j(0); j < pattern_size.width; j++)
    {
      //Assume the width of each square is 2 inches
      points_3d.push_back(cv::Point3f(size*j, size*i, 0.0));
    }
  }

  cv::Mat imgL, g_imgL, imgR, g_imgR;
  std::vector<std::vector<cv::Point3f>> object_points;
  std::vector<std::vector<cv::Point2f>> image_pointsL;
  std::vector<std::vector<cv::Point2f>> image_pointsR;
  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
  cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01);
  std::string filenameL, filenameR;

  for(int i(0); i<num_pics; i++)
  {
    if(i < 10)
    {
      filenameL = left_imgs + std::to_string(0) + std::to_string(i) + filetype;
      filenameR = right_imgs + std::to_string(0) + std::to_string(i) + filetype;
    }
    else
    {
      filenameL = left_imgs + std::to_string(i) + filetype;
      filenameR = right_imgs + std::to_string(i) + filetype;
    }
    imgL = cv::imread(filenameL);
    cv::cvtColor(imgL, g_imgL, cv::COLOR_RGB2GRAY);
    imgR = cv::imread(filenameR);
    cv::cvtColor(imgR, g_imgR, cv::COLOR_RGB2GRAY);

    std::vector<cv::Point2f> cornersL, cornersR;
    bool pattern_foundL = findChessboardCorners(g_imgL, pattern_size, cornersL, flags);
    bool pattern_foundR = findChessboardCorners(g_imgR, pattern_size, cornersR, flags);

    if(pattern_foundL && pattern_foundR)
    {
      cornerSubPix(g_imgL, cornersL, cv::Size(3, 3), cv::Size(-1, -1), criteria);
      cornerSubPix(g_imgR, cornersR, cv::Size(3, 3), cv::Size(-1, -1), criteria);
      image_pointsL.push_back(cornersL);
      image_pointsR.push_back(cornersR);
      object_points.push_back(points_3d);
    }
  }

  //stereo calibration
  cv::Mat R, E, T, F, per_view_errors;
  cv::stereoCalibrate(object_points, image_pointsL, image_pointsR,
                      camera_matrixL, dst_coeffsL, camera_matrixR, dst_coeffsR, pattern_size,
                      R, T, E, F, per_view_errors, cv::CALIB_FIX_INTRINSIC, criteria);

  std::cout << "R: " << R << "\nT: " << T << std::endl;

  return 0;
}
