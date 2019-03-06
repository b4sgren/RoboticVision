#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

int main()
{
  std::string filenameL{"../../HW3/calibration_imgs/combinedCalibrate/aL29.bmp"};
  std::string filenameR{"../../HW3/calibration_imgs/combinedCalibrate/aR29.bmp"};

  cv::Mat imgL, imgR, g_imgL, g_imgR;
  imgL = cv::imread(filenameL);
  imgR = cv::imread(filenameR);

  cv::cvtColor(imgL, g_imgL, cv::COLOR_BGR2GRAY);
  cv::cvtColor(imgR, g_imgR, cv::COLOR_BGR2GRAY);

  cv::Size pattern_size(10, 7);
  std::vector<cv::Point2f> cornersL, cornersR;
  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
  bool foundL = cv::findChessboardCorners(g_imgL, pattern_size, cornersL, flags);
  bool foundR = cv::findChessboardCorners(g_imgR, pattern_size, cornersR, flags);

  cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);
  cv::cornerSubPix(g_imgL, cornersL, cv::Size(5,5), cv::Size(-1,-1), criteria);
  cv::cornerSubPix(g_imgR, cornersR, cv::Size(5,5), cv::Size(-1,-1), criteria);

  cv::imshow("Left", imgL);
  cv::imshow("Right", imgR);
  cv::waitKey(0);

  return 0;
}
