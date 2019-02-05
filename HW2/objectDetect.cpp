#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <vector>
#include <iostream>

int main()
{
  cv::FileStorage fin1("camera_params.txt", cv::FileStorage::READ);
  cv::Mat camera_matrix, dst_coeff;
  fin1["Camera_Matrix"] >> camera_matrix;
  fin1["Distortion_Params"] >> dst_coeff;
  fin1.release();

  std::vector<cv::Point3f> world_points(20);
  std::vector<cv::Point2f> image_points(20);
  std::ifstream fin("../Data_Points.txt");

  double temp1, temp2, temp3;
  for(int i(0); i<20; i++)
  {
    fin >> temp1 >> temp2;
    image_points[i] = cv::Point2f(temp1, temp2);
  }
  for(int i(0); i<20; i++)
  {
    fin >> temp1 >> temp2 >> temp3;
    world_points[i] = cv::Point3f(temp1, temp2, temp3);
  }
  fin.close();

  cv::Mat r_vec, t_vec;
  cv::solvePnP(world_points, image_points, camera_matrix, dst_coeff, r_vec, t_vec);

  cv::Mat R;
  cv::Rodrigues(r_vec, R);

  std::cout << "R : = " << R << std::endl;
  std::cout << "T : = " << t_vec << std::endl;

  return 0;
}
