#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

void readFile(std::string filename, cv::Mat &camera_mat, cv::Mat &dist_coeff)
{
  cv::FileStorage fin(filename, cv::FileStorage::READ);
  fin["Camera_Matrix"] >> camera_mat;
  fin["Distortion_Params"] >> dist_coeff;
  fin.release();
}

cv::Mat absoluteDifference(cv::Mat gray_frame, cv::Mat prev_frame)
{
  cv::Mat image;
  cv::absdiff(prev_frame, gray_frame, image);

  return image;
}

void findCorners(cv::Mat imgL, std::vector<cv::Point2f> &pointsL)
{
  cv::Size pattern_size(10, 7);

  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);

  std::vector<cv::Point2f> corners;
  findChessboardCorners(imgL, pattern_size, corners, flags);
  pointsL.push_back(corners[7]);
  pointsL.push_back(corners[39]);
  pointsL.push_back(corners[68]);
}

void drawLines(cv::Mat &imgL, cv::Mat &imgR)
{
  std::vector<cv::Point2f> points;
  findCorners(imgL, points);
  cv::cvtColor(imgL, imgL, cv::COLOR_GRAY2BGR);
  cv::cvtColor(imgR, imgR, cv::COLOR_GRAY2BGR);

  cv::Scalar color(255, 0, 0);
  double x1(-10), x2(800);
  for(cv::Point2f point : points)
  {
    cv::line(imgL, cv::Point2f(x1, point.y), cv::Point2f(x2, point.y), color);
    cv::line(imgR, cv::Point2f(x1, point.y), cv::Point2f(x2, point.y), color);
  }
}

int main()
{
  std::string left_img("../calibration_imgs/combinedCalibrate/aL76.bmp");
  std::string right_img("../calibration_imgs/combinedCalibrate/aR76.bmp");

  cv::Mat imgL, imgR, g_imgL, g_imgR;
  imgL = cv::imread(left_img);
  cv::cvtColor(imgL, g_imgL, cv::COLOR_RGB2GRAY);
  imgR = cv::imread(right_img);
  cv::cvtColor(imgR, g_imgR, cv::COLOR_RGB2GRAY);

  cv::Mat camera_matL, dist_coeffL, camera_matR, dist_coeffR;
  readFile("leftIntrinsics.txt", camera_matL, dist_coeffL);
  readFile("rightIntrinsics.txt", camera_matR, dist_coeffR);

  cv::Mat F, E, R, T;
  cv::FileStorage fin("stereo_params.txt", cv::FileStorage::READ);
  fin["F"] >> F;
  fin["E"] >> E;
  fin["R"] >> R;
  fin["T"] >> T;
  fin.release();

  cv::Mat P1, P2, R1, R2, Q;
  cv::stereoRectify(camera_matL, dist_coeffL, camera_matR, dist_coeffR, g_imgL.size(),
                    R, T, R1, R2, P1, P2, Q);

  cv::Mat map1L, map2L, map1R, map2R; // Not sure I need 4. His slides make it sound like I just need 2
  cv::initUndistortRectifyMap(camera_matL, dist_coeffL, R1, P1, g_imgL.size(), CV_32FC1, map1L, map2L);
  cv::initUndistortRectifyMap(camera_matR, dist_coeffR, R2, P2, g_imgR.size(), CV_32FC1, map1R, map2R);

  cv::Mat outputL, outputR;
  cv::remap(g_imgL, outputL, map1L, map2L, cv::INTER_LINEAR);
  cv::remap(g_imgR, outputR, map1R, map2R, cv::INTER_LINEAR);

  cv::Mat absL, absR;
  absL = absoluteDifference(g_imgL, outputL);
  absR = absoluteDifference(g_imgR, outputR);

  drawLines(outputL, outputR);

  // cv::imshow("Left Original", g_imgL);
  cv::imshow("Left Remap", outputL);
  // cv::imshow("Left Abs", absL);
  // cv::imshow("Right Original", g_imgR);
  cv::imshow("Right Remap", outputR);
  // cv::imshow("Right Abs", absR);
  cv::waitKey();

  return 0;
}
