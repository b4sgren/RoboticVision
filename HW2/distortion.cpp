#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <iostream>
#include <vector>

std::vector<cv::Mat> undistortImage(std::string filename, cv::Mat camera_matrix, cv::Mat dst_coeff)
{
  cv::Mat img, undst_img;
  img = cv::imread(filename);
  cv::undistort(img, undst_img, camera_matrix, dst_coeff);

  return std::vector<cv::Mat>{img, undst_img};
}

cv::Mat absoluteDifference(cv::Mat gray_frame, cv::Mat prev_frame)
{
  // cv::cvtColor(gray_frame, gray_frame, cv::COLOR_RGB2GRAY);
  // cv::cvtColor(prev_frame, prev_frame, cv::COLOR_RGB2GRAY);
  cv::Mat image;
  cv::absdiff(prev_frame, gray_frame, image);

  return image;
}

int main()
{
  cv::FileStorage fin("camera_params.txt", cv::FileStorage::READ);
  cv::Mat camera_matrix, dst_coeffs;
  fin["Camera_Matrix"] >> camera_matrix;
  fin["Distortion_Params"] >> dst_coeffs;
  fin.release();

  std::vector<cv::Mat> far = undistortImage("../Far.jpg", camera_matrix, dst_coeffs);
  std::vector<cv::Mat> close = undistortImage("../Close.jpg", camera_matrix, dst_coeffs);
  std::vector<cv::Mat> turn = undistortImage("../Turned.jpg", camera_matrix, dst_coeffs);

  imshow("Far", absoluteDifference(far[0], far[1]));
  imshow("Close", absoluteDifference(close[0], close[1]));
  imshow("Turned", absoluteDifference(turn[0], turn[1]));
  cv::waitKey();

  return 0;
}
