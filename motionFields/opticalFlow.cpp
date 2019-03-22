#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <iostream>
#include <queue>
#include <string>

std::vector<cv::Mat> skipFrames(int n_frames, int max_level)
{
  std::queue<cv::Mat> prev_imgs;
  cv::VideoCapture cap("../MotionFieldVideo.mp4");
  std::vector<cv::Mat> frames;

  for(int i(0); i < n_frames; i++)
  {
    cv::Mat img, g_img;
    cap >> img;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_imgs.push(g_img);
  }

  int max_corners(500);
  double quality(0.01), min_dist(10.0), min_eig(0.02);
  cv::TermCriteria crit{cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 40, 0.001};
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
    cv::calcOpticalFlowPyrLK(prev_img, g_img, prev_corners, corners, status, err, cv::Size(21, 21), max_level,
                             crit, 0, min_eig);

    int counter (0);
    for(int i(0); i < prev_corners.size(); i++)
    {
      if(status[i] && err[i] < 20)
      {
        counter++;
        cv::circle(img, prev_corners[i], 3, cv::Scalar(0, 255, 0), -1);
        cv::line(img, prev_corners[i], corners[i], cv::Scalar(0, 0, 255), 1);
      }
    }
    // std::cout << prev_corners.size() << "\t" << counter << std::endl;

    cv::imshow("MotionField", img);
    cv::waitKey(1);
    frames.push_back(img);
    prev_imgs.pop();
    prev_imgs.push(g_img);
  }
  cap.release();

  return frames;
}

void makeVideo(std::vector<cv::Mat> v1, std::vector<cv::Mat> v2, std::string filename)
{
  int ex = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
  cv::Size size(v1[0].rows, v1[0].cols);
  cv::VideoWriter v_out(filename, ex, 30, size, true);

  for(int i(0); i < v1.size(); i++)
    v_out << v1[i];
  for(int i(0); i < v2.size(); i++)
    v_out << v2[i];
  v_out.release();
}

int main()
{
  std::vector<cv::Mat> set1, set2, set3, set4;
  set1 = skipFrames(1, 0); //Never lose many features. It just doesn't do a good job at detecting them again
  set2 = skipFrames(10, 0);
  makeVideo(set1, set2, "task1_1.avi");

  set3 = skipFrames(1, 3);
  set4 = skipFrames(10, 3);
  makeVideo(set3, set4, "task1_2.avi");
  return 0;
}
