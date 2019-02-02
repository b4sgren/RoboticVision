#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <vector>

int main()
{
  cv::FileStorage fin1("camera_params.txt", cv::FileStorage::READ);
  cv::Mat camera_matrix, dst_coeff;
  fin1["Camera_Matrix"] >> camera_matrix;
  fin1["Distortion_Params"] >> dst_coeff;
  fin1.release();

  std::vector<cv::Point3f> world_points;
  std::vector<cv::Point2f> image_points;
  std::ifstream fin("../Data_Points.txt");

  double temp1, temp2, temp3;
  for(int i(0); i<20; i++)
  {
    fin >> temp1 >> temp2;
    image_points.push_back(cv::Point2f(temp1, temp2));
  }
  for(int i(0); i<20; i++)
  {
    fin >> temp1 >> temp2 >> temp3;
    world_points.push_back(cv::Point3f(temp1, temp2, temp3));
  }
  fin.close();

  cv::Mat img = cv::imread("../Object_with_Corners.jpg");

  return 0;
}
