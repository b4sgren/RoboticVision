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

  int min_hessian(400), max_features(500); //min hessian for SURF can't use
  while(true)
  {
    cv::Mat prev_img, img, g_img;
    cap >> img;
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_img = prev_imgs.front();

    //will use BF. Look into using FLANN and why I can use SURF/SIFT features
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create(); //detector for ORB features
    std::vector<cv::KeyPoint> pts1, pts2; //1 is for the previous, 2 is for the current image
    cv::Mat descriptors1, descriptors2;

    detector->detect(prev_img, pts1);
    detector->detect(g_img, pts2);

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
