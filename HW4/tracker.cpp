#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>

int main()
{
  std::string filenameL("../baseball/ballL"), filenameR("../baseball/ballR");
  std::string file_ext(".bmp");
  int num_files(100);

  cv::Mat imgL, imgR, backgroundL, backgroundR;
  imgL = cv::imread(filenameL + "00" + file_ext);
  imgR = cv::imread(filenameR + "00" + file_ext);
  cv::cvtColor(imgL, backgroundL, cv::COLOR_BGR2GRAY);
  cv::cvtColor(imgR, backgroundR, cv::COLOR_BGR2GRAY);

  cv::imshow("Left", imgL);
  cv::imshow("Right", imgR);
  cv::waitKey(0);

  return 0;
}
