#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>
#include <cmath>

int mouseX, mouseY;

void firstCallback(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        mouseX = x;
        mouseY = y;
        std::cout << "x: " << x << std::endl << "y: " << y << std::endl;
    }
}

void lastCallback(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        mouseX = x;
        mouseY = y;
        std::cout << "x: " << x << std::endl << "y: " << y << std::endl;
    }
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

  cv::FileStorage fin("../camera_params.yaml", cv::FileStorage::READ);
  cv::Mat M, distortion;
  fin["Camera_Matrix"] >> M;
  fin["Distortion_Params"] >> distortion;
  fin.release();
  double fx{M.at<double>(0,0)};
  double fy{M.at<double>(1,1)};
  double ox{M.at<double>(0,2)};
  double oy{M.at<double>(1,2)};

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

    cv::undistortPoints(prev_corners, prev_corners, M, distortion);
    cv::undistortPoints(new_corners, new_corners, M, distortion);
    for(int i(0); i < prev_corners.size(); i++)
    {
      prev_corners[i].x = prev_corners[i].x * fx + ox;
      prev_corners[i].y = prev_corners[i].y * fy + oy;
      new_corners[i].x = new_corners[i].x * fx + ox;
      new_corners[i].y = new_corners[i].y * fy + oy;
    }

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

  cv::Mat status;
  F = cv::findFundamentalMat(orig_pts, final_pts, cv::FM_RANSAC, 3, 0.99, status);

  return F;
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

  std::cout << "F:\n" << F << "\n\n";

  cv::FileStorage fin("../camera_params.yaml", cv::FileStorage::READ);
  cv::Mat M, distortion;
  fin["Camera_Matrix"] >> M;
  fin["Distortion_Params"] >> distortion;
  fin.release();

  cv::Mat E = M.t() * F * M;

  cv::Mat R1, R2, T;
  cv::decomposeEssentialMat(E, R1, R2, T);
  std::cout << "E: " << E << "\n\n";
  // std::cout << "R1: " << R1 << "\n" << "\nR2: " << R2 << "\nT: " << T << "\n\n";

  //Determine Correct combination of R and T. T^ * R = E if E is normalized
  // (R1, T), (R1, -T), (R2, T), (R2, -T)
  double e1 = 3 - abs(R1.at<double>(0,0)) - abs(R1.at<double>(1, 1)) - abs(R1.at<double>(2,2));
  double e2 = 3 - abs(R2.at<double>(0,0)) - abs(R2.at<double>(1, 1)) - abs(R2.at<double>(2,2));

  if (name.substr(0,8) == "Parallel")
  {
    if (e1 < e2)
      std::cout << "R: \n" << R1 << std::endl;
    else
      std::cout << "R: \n" << R2 << std::endl;
  }
  else
  {
    if (R1.at<double>(1,1) > 0)
      std::cout << "R: \n" << R1 << std::endl;
    else
      std::cout << "R: \n" << R2 << std::endl;
  }
  if (T.at<double>(0) > 0)
    std::cout << "T: \n" << T << std::endl;
  else
    std::cout << "T: \n" << -T << std::endl;
}

int main()
{
  performRectification("ParallelCube");
  performRectification("ParallelReal");
  performRectification("TurnCube");
  performRectification("TurnReal");

  return 0;
}
