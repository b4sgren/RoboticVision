#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

int main()
{
  cv::Mat img = cv::imread("../calibration_images/AR1.jpg");
  cv::Mat g_img;
  cv::cvtColor(img, g_img, cv::COLOR_RGB2GRAY);

  std::vector<cv::Point2f> corners;
  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
  cv::Size window(10, 7);
  bool pattern_found = cv::findChessboardCorners(img, window, corners, flags);

  cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.01);
  cv::cornerSubPix(g_img, corners, cv::Size(3, 3), cv::Size(-1, -1), criteria);

  cv::drawChessboardCorners(img, window, corners, pattern_found);

  cv::imwrite("task1.jpg", img);

  cv::imshow("Calibration", img);
  cv::waitKey();

  return 0;
}
