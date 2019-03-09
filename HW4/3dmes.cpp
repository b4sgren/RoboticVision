#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

int main()
{
  std::string filenameL{"../../HW3/calibration_imgs/combinedCalibrate/aL09.bmp"};
  std::string filenameR{"../../HW3/calibration_imgs/combinedCalibrate/aR09.bmp"};

  cv::Mat imgL, imgR, g_imgL, g_imgR;
  imgL = cv::imread(filenameL);
  imgR = cv::imread(filenameR);

  cv::cvtColor(imgL, g_imgL, cv::COLOR_BGR2GRAY);
  cv::cvtColor(imgR, g_imgR, cv::COLOR_BGR2GRAY);

  cv::Size pattern_size(10, 7);
  std::vector<cv::Point2f> cornersL, cornersR;
  int flags(cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
  bool foundL = cv::findChessboardCorners(g_imgL, pattern_size, cornersL, flags);
  bool foundR = cv::findChessboardCorners(g_imgR, pattern_size, cornersR, flags);

  cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);
  cv::cornerSubPix(g_imgL, cornersL, cv::Size(5,5), cv::Size(-1,-1), criteria);
  cv::cornerSubPix(g_imgR, cornersR, cv::Size(5,5), cv::Size(-1,-1), criteria);

  cv::FileStorage fin("../leftIntrinsics.txt", cv::FileStorage::READ);
  cv::Mat camera_matL, dst_coeffL;
  fin["Camera_Matrix"] >> camera_matL;
  fin["Distortion_Params"] >> dst_coeffL;
  fin.release();

  fin.open("../rightIntrinsics.txt", cv::FileStorage::READ);
  cv::Mat camera_matR, dst_coeffR;
  fin["Camera_Matrix"] >> camera_matR;
  fin["Distortion_Params"] >> dst_coeffL;
  fin.release();

  fin.open("../stereo_params.txt", cv::FileStorage::READ);
  //Extrinsic Parameters
  cv::Mat R, T, E, F;
  fin["R"] >> R;
  fin["T"] >> T;
  fin["E"] >> E;
  fin["F"] >> F;
  fin.release();

  //Rectification Parameters
  cv::Mat P1, P2, R1, R2, Q;
  cv::stereoRectify(camera_matL, dst_coeffL, camera_matR, dst_coeffR, g_imgL.size(),
                    R, T, R1, R2, P1, P2, Q);

  std::vector<cv::Point2f> ptsL{cornersL[0], cornersL[9], cornersL[69], cornersL[60]};
  std::vector<cv::Point2f> ptsR{cornersR[0], cornersR[9], cornersR[69], cornersR[60]};
  std::vector<cv::Point2f> outputL, outputR;
  cv::undistortPoints(ptsL, outputL, camera_matL, dst_coeffL, R1, P1);
  cv::undistortPoints(ptsR, outputR, camera_matR, dst_coeffR, R2, P2);

  std::vector<cv::Point3f> perspL, perspR;
  for(int i(0); i < outputL.size(); i++)
  {
    perspL.push_back(cv::Point3f(outputL[i].x, outputL[i].y, outputL[i].x - outputR[i].x));
    perspR.push_back(cv::Point3f(outputR[i].x, outputR[i].y, outputL[i].x - outputR[i].x));
  }

  std::vector<cv::Point3f> finalL, finalR;
  cv::perspectiveTransform(perspL, finalL, Q);
  // for(int i(0); i < finalL.size(); i++)
  // {
  //   cv::Mat pt = cv::Mat(finalL[i]);
  //   cv::Mat newPt;
  //   pt.convertTo(newPt, CV_64F);
  //   //Not sure which to use. Is R from left camera to right or from right to left?
  //   // newPt = (R.t() * newPt) + -R.t() * T;
  //   newPt = (R * newPt) - T;
  //   finalR.push_back(cv::Point3f(newPt));
  // }
  cv::perspectiveTransform(perspR, finalR, Q); //x is wrong on this

  std::cout << finalL[0] << finalL[1] << finalL[2] << finalL[3] <<std::endl;
  std::cout << finalR[0] << finalR[1] << finalR[2] << finalR[3] << std::endl;

  for(int i(0); i < finalL.size(); i++)
  {
    cv::circle(imgL, cv::Point2f(ptsL[i].x, ptsL[i].y), 5, cv::Scalar(0, 0, 255), 2, 8);
    cv::circle(imgR, cv::Point2f(ptsR[i].x, ptsR[i].y), 5, cv::Scalar(0, 0, 255), 2, 8);
  }

  cv::imwrite("task1L.jpg", imgL);
  cv::imwrite("task1R.jpg", imgR);

  cv::imshow("Left", imgL);
  cv::imshow("Right", imgR);
  cv::waitKey(0);

  return 0;
}
