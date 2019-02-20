#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

void intrinsicCalibrate(std::string input_file, std::string output_file);

int main()
{
  std::string left_imgs_file("../calibration_imgs/leftCalibrate/aL");
  std::string left_output_file("leftIntrinsics.txt");
  std::string right_imgs_file("../calibration_imgs/rightCalibrate/aR");
  std::string right_output_file("rightIntrinsics.txt");

  intrinsicCalibrate(left_imgs_file, left_output_file); //Done so don't need to call again
  intrinsicCalibrate(right_imgs_file, right_output_file); // Done so don't need to call again

  return 0;
}

void intrinsicCalibrate(std::string input_file, std::string output_file)
{
  std::string filetype(".bmp");
  int num_pics(100); //my camera

  cv::Mat img, g_img;
  std::vector<std::vector<cv::Point3f>> object_points; //vector of points of corners in 3D space. All of these are the same
  std::vector<std::vector<cv::Point2f>> image_points; //vector of vectors with corner locations

  cv::Size pattern_size(9, 7);
  double size = 3.88636; // See baseball pdf for last 3 digits

  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
  cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.01);

  //This will generate the object points which will be the same for all images
  std::vector<cv::Point3f> points_3d;
  for(int i(0); i < pattern_size.height; i++)
  {
    for(int j(0); j < pattern_size.width; j++)
    {
      points_3d.push_back(cv::Point3f(size*j, size*i, 0.0));
    }
  }

  std::string filename;
  for(int i(0); i<num_pics; i++)
  {
    if(i < 10)
      filename = input_file + std::to_string(0) + std::to_string(i) + filetype;
    else
      filename = input_file + std::to_string(i) + filetype;

    img = cv::imread(filename);
    cv::cvtColor(img, g_img, cv::COLOR_RGB2GRAY);

    std::vector<cv::Point2f> corners;
    corners.clear();
    bool pattern_found = findChessboardCorners(g_img, pattern_size, corners, flags);
    if(pattern_found)
    {
      cornerSubPix(g_img, corners, cv::Size(3, 3), cv::Size(-1, -1), criteria);
      image_points.push_back(corners);
      object_points.push_back(points_3d);
    }
  }
  cv::Mat camera_matrix, dst_coeffs;
  std::vector<cv::Mat> r_vecs, t_vecs;
  cv::calibrateCamera(object_points, image_points, g_img.size(), camera_matrix, dst_coeffs, r_vecs, t_vecs);

  std::cout << "Intrinsic Parameters:\n" << camera_matrix << std::endl << std::endl;
  std::cout << "Distortion Parameters:\n" << dst_coeffs << std::endl << std::endl;

  cv::FileStorage fout(output_file, cv::FileStorage::WRITE);
  fout << "Camera_Matrix" << camera_matrix << "Distortion_Params" << dst_coeffs;
  fout.release();
}
