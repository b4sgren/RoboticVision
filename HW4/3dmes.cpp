#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

int main()
{
  std::string filenameL{"../../HW3/calibration_imgs/combinedCalibrate/aL29.bmp"};
  std::string filenameR{"../../HW3/calibration_imgs/combinedCalibrate/aR29.bmp"};

  cv::Mat imgL, imgR;
  imgL = cv::imread(filenameL);
  imgR = cv::imread(filenameR);

  cv::imshow("Left", imgL);
  cv::imshow("Right", imgR);
  cv::waitKey(0);

  return 0;
}
