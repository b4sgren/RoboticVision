#include <opencv2/opencv.hpp>

int main()
{
  cv::VideoCapture cap("../MotionFieldVideo.mp4");

  cv::Mat prev_img, img;
  cap >> prev_img;

  while(true)
  {
    cap >> img;
    if(img.empty())
      break;

    cv::imshow("MotionField", img);
    cv::waitKey(30);

    img.copyTo(prev_img);
  }

  return 0;
}
