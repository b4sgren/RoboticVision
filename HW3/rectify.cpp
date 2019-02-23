#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

int main()
{
  std::string left_img("../calibration_imgs/combinedCalibrate/aL76.bmp");
  std::string right_img("../calibration_imgs/combinedCalibrate/aR76.bmp");

  cv::Mat imgL, imgR, g_imgL, g_imgR;
  imgL = cv::imread(left_img);
  cv::cvtColor(imgL, g_imgL, cv::COLOR_RGB2GRAY);
  imgR = cv::imread(right_img);
  cv::cvtColor(imgR, g_imgR, cv::COLOR_RGB2GRAY);

  return 0;
}
