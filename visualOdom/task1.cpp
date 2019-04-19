#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <glob.h>
#include <iostream>

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

  for(int i{1}; i < filenames.size(); i++)
  {
    img = cv::imread(filenames[i]);
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    cv::imshow("Frame", img);
    cv::waitKey(0);
  }

  return 0;
}
