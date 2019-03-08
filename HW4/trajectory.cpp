#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>

cv::Mat absoluteDifference(cv::Mat gray_frame, cv::Mat prev_frame);
cv::Mat computeThreshold(cv::Mat gray_frame, int thresh, int high_val, int type);
cv::Mat cleanUpNoise(cv::Mat noisy_img);
void readFile(std::string filename, cv::Mat &camera_mat, cv::Mat &dst_c);
cv::Mat findCentroids(cv::Mat diff, cv::Mat img, std::vector<cv::Point2f> &centers);

int main()
{
  std::string filenameL("../baseball/ballL"), filenameR("../baseball/ballR");
  std::string file_ext(".bmp");
  int num_files(100);

  cv::Mat imgL, imgR, backgroundL, backgroundR;
  imgL = cv::imread(filenameL + "00" + file_ext);
  imgR = cv::imread(filenameR + "00" + file_ext);

  cv::Mat camera_matL, dst_coeffL;
  readFile("../leftIntrinsics.txt", camera_matL, dst_coeffL);

  cv::Mat camera_matR, dst_coeffR;
  readFile("../rightIntrinsics.txt", camera_matR, dst_coeffR);

  cv::FileStorage fin("../stereo_params.txt", cv::FileStorage::READ);
  //Extrinsic Parameters
  cv::Mat R, T, E, F;
  fin["R"] >> R;
  fin["T"] >> T;
  fin["E"] >> E;
  fin["F"] >> F;
  fin.release();

  cv::Mat P1, P2, R1, R2, Q;
  cv::stereoRectify(camera_matL, dst_coeffL, camera_matR, dst_coeffR, imgL.size(),
                    R, T, R1, R2, P1, P2, Q);

  cv::Rect roiL(300, 0, 640 - 300, 480);
  cv::Rect roiR(200, 0, 640 - 200, 480);
  imgL = imgL(roiL);
  imgR = imgR(roiR);

  cv::cvtColor(imgL, backgroundL, cv::COLOR_BGR2GRAY);
  cv::cvtColor(imgR, backgroundR, cv::COLOR_BGR2GRAY);

  // cv::imshow("Orig. L", imgL);
  // cv::imshow("Orig. R", imgR);
  // cv::waitKey(0);

  cv::Mat g_imgL, g_imgR;
  //Determine how to detect when the ball first shows up
  for(int i(1); i < 49; i++)
  {
    std::string file_num;
    if(i < 10)
      file_num = "0" + std::to_string(i);
    else
      file_num = std::to_string(i);

    imgL = cv::imread(filenameL + file_num + file_ext);
    imgR = cv::imread(filenameR + file_num + file_ext);

    imgL = imgL(roiL);
    imgR = imgR(roiR);
    cv::cvtColor(imgL, g_imgL, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imgR, g_imgR, cv::COLOR_BGR2GRAY);

    cv::Mat binL, binR;
    binL = absoluteDifference(g_imgL, backgroundL);
    binR = absoluteDifference(g_imgR, backgroundR);

    binL = computeThreshold(binL, 20, 255, 0);
    binR = computeThreshold(binR, 20, 255, 0);

    binL = cleanUpNoise(binL);
    binR = cleanUpNoise(binR);

    std::vector<cv::Point2f> centersL, centersR;
    findCentroids(binL, imgL, centersL);
    findCentroids(binR, imgR, centersR);

    std::vector<cv::Point2f> undstL, undstR;
    cv::undistortPoints(centersL, undstL, camera_matL, dst_coeffL, R1, P1);
    cv::undistortPoints(centersR, undstR, camera_matR, dst_coeffR, R2, P2);

    std::vector<cv::Point3f> perspL, perspR;
    for(int i(0); i < undstL.size(); i++)
    {
      perspL.push_back(cv::Point3f(undstL[i].x + roiL.x, undstL[i].y, undstL[i].x + roiL.x - undstR[i].x - roiR.x));
      perspR.push_back(cv::Point3f(undstR[i].x + roiR.x, undstR[i].y, undstL[i].x + roiL.x - undstR[i].x - roiR.x));
    }

    std::vector<cv::Point3f> finalL;
    cv::perspectiveTransform(perspL, finalL, Q);
    std::cout << finalL[0] << std::endl;

    if((i - 29)%5 == 0)
    {
      cv::imwrite("task2_" + std::to_string(i-29) + "L.jpg", imgL);
      cv::imwrite("task2_" + std::to_string(i-29) + "R.jpg", imgR);
    }

    cv::imshow("Left", imgL);
    cv::imshow("Right", imgR);
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
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
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

cv::Mat findCentroids(cv::Mat diff, cv::Mat img, std::vector<cv::Point2f> &centers)
{
  cv::Mat canny_out;
  cv::Canny(diff, canny_out, 100, 200, 3);
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(canny_out, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

  std::vector<cv::Moments> mu(contours.size());
  std::vector<cv::Point2f> mc(contours.size());
  for(int i(0); i < contours.size(); i++)
  {
    mu[i] = cv::moments(contours[i]);
    centers.push_back(cv::Point2f(static_cast<float>(mu[i].m10/(mu[i].m00+1e-5)),
                    static_cast<float>(mu[i].m01/(mu[i].m00+1e-5))));
    cv::circle(img, centers[i], 30, cv::Scalar(0, 0, 255), 1, 8);
  }

  return img;
}
