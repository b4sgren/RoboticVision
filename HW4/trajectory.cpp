#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include <vector>
#include <string>
#include <cmath>

cv::Mat absoluteDifference(cv::Mat gray_frame, cv::Mat prev_frame);
cv::Mat computeThreshold(cv::Mat gray_frame, int thresh, int high_val, int type);
cv::Mat cleanUpNoise(cv::Mat noisy_img);
void readFile(std::string filename, cv::Mat &camera_mat, cv::Mat &dst_c);
cv::SimpleBlobDetector::Params setupParams();
void averageKeyPoints(std::vector<cv::KeyPoint> centers, double &center_x, double &center_y);

int main()
{
  std::string filenameL("../baseball/ballL"), filenameR("../baseball/ballR");
  std::string file_ext(".bmp");
  int num_files(100);

  cv::Mat imgL, imgR, backgroundL, backgroundR;
  imgL = cv::imread(filenameL + "00" + file_ext);
  imgR = cv::imread(filenameR + "00" + file_ext);

  cv::cvtColor(imgL, backgroundL, cv::COLOR_BGR2GRAY);
  cv::cvtColor(imgR, backgroundR, cv::COLOR_BGR2GRAY);

  int box_size(100);
  cv::Rect roiL(325, 50, box_size, box_size);
  imgL = imgL(roiL);

  cv::Rect roiR(225, 50, box_size, box_size);
  imgR = imgR(roiR);

  cv::SimpleBlobDetector::Params params = setupParams();
  cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

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
  cv::stereoRectify(camera_matL, dst_coeffL, camera_matR, dst_coeffR, backgroundL.size(),
                    R, T, R1, R2, P1, P2, Q);

  cv::Mat g_imgL, g_imgR;
  int counter(0);
  bool ball_foundL(false), ball_foundR(false);
  for(int i(20); i < 100; i++)
  {
    std::string file_num;
    // if(i < 10) needed if starting from 0
    //   file_num = "0" + std::to_string(i);
    // else
    file_num = std::to_string(i);

    imgL = cv::imread(filenameL + file_num + file_ext);
    imgR = cv::imread(filenameR + file_num + file_ext);

    cv::cvtColor(imgL, g_imgL, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imgR, g_imgR, cv::COLOR_BGR2GRAY);

    g_imgL = g_imgL(roiL);
    g_imgR = g_imgR(roiR);

    cv::Mat cropped_backL = backgroundL(roiL);
    cv::Mat cropped_backR = backgroundR(roiR);

    cv::Mat binL, binR;
    binL = absoluteDifference(g_imgL, cropped_backL);
    binR = absoluteDifference(g_imgR, cropped_backR);

    binL = computeThreshold(binL, 20, 255, 0);
    binR = computeThreshold(binR, 20, 255, 0);

    binL = cleanUpNoise(binL);
    binR = cleanUpNoise(binR);

    std::vector<cv::KeyPoint> centersL, centersR;
    detector->detect(binL, centersL);
    detector->detect(binR, centersR);

    if(centersL.size() !=0)
      ball_foundL = true;
    else
      ball_foundL = false;
    if(centersR.size() !=0)
      ball_foundR = true;
    else
      ball_foundR = false;

    if(ball_foundL && ball_foundR)
    {
      //Average the blob keypts to determine center of ball
      double center_x_L(0.0), center_y_L(0.0);
      averageKeyPoints(centersL, center_x_L, center_y_L); //Returns the average in the roi
      center_x_L += roiL.x; // Convert position back to the original img
      center_y_L += roiR.y;


      double center_x_R(0.0), center_y_R(0.0);
      averageKeyPoints(centersR, center_x_R, center_y_R);
      center_x_R += roiR.x;
      center_y_R += roiR.y;

      roiL.x = floor(center_x_L) - box_size/2.0;
      roiL.y = floor(center_y_L) - box_size/2.0;

      roiR.x = floor(center_x_R) - box_size/2.0;
      roiR.y = floor(center_y_R) - box_size/2.0;

      cv::circle(imgL, cv::Point2f(center_x_L, center_y_L), 30, cv::Scalar(0, 0, 255), 3, 8);
      cv::circle(imgR, cv::Point2f(center_x_R, center_y_R), 30, cv::Scalar(0, 0, 255), 3, 8);

      //undistort and rectify pts
      std::vector<cv::Point2f> outputL, outputR;
      std::vector<cv::Point2f> ptsL{cv::Point2f(center_x_L, center_y_L)};
      std::vector<cv::Point2f> ptsR{cv::Point2f(center_x_R, center_y_R)};
      cv::undistortPoints(ptsL, outputL, camera_matL, dst_coeffL, R1, P1);
      cv::undistortPoints(ptsR, outputR, camera_matR, dst_coeffR, R2, P2);

      std::cout << outputL[0].x << "\t" << outputR[0].x << std::endl;
      //Note: The output y values are not the same (probably b/c keypts do not match exactly)

      //Do perspective transform
      std::vector<cv::Point3f> perspL;
      perspL.push_back(cv::Point3f(outputL[0].x, outputL[0].y, outputL[0].x - outputR[0].x));

      std::vector<cv::Point3f> finalL;
      cv::perspectiveTransform(perspL, finalL, Q);

      // std::cout << finalL[i] << std::endl;
    }

    cv::imshow("Left", binL);
    cv::imshow("Right", binR);
    // cv::imshow("LeftI", imgL);
    // cv::imshow("RightI", imgR);
    cv::waitKey(0);
  }

  return 0;
}

cv::Mat absoluteDifference(cv::Mat gray_frame, cv::Mat prev_frame)
{
  cv::Mat image;
  cv::absdiff(prev_frame, gray_frame, image);

  return image;
}

cv::Mat computeThreshold(cv::Mat gray_frame, int thresh, int high_val, int type)
{
  cv::Mat image;
  cv::threshold(gray_frame, image, thresh, high_val, type);

  return image;
}

cv::Mat cleanUpNoise(cv::Mat noisy_img)
{
  cv::Mat img;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
  cv::erode(noisy_img, img, element);
  cv::dilate(img, img, element);

  return img;
}

void readFile(std::string filename, cv::Mat& camera_mat, cv::Mat& dst_c)
{
  cv::FileStorage fin(filename, cv::FileStorage::READ);
  fin["Camera_Matrix"] >> camera_mat;
  fin["Distortion_Params"] >> dst_c;
  fin.release();
}

cv::SimpleBlobDetector::Params setupParams()
{
    cv::SimpleBlobDetector::Params params;
    params.minThreshold = 100;
    params.maxThreshold = 255;
    params.filterByColor = true;
    params.blobColor = 255;
    params.filterByArea = false;
    params.filterByCircularity = false;
    params.filterByConvexity = false;
    params.filterByInertia = false;

    return params;
}

void averageKeyPoints(std::vector<cv::KeyPoint> centers, double &center_x, double &center_y)
{
  for(int i(0); i < centers.size(); i++)
  {
    center_x += centers[i].pt.x;
    center_y += centers[i].pt.y;
  }
  center_x /= centers.size();
  center_y /= centers.size();
}
