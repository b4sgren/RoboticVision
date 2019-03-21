#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
// #include <opencv2/xfeatures2d.hpp> //I don't have this

#include <iostream>
#include <vector>
#include <queue>

cv::Point2f getPoint(cv::Point2f pt, int side, cv::Mat img)
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

  return cv::Point2f(x, y);
}

void skipFrames(int n_frames)
{
  std::queue<cv::Mat> prev_imgs;
  cv::VideoCapture cap("../MotionFieldVideo.mp4");

  int max_corners(500), side(5);
  int s_side = 11 * side;
  double quality(0.01), min_dist(25.0);
  cv::Size template_size{side, side};
  cv::Size search_size{s_side, s_side};
  int match_method = cv::TM_SQDIFF_NORMED;
  while(true)
  {
    cv::Mat prev_img, img, g_img;
    cap >> img;
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    g_img.copyTo(prev_img);

    std::vector<cv::Point2f> prev_corners;
    cv::goodFeaturesToTrack(prev_img, prev_corners, max_corners, quality, min_dist);
    int counter(0);

    std::vector<cv::Point2f> new_corners;
    for(int i(0); i < n_frames; i++)
    {
      cap >> img;
      if(img.empty())
        break;
      cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

      //creates a template for each pt to search the img
      for(cv::Point2f pt : prev_corners)
      {
        cv::Point2f pt2 = getPoint(pt, side, img);
        cv::Rect roi{pt2, template_size};
        cv::Mat templ = prev_img(roi);

        cv::Point2f search_pt = getPoint(pt, s_side, img);
        cv::Rect search_roi{search_pt, search_size};
        cv::Mat search_img = g_img(search_roi);

        int result_cols = search_img.cols - templ.cols + 1;
        int result_rows = search_img.rows - templ.rows + 1;
        cv::Mat result;
        result.create(result_rows, result_cols, CV_32FC1);
        cv::matchTemplate(search_img, templ, result, match_method);

        cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        double minVal; double maxVal;
        cv::Point matchLoc, minLoc, maxLoc;
        cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
        matchLoc.x = pt.x - result_cols/2.0 + minLoc.x;
        matchLoc.y = pt.y - result_rows/2.0 + minLoc.y;

        new_corners.push_back(matchLoc);

        // cv::circle(img, pt, 3, cv::Scalar(0, 255, 0), -1);
        // cv::line(img, pt, matchLoc, cv::Scalar(0, 0, 255), 1);
      }

      cv::Mat status;
      cv::Mat F = cv::findFundamentalMat(prev_corners, new_corners, cv::FM_RANSAC, 3, 0.99, status);

      //iterate through each pt and determine if the match is good
      // for(int j(0); j <status.size(); j++)
      // {
      //   if(status[i])
      // }

      counter++;
      cv::imshow("MotionField", img);
      cv::waitKey(1);
      g_img.copyTo(prev_img);
    }
  }
  cap.release();
}

int main()
{
  skipFrames(1); //number of sequential frames to match images in
  // skipFrames(10);

  return 0;
}
