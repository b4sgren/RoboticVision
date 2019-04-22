#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <glob.h>
#include <iostream>
#include <fstream>

//based off this https://avisingh599.github.io/vision/monocular-vo/

void getFeatures(const cv::Mat &img, std::vector<cv::Point2f> &corners);
void matchFeatures(const cv::Mat &key_frame, const cv::Mat& img, std::vector<cv::Point2f> &key_frame_corners, std::vector<cv::Point2f> &corners);

int main()
{
  std::string filepath{"../PracticeImgs/PracticeSequence/*.png"};

  glob_t result;
  glob(filepath.c_str(), GLOB_TILDE, NULL, &result); //gets all the files
  std::vector<std::string> filenames;
  for(size_t i{0}; i < result.gl_pathc; i++)
    filenames.push_back(std::string(result.gl_pathv[i])); // puts them in order

  cv::FileStorage fin{"../PracticeImgs/CameraParameters.yaml", cv::FileStorage::READ};
  cv::Mat M;
  fin["Camera_Matrix"] >> M;
  fin.release();

  cv::Mat img, g_img, key_frame, g_key_frame;
  key_frame = cv::imread(filenames[0]);
  cv::cvtColor(key_frame, g_key_frame, cv::COLOR_BGR2GRAY);

  std::vector<cv::Point2f> key_frame_features, features;
  cv::Mat E, R, T;
  cv::Mat C = cv::Mat::eye(4,4, CV_32F);
  double sf{1.0};
  cv::Rect roi1{cv::Point{0,0}, cv::Size{3,3}}, roi2{cv::Point{0,3}, cv::Size{3,1}};
  std::ofstream fout{"../PracticeSequenceEstimate.txt"};
  for(int i{1}; i < filenames.size(); i+=1)
  {
    img = cv::imread(filenames[i]);
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    features.clear();
    key_frame_features.clear();
    getFeatures(g_key_frame, key_frame_features); //Maybe try a different method to get more features
    matchFeatures(g_key_frame, g_img, key_frame_features, features);  //Issues matching features when using Fast
    E = cv::findEssentialMat(features, key_frame_features, M, cv::RANSAC, 0.999, 0.1);
    cv::recoverPose(E, key_frame_features, features, M, R, T);
    T *= -sf;

    // cv::Mat temp = cv::Mat::eye(4,4, CV_32F);
    // R.copyTo(temp(roi1));
    // // T.copyTo(temp(roi2));
    // temp.at<float>(0, 3) = T.at<float>(0,0);
    // temp.at<float>(1, 3) = T.at<float>(1,0);
    // temp.at<float>(2, 3) = T.at<float>(2,0);
    // std::cout << T << std::endl;
    // C = C * temp;

    //write R and T to a file
    fout << R.at<double>(0,0) << "\t" << R.at<double>(0,1) << "\t" << R.at<double>(0,2) << "\t" << T.at<double>(0,0) << "\t";
    fout << R.at<double>(1,0) << "\t" << R.at<double>(1,1) << "\t" << R.at<double>(1,2) << "\t" << T.at<double>(1,0) << "\t";
    fout << R.at<double>(2,0) << "\t" << R.at<double>(2,1) << "\t" << R.at<double>(2,2) << "\t" << T.at<double>(2,0) << "\t\n";

    cv::imshow("Frame", img);
    cv::waitKey(1);

    g_img.copyTo(g_key_frame);
  }
  // std::cout << "Here\n";
  // fout.close();

  return 0;
}

void getFeatures(const cv::Mat &img, std::vector<cv::Point2f> &corners)
{
  int max_corners{500};
  double quality{0.02}, min_dist{10};
  cv::goodFeaturesToTrack(img, corners, max_corners, quality, min_dist);

  // //using FAST Features
  // std::vector<cv::KeyPoint> keypoints;
  // int threshold{20};
  // bool nonmaxSuppression{true};
  // cv::FAST(img, keypoints, threshold, nonmaxSuppression);
  // cv::KeyPoint::convert(keypoints, corners, std::vector<int>());
}

void matchFeatures(const cv::Mat &key_frame, const cv::Mat& img, std::vector<cv::Point2f> &key_frame_corners, std::vector<cv::Point2f> &corners)
{
  cv::Size window_size{21, 21};
  cv::TermCriteria criteria{cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.001};
  std::vector<uchar> status;
  std::vector<float> error;
  int max_level{3};

  cv::calcOpticalFlowPyrLK(key_frame, img, key_frame_corners, corners, status, error, window_size, max_level, criteria);

  //reject non-matches
  int cols{img.cols}, rows{img.rows};
  std::vector<cv::Point2f> temp1, temp2;
  for(int i{0}; i < status.size(); i++)
  {
    cv::Point2f pt = corners[i];
    if(status[i] == 1)
    {
      temp1.push_back(key_frame_corners[i]);
      temp2.push_back(corners[i]);
    }
  }
  key_frame_corners = temp1;
  corners = temp2;
}
