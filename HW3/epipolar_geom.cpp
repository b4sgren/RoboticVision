#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <vector>
#include <iostream>

void readFile(std::string filename, cv::Mat &camera_mat, cv::Mat &dist_coeff)
{
  cv::FileStorage fin(filename, cv::FileStorage::READ);
  fin["Camera_Matrix"] >> camera_mat;
  fin["Distortion_Params"] >> dist_coeff;
  fin.release();
}

void findCorners(cv::Mat imgL, cv::Mat imgR, std::vector<cv::Point2f> &pointsL, std::vector<cv::Point2f> &pointsR)
{
  cv::Size pattern_size(10, 7);

  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);

  std::vector<cv::Point2f> corners;
  findChessboardCorners(imgL, pattern_size, corners, flags);
  pointsL.push_back(corners[7]);
  pointsL.push_back(corners[39]);
  pointsL.push_back(corners[68]);

  corners.clear();
  findChessboardCorners(imgR, pattern_size, corners, flags);
  pointsR.push_back(corners[18]);
  pointsR.push_back(corners[42]);
  pointsR.push_back(corners[54]);
}

int main()
{
  std::string left_img("../calibration_imgs/combinedCalibrate/aL76.bmp");
  std::string right_img("../calibration_imgs/combinedCalibrate/aR76.bmp");

  cv::Mat imgL, imgR, g_imgL, g_imgR;
  imgL = cv::imread(left_img);
  cv::cvtColor(imgL, g_imgL, cv::COLOR_RGB2GRAY);
  imgR = cv::imread(right_img);
  cv::cvtColor(imgR, g_imgR, cv::COLOR_RGB2GRAY);

  cv::Mat camera_matL, dist_coeffL, camera_matR, dist_coeffR;
  readFile("leftIntrinsics.txt", camera_matL, dist_coeffL);
  readFile("rightIntrinsics.txt", camera_matR, dist_coeffR);

  cv::Mat F;
  cv::FileStorage fin("stereo_params.txt", cv::FileStorage::READ);
  fin["F"] >> F;
  fin.release();

  cv::Mat undistL, undistR;
  cv::undistort(g_imgL, undistL, camera_matL, dist_coeffL);
  cv::undistort(g_imgR, undistR, camera_matR, dist_coeffR);

  std::vector<cv::Point2f> pointsL, pointsR;
  findCorners(g_imgL, g_imgR, pointsL, pointsR);

  // std::vector<cv::Point3f> epi_linesR, epi_linesL;
  cv::Mat epi_linesR, epi_linesL;
  cv::computeCorrespondEpilines(pointsL, 1, F, epi_linesR);
  cv::computeCorrespondEpilines(pointsR, 2, F, epi_linesL);

  // cv::imshow("Left", g_imgL);
  // cv::imshow("Right", g_imgR);
  // cv::waitKey();

  return 0;
}
