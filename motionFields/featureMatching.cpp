#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
// #include <opencv2/xfeatures2d.hpp>

#include <iostream>
#include <vector>
#include <queue>

void skipFrames(int n_frames)
{
  std::queue<cv::Mat> prev_imgs;
  cv::VideoCapture cap("../MotionFieldVideo.mp4");

  for(int i(0); i < n_frames; i++)
  {
    cv::Mat img, g_img;
    cap >> img;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_imgs.push(g_img);
  }

  int max_corners(500);
  double quality(0.003), min_dist(50.0), min_eig(0.005);
  cv::TermCriteria crit{cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01};
  while(true)
  {
    cv::Mat prev_img, img, g_img;
    cap >> img;
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_img = prev_imgs.front();

    // int counter (0);
    // for(int i(0); i < prev_corners.size(); i++)
    // {
    //   if(status[i] == 1)
    //   {
    //     counter++;
    //     // std::cout << status[i] << std::endl;
    //     cv::circle(img, prev_corners[i], 3, cv::Scalar(0, 255, 0), -1);
    //     // cv::circle(img, corners[i], 3, cv::Scalar(0, 0, 255), -1);
    //     cv::line(img, prev_corners[i], corners[i], cv::Scalar(0, 0, 255), 1);
    //   }
    // }
    // std::cout << prev_corners.size() << "\t" << counter << std::endl;

    cv::imshow("MotionField", img);
    cv::waitKey(1);
    prev_imgs.pop();
    prev_imgs.push(g_img);
  }
  cap.release();
}

int main()
{
  skipFrames(1);

  return 0;
}
