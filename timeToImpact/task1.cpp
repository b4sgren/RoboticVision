#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>

int main()
{
  std::string path{"../images/T"};
  std::string filetype{".jpg"};

  cv::Mat M, dst;
  cv::FileStorage fin("../camera_params.yaml", cv::FileStorage::READ);
  fin["Camera_Matrix"] >> M;
  fin["Distortion_Params"] >> dst;

  
  return 0;
}
