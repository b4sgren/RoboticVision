#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>
#include <vector>

void getFeatures(std::vector<cv::Point2f>& corners, const cv::Mat& img)
{
  corners.clear();
  int max_corners(300);
  double quality(0.01), min_dst(25.0);
  cv::goodFeaturesToTrack(img, corners, max_corners, quality, min_dst);
}

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

void templateMatching(std::vector<cv::Point2f>& corners,const std::vector<cv::Point2f> prev_corners,
                      const cv::Mat& g_prev, const cv::Mat& g_img)
{
  corners.clear();

  int side(5);
  int s_side = 11 * side;
  cv::Size template_size{side, side};
  cv::Size search_size{s_side, s_side};
  int match_method = cv::TM_SQDIFF_NORMED;
  for(cv::Point2f pt : prev_corners)
  {
    cv::Point2f pt2 = getPoint(pt, side, g_img);
    cv::Rect roi{pt2, template_size};
    cv::Mat templ = g_prev(roi);

    cv::Point2f search_pt = getPoint(pt, s_side, g_img);
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

    corners.push_back(matchLoc);
  }
}

int main()
{
  std::string path{"../images/T"};
  std::string filetype{".jpg"};

  cv::Mat M, dst;
  cv::FileStorage fin("../camera_params.yaml", cv::FileStorage::READ);
  fin["Camera_Matrix"] >> M;
  fin["Distortion_Params"] >> dst;

  cv::Mat img, g_img, img_prev, g_prev;
  img_prev = cv::imread(path + "1" + filetype);
  cv::cvtColor(img_prev, g_prev, cv::COLOR_BGR2GRAY);



  std::vector<cv::Point2f> corners, prev_corners;
  for(int i(2); i < 19; i++)
  {
    getFeatures(prev_corners, g_prev);

    img = cv::imread(path + std::to_string(i) + filetype);
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    templateMatching(corners, prev_corners, g_prev, g_img);

    cv::imshow("Image", img);
    cv::waitKey(0);
  }

  return 0;
}
