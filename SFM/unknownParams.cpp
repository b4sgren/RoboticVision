#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

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

cv::Mat getFundamentalMat(std::string filename, int n_frames)
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

  int temp(11), side(5);
  int s_side = 11 * side;
  cv::Size template_size{side, side};
  cv::Size search_size{s_side, s_side};
  int match_method = cv::TM_SQDIFF_NORMED;
  for(int i(temp); i < temp + n_frames; i++)
  {
    img = cv::imread(filename + std::to_string(i) + filetype);
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    new_corners.clear();
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

  return F;
}

void performRectification(std::string name)
{
  std::string path("../");
  std::string filename(path + name + "/" + name);

  cv::Mat F = getFundamentalMat(filename, 5);
}

int main()
{
  performRectification("ParallelCube");
  // performRectification("ParallelReal");
  // performRectification("TurnedCube");
  // performRectification("TurnedReal");

  return 0;
}
