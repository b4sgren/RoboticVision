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

  cv::Mat img, g_img, img_prev, g_prev;
  img_prev = cv::imread(path + "1" + filetype);
  cv::cvtColor(img_prev, g_prev, cv::COLOR_BGR2GRAY);
  for(int i(2); i < 19; i++)
  {
    img = cv::imread(path + std::to_string(i) + filetype);
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);

    cv::imshow("Image", img);
    cv::waitKey(0);
  }

  return 0;
}
