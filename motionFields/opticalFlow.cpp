#include <opencv2/opencv.hpp>

int main()
{
  cv::VideoCapture cap("../MotionFieldVideo.mp4");

  cv::Mat img;

  while(true)
  {
    cap >> img;
    if(img.empty())
      break;

    cv::imshow("MotionField", img);
    cv::waitKey(30);
  }

  return 0;
}
