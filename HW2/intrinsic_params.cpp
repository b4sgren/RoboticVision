#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <cmath>

int main()
{
  // std::string path("../calibration_images/AR");
  std::string path("../my_calibration_images/AR");
  std::string filetype(".jpg");
  int num_pics(25); //41 for Dr. Lee's camera

  cv::Mat img, g_img;
  std::vector<std::vector<cv::Point3f>> object_points; //vector of points of corners in 3D space. All of these are the same
  std::vector<std::vector<cv::Point2f>> image_points; //vector of vectors with corner locations
  // cv::Size pattern_size(10, 7); //For Dr. Lees
  cv::Size pattern_size(9, 7);
  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
  cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.01);

  //This will generate the object points which will be the same for all images
  std::vector<cv::Point3f> points_3d;
  for(int i(0); i < pattern_size.height; i++)
  {
    for(int j(0); j < pattern_size.width; j++)
    {
      //Assume the width of each square is 2 inches
      points_3d.push_back(cv::Point3f(1.0*j, 1.0*i, 0.0));
    }
  }

  for(int i(1); i<num_pics; i++)
  {
    img = cv::imread(path + std::to_string(i) + filetype);
    cv::cvtColor(img, g_img, cv::COLOR_RGB2GRAY);

    std::vector<cv::Point2f> corners;
    corners.clear();
    bool pattern_found = findChessboardCorners(g_img, pattern_size, corners, flags);
    if(!pattern_found)
    {
      std::cout << "Failed on picutre " << i << std::endl;
      return -1;
    }
    cornerSubPix(g_img, corners, cv::Size(3, 3), cv::Size(-1, -1), criteria);

    image_points.push_back(corners);
    object_points.push_back(points_3d);
  }

  cv::Mat camera_matrix, dst_coeffs;
  std::vector<cv::Mat> r_vecs, t_vecs;
  cv::calibrateCamera(object_points, image_points, g_img.size(), camera_matrix, dst_coeffs, r_vecs, t_vecs);

  //Get focal length camera_mat = [fx s cx; 0 fy cy; 0 0 1]
  double pix_size = 1; //4.7e-6; Use for Dr. Lees camera

  double fx = camera_matrix.at<double>(0, 0) * pix_size;
  double fy = camera_matrix.at<double>(1, 1) * pix_size;
  // double f = sqrt(fx*fx + fy*fy) * 1000; //For dr lees camera
  double f = sqrt(fx*fx + fy*fy); //Don't know specs for mine

  std::cout << "Focal Length in mm : " << f << std::endl << std::endl;

  std::cout << "Intrinsic Parameters:\n" << camera_matrix << std::endl << std::endl;
  std::cout << "Distortion Parameters:\n" << dst_coeffs << std::endl << std::endl;

  // cv::FileStorage fout("camera_params.txt", cv::FileStorage::WRITE);
  cv::FileStorage fout("my_camera_params.txt", cv::FileStorage::WRITE);
  fout << "Camera_Matrix" << camera_matrix << "Distortion_Params" << dst_coeffs;
  fout.release();

  return 0;
}
