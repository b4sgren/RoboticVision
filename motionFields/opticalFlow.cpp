#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <iostream>

int main()
{
  cv::VideoCapture cap("../MotionFieldVideo.mp4");

  cv::Mat prev_img, img;
  cap >> prev_img;
  cv::cvtColor(prev_img, prev_img, cv::COLOR_BGR2GRAY);

  int max_corners(500);
  double quality(0.01), min_dist(50.0);
  while(true)
  {
    cap >> img;
    if(img.empty())
      break;
    cv::Mat g_img;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    std::vector<cv::Point2f> prev_corners;
    cv::goodFeaturesToTrack(prev_img, prev_corners, max_corners, quality, min_dist);

    std::vector<uchar> status;
    std::vector<cv::Point2f> corners;
    std::vector<float> err;
    cv::calcOpticalFlowPyrLK(prev_img, g_img, prev_corners, corners, status, err);

    // cv::imshow("MotionField", img);
    // cv::waitKey(30);

    g_img.copyTo(prev_img);
  }

  return 0;
}
