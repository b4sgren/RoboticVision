#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

void readFile(std::string filename, cv::Mat &camera_mat, cv::Mat &dist_coeff)
{
  cv::FileStorage fin(filename, cv::FileStorage::READ);
  fin["Camera_Matrix"] >> camera_mat;
  fin["Distortion_Params"] >> dist_coeff;
  fin.release();
}

int main()
{
  std::string left_img("../calibration_imgs/combinedCalibrate/aL76.bmp");
  std::string right_img("../calibration_imgs/combinedCalibrate/aR76.bmp");

  cv::Mat camera_matL, dist_coeffL, camera_matR, dist_coeffR;
  readFile("leftIntrinsics.txt", camera_matL, dist_coeffL);
  readFile("rightIntrinsics.txt", camera_matR, dist_coeffR);

  return 0;
}
