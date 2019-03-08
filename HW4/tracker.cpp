#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>

cv::Mat absoluteDifference(cv::Mat gray_frame, cv::Mat prev_frame);
cv::Mat computeThreshold(cv::Mat gray_frame, int thresh, int high_val, int type);
cv::Mat cleanUpNoise(cv::Mat noisy_img);

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

  cv::Mat g_imgL, g_imgR;
  for(int i(1); i < num_files; i++)
  {
    std::string file_num;
    if(i < 10)
      file_num = "0" + std::to_string(i);
    else
      file_num = std::to_string(i);

    imgL = cv::imread(filenameL + file_num + file_ext);
    imgR = cv::imread(filenameR + file_num + file_ext);
    cv::cvtColor(imgL, g_imgL, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imgR, g_imgR, cv::COLOR_BGR2GRAY);

    g_imgL = absoluteDifference(g_imgL, backgroundL);
    g_imgR = absoluteDifference(g_imgR, backgroundR);

    g_imgL = computeThreshold(g_imgL, 17, 255, 0);
    g_imgR = computeThreshold(g_imgR, 17, 255, 0);

    g_imgL = cleanUpNoise(g_imgL);
    g_imgR = cleanUpNoise(g_imgR);

    cv::imshow("Left", g_imgL);
    cv::imshow("Right", g_imgR);
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
