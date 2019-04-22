#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <glob.h>
#include <iostream>
#include <fstream>
#include <cmath>

//based off this https://avisingh599.github.io/vision/monocular-vo/

void getFeatures(const cv::Mat &img, std::vector<cv::Point2f> &corners);
void matchFeatures(const cv::Mat &key_frame, const cv::Mat& img, std::vector<cv::Point2f> &key_frame_corners, std::vector<cv::Point2f> &corners);

int main()
{
  std::string filepath{"../HallwayImgs/HallwaySequence/*.png"};
  cv::Mat R_tot = cv::Mat::eye(3, 3, CV_64F);
  cv::Mat T_tot = cv::Mat::zeros(3, 1, CV_64F);

  glob_t result;
  glob(filepath.c_str(), GLOB_TILDE, NULL, &result); //gets all the files
  std::vector<std::string> filenames;
  for(size_t i{0}; i < result.gl_pathc; i++)
    filenames.push_back(std::string(result.gl_pathv[i])); // puts them in order

  cv::FileStorage fin{"../hallwayImgs/CameraParameters.yaml", cv::FileStorage::READ};
  cv::Mat M;
  fin["Camera_Matrix"] >> M;
  fin.release();

  cv::Mat img, g_img, key_frame, g_key_frame;
  key_frame = cv::imread(filenames[0]);
  cv::cvtColor(key_frame, g_key_frame, cv::COLOR_BGR2GRAY);

  std::vector<cv::Point2f> key_frame_features, features;
  cv::Mat E, R, T;
  double sf{1.0};
  std::ofstream fout{"../HallwaySequenceEstimate.txt"};
  for(int i{1}; i < filenames.size(); i+= int(sf))
  {
    img = cv::imread(filenames[i]);
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    getFeatures(g_key_frame, key_frame_features);
    matchFeatures(g_key_frame, g_img, key_frame_features, features);
    std::cout << features.size() << std::endl;
    // cv::Mat mask;
    // E = cv::findEssentialMat(key_frame_features, features, M, cv::RANSAC, 0.999, 0.5, mask);
    // // E = cv::findEssentialMat(features, key_frame_features, M, cv::RANSAC, 0.999, 1.0, mask);
    // cv::recoverPose(E, key_frame_features, features, M, R, T, mask);
    //
    // if(abs(T.at<double>(2)) > abs(T.at<double>(0)) && abs(T.at<double>(2)) > abs(T.at<double>(1)))
    // {
    //   T_tot += -sf * (R_tot * T);
    //   R_tot = R.t() * R_tot;
    //
    //   //write R and T to a file
    //   fout << R_tot.at<double>(0,0) << "\t" << R_tot.at<double>(0,1) << "\t" << R_tot.at<double>(0,2) << "\t" << T_tot.at<double>(0,0) << "\t";
    //   fout << R_tot.at<double>(1,0) << "\t" << R_tot.at<double>(1,1) << "\t" << R_tot.at<double>(1,2) << "\t" << T_tot.at<double>(1,0) << "\t";
    //   fout << R_tot.at<double>(2,0) << "\t" << R_tot.at<double>(2,1) << "\t" << R_tot.at<double>(2,2) << "\t" << T_tot.at<double>(2,0) << "\t\n";
    // }

    cv::imshow("Frame", img);
    cv::waitKey(1);

    g_img.copyTo(g_key_frame);
  }
  fout.close();

  return 0;
}

void getFeatures(const cv::Mat &img, std::vector<cv::Point2f> &corners)
{
  // int max_corners{500};
  // double quality{0.01}, min_dist{10};
  // cv::goodFeaturesToTrack(img, corners, max_corners, quality, min_dist);

  //using FAST Features
  std::vector<cv::KeyPoint> keypoints;
  int threshold{10};
  bool nonmaxSuppression{true};
  cv::FAST(img, keypoints, threshold, nonmaxSuppression);
  cv::KeyPoint::convert(keypoints, corners, std::vector<int>());
}

void matchFeatures(const cv::Mat &key_frame, const cv::Mat& img, std::vector<cv::Point2f> &key_frame_corners, std::vector<cv::Point2f> &corners)
{
  std::vector<uchar> status;
  std::vector<float> error;

  cv::calcOpticalFlowPyrLK(key_frame, img, key_frame_corners, corners, status, error);

  //reject non-matches
  int idx = 0;
  double pix_vel, pix_vel_thresh = 4.0;
  for (int i = 0; i < status.size(); i++)
  {
    cv::Point2f pt;
    pt = corners.at(i - idx);
    if ((status.at(i) == 0) || (pt.x<0) || (pt.y<0))
    {
      key_frame_corners.erase(key_frame_corners.begin() + (i - idx));
      corners.erase(corners.begin() + (i - idx));
      idx++;
    }
    else
    {
      pix_vel = sqrt(pow(corners[i-idx].x - key_frame_corners[i-idx].x, 2) + pow(corners[i-idx].y - key_frame_corners[i-idx].y, 2));
      // Get rid of points if OF isn't greater than threshold
      if (pix_vel < pix_vel_thresh)
      {
        key_frame_corners.erase(key_frame_corners.begin() + (i - idx));
        corners.erase(corners.begin() + (i - idx));
        idx++;
      }
    }
  }
}
