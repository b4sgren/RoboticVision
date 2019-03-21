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

  int max_corners(500), side(100);
  double quality(0.01), min_dist(10.0);
  cv::Size template_size{side, side};
  int match_method = cv::TM_SQDIFF;
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

    std::vector<cv::Mat> templates; //Create the template images
    for(cv::Point2f pt : prev_corners)
    {
      float x, y;
      if(pt.x > img.cols - side/2.0)
        x = img.cols - side;
      else if(pt.x < side/2.0)
        x = 0;
      else
        x = pt.x- side/2.0;
      if(pt.y > img.rows - side/2.0)
        y = img.rows - side;
      else if(pt.y < side/2.0)
        y = 0;
      else
         y = pt.y - side/2.0;

      cv::Point2f pt2(x, y);
      cv::Rect roi{pt2, template_size};
      cv::Mat temp = prev_img(roi);
      templates.push_back(temp);
    }

    //Find matches
    //https://docs.opencv.org/4.0.1/de/lda9/tutorial_template_matching.html
    for(cv::Mat templ : templates)
    {
      cv::Mat result;
      result.create(g_img.rows, g_img.cols, CV_32FC1);
      cv::matchTemplate(g_img, templ, result, match_method); //This takes forever. How to go faster??
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
