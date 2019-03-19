#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <iostream>
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

  int max_corners(500), max_level(0);
  double quality(0.003), min_dist(50.0);
  while(true)
  {
    cv::Mat prev_img, img, g_img;
    cap >> img;
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_img = prev_imgs.front();

    std::vector<cv::Point2f> prev_corners;
    cv::goodFeaturesToTrack(prev_img, prev_corners, max_corners, quality, min_dist);

    std::vector<unsigned char> status;
    std::vector<cv::Point2f> corners;
    std::vector<float> err;
    cv::calcOpticalFlowPyrLK(prev_img, g_img, prev_corners, corners, status, err, cv::Size(15, 15), max_level);

    int counter (0);
    for(int i(0); i < prev_corners.size(); i++)
    {
      if(status[i] == 1)
      {
        counter++;
        // std::cout << status[i] << std::endl;
        cv::circle(img, prev_corners[i], 3, cv::Scalar(0, 255, 0), -1);
        // cv::circle(img, corners[i], 3, cv::Scalar(0, 0, 255), -1);
        cv::line(img, prev_corners[i], corners[i], cv::Scalar(0, 0, 255), 1);
      }
    }
    std::cout << prev_corners.size() << "\t" << counter << std::endl;

    cv::imshow("MotionField", img);
    cv::waitKey(1);
    prev_imgs.pop();
    prev_imgs.push(g_img);
  }
  cap.release();
}

int main()
{
  skipFrames(1); //Never lose many features. It just doesn't do a good job at detecting them again
  // cv::VideoCapture cap("../MotionFieldVideo.mp4");
  //
  // cv::Mat prev_img, img;
  // cap >> prev_img;
  // cv::cvtColor(prev_img, prev_img, cv::COLOR_BGR2GRAY);
  //
  // int max_corners(500), max_level(0);
  // double quality(0.001), min_dist(50.0);
  // while(true)
  // {
  //   cap >> img;
  //   if(img.empty())
  //     break;
  //   cv::Mat g_img;
  //   cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
  //
  //   std::vector<cv::Point2f> prev_corners;
  //   cv::goodFeaturesToTrack(prev_img, prev_corners, max_corners, quality, min_dist);
  //
  //   std::vector<uchar> status;
  //   std::vector<cv::Point2f> corners;
  //   std::vector<float> err;
  //   cv::calcOpticalFlowPyrLK(prev_img, g_img, prev_corners, corners, status, err, cv::Size(15, 15), max_level);
  //
  //   for(int i(0); i < prev_corners.size(); i++)
  //   {
  //     cv::circle(img, prev_corners[i], 3, cv::Scalar(0, 255, 0), -1);
  //     cv::line(img, prev_corners[i], corners[i], cv::Scalar(0, 0, 255), 1);
  //   }
  //
  //   cv::imshow("MotionField", img);
  //   cv::waitKey(1);
  //
  //   g_img.copyTo(prev_img);
  // }
  // cap.release();

  return 0;
}
