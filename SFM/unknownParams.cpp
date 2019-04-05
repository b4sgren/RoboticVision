#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>

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

std::vector<cv::Point2f> templateMatching(std::vector<cv::Point2f> &prev_corners,const cv::Mat &prev_img,const cv::Mat &g_img)
{
  std::vector<cv::Point2f> new_corners;
  int side(5);
  int s_side = 11 * side;
  cv::Size template_size{side, side};
  cv::Size search_size{s_side, s_side};
  int match_method = cv::TM_SQDIFF_NORMED;
  for(cv::Point2f pt : prev_corners)
  {
    cv::Point2f pt2 = getPoint(pt, side, prev_img);
    cv::Rect roi{pt2, template_size};
    cv::Mat templ = prev_img(roi);

    cv::Point2f search_pt = getPoint(pt, s_side, prev_img);
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
  }

  return new_corners;
}

cv::Mat getFundamentalMat(std::string filename, int n_frames, std::vector<cv::Point2f>& orig_pts,
                          std::vector<cv::Point2f>& final_pts)
{
  cv::Mat F;
  std::vector<cv::Point2f> pts;
  std::string filetype(".jpg");

  cv::Mat img, g_img, prev_img;

  img = cv::imread(filename + "10" + filetype);
  cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
  prev_img = g_img.clone();

  std::vector<cv::Point2f> prev_corners, orig_corners, new_corners;
  int max_corners(500);
  double quality(0.01), min_dist(25.0);
  cv::goodFeaturesToTrack(prev_img, prev_corners, max_corners, quality, min_dist);
  orig_corners = prev_corners;
  int counter(0);

  int temp(11);
  for(int i(temp); i < temp + n_frames; i++)
  {
    img = cv::imread(filename + std::to_string(i) + filetype);
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    new_corners = templateMatching(prev_corners, prev_img, g_img);

    cv::Mat status;
    std::cout << prev_corners.size() << "\t" << new_corners.size() << "\n";
    F = cv::findFundamentalMat(prev_corners, new_corners, cv::FM_RANSAC, 3, 0.99, status);

    // iterate through each pt and determine if the match is good
    prev_corners.clear();
    std::vector<cv::Point2f> temp;
    for(int j(0); j < status.rows; j++)
    {
      if(status.at<uchar>(j,0))
      {
        prev_corners.push_back(new_corners[j]);
        temp.push_back(orig_corners[j]);
      }
    }
    orig_corners = temp;
    g_img.copyTo(prev_img);
  }
  orig_pts = orig_corners;
  final_pts = prev_corners;

  return F;
}

void drawLines(cv::Mat &imgL, cv::Mat &imgR, std::vector<cv::Point2f> pts1, std::vector<cv::Point2f> pts2)
{

  cv::Scalar color(0, 0, 255);
  double x1(-10), x2(800);
  for(cv::Point2f point : pts1)
    cv::line(imgL, cv::Point2f(x1, point.y), cv::Point2f(x2, point.y), color);
  for(cv::Point2f point : pts2)
    cv::line(imgR, cv::Point2f(x1, point.y), cv::Point2f(x2, point.y), color);
}

void performRectification(std::string name)
{
  std::string path("../");
  std::string filename(path + name + "/" + name);

  cv::Mat img = cv::imread(filename + "5.jpg");
  cv::Mat img2 = cv::imread(filename + "10.jpg");

  cv::Mat g_img, g_img2;
  cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
  cv::cvtColor(img2, g_img2, cv::COLOR_BGR2GRAY);

  std::vector<cv::Point2f> orig_pts, final_pts;
  cv::Mat F = getFundamentalMat(filename, 5, orig_pts, final_pts);

  cv::Mat H1, H2;
  bool homography_found = cv::stereoRectifyUncalibrated(orig_pts, final_pts, F, img.size(), H1, H2);

  //Estimates for Camera Matrix and distortion params
  cv::FileStorage fin("../camera_params2.yaml", cv::FileStorage::READ);
  cv::Mat M, distortion;
  fin["Camera_Matrix"] >> M;
  fin["Distortion_Params"] >> distortion;
  fin.release();

  cv::Mat R1, R2;
  R1 = M.inv() * H1 * M;
  R2 = M.inv() * H2 * M;

  std::vector<cv::Point2f> pts1{orig_pts[0], orig_pts[25], orig_pts[50]};
  std::vector<cv::Point2f> pts2{final_pts[0], final_pts[25], final_pts[50]};

  cv::Size img_size{640,480};
  cv::Mat map1_1, map1_2, map2_1, map2_2, first_rect, last_rect;
  cv::initUndistortRectifyMap(M, distortion, R1, M, img_size, 5, map1_1, map1_2);
  remap(img, first_rect, map1_1, map1_2, cv::INTER_LINEAR);
  cv::initUndistortRectifyMap(M, distortion, R2, M, img_size, 5, map2_1, map2_2);
  remap(img2, last_rect, map2_1, map2_2, cv::INTER_LINEAR);

  for (int i{1}; i < 21; i++)
    {
      cv::Point left_pt{0, 25*i};
      cv::Point right_pt{800, 25*i};
      cv::line(first_rect,left_pt,right_pt,cv::Scalar(0,0,255),1);
      cv::line(last_rect,left_pt,right_pt,cv::Scalar(0,0,255),1);
    }

  cv::imwrite("task1_10" + name + ".jpg", first_rect);
  cv::imwrite("task1_15" + name + ".jpg", last_rect);
  cv::imshow("Frame5", first_rect);
  cv::imshow("Frame10", last_rect);
  cv::waitKey(0);
}

int main()
{
  performRectification("ParallelCube");
  performRectification("ParallelReal");
  performRectification("TurnCube");
  performRectification("TurnReal");

  return 0;
}
