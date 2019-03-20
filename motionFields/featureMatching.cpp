#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
// #include <opencv2/xfeatures2d.hpp> //I don't have this

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
  double quality(0.01), min_dist(10.0);
  cv::Size template_size{15, 15};
  int match_method = cv::TM_SQDIFF;
  while(true)
  {
    cv::Mat prev_img, img, g_img;
    cap >> img;
    std::cout << img.rows << "\t" << img.cols;
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_img = prev_imgs.front();

    std::vector<cv::Point2f> prev_corners;
    cv::goodFeaturesToTrack(prev_img, prev_corners, max_corners, quality, min_dist);

    std::vector<cv::Mat> templates; //Create the template images
    for(cv::Point2f pt : prev_corners)
    {
      std::cout << pt << std::endl;
      cv::Rect roi{pt, template_size};
      cv::Mat temp = g_img(roi);
      // templates.push_back(temp);
    }



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
