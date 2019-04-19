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
  double sf{1.0};
  std::ofstream fout{"../PracticeSequenceEstimate.txt"};
  for(int i{1}; i < filenames.size(); i++)
  {
    img = cv::imread(filenames[i]);
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    features.clear();
    key_frame_features.clear();
    getFeatures(g_key_frame, key_frame_features); //Maybe try a different method to get more features
    matchFeatures(g_key_frame, g_img, key_frame_features, features);
    E = cv::findEssentialMat(key_frame_features, features, M); //This seems pretty slow. May want faster alternative
    cv::recoverPose(E, key_frame_features, features, M, R, T);
    T *= sf;

    //write R and T to a file
    fout << R.at<double>(0,0) << "\t" << R.at<double>(0,1) << "\t" << R.at<double>(0,2) << "\t" << T.at<double>(0,0) << "\t";
    fout << R.at<double>(1,0) << "\t" << R.at<double>(1,1) << "\t" << R.at<double>(1,2) << "\t" << T.at<double>(1,0) << "\t";
    fout << R.at<double>(2,0) << "\t" << R.at<double>(2,1) << "\t" << R.at<double>(2,2) << "\t" << T.at<double>(2,0) << "\t\n";

    cv::imshow("Frame", img);
    cv::waitKey(1);

    g_img.copyTo(g_key_frame);
  }
  fout.close();

  return 0;
}

void getFeatures(const cv::Mat &img, std::vector<cv::Point2f> &corners)
{
  int max_corners{500};
  double quality{0.02}, min_dist{10};
  cv::goodFeaturesToTrack(img, corners, max_corners, quality, min_dist);
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
  int index_correction{0};
  for(int i{0}; i < status.size(); i++)
  {
    cv::Point2f pt = corners[i-index_correction];
    if(status[i] == 0 || pt.x < 0 || pt.y < 0)
    {
      if(pt.x<0 || pt.y < 0)
        status[i] = 0;
      key_frame_corners.erase(key_frame_corners.begin() + i - index_correction);
      corners.erase(key_frame_corners.begin() + i - index_correction);
      index_correction++;
    }
  }
}
