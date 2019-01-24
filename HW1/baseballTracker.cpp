#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>

using namespace cv;

Mat computeThreshold(Mat gray_frame, int thresh, int high_val, int type)
{
  Mat image;
  threshold(gray_frame, image, thresh, high_val, type);

  return image;
}

Mat absoluteDifference(Mat gray_frame, Mat prev_frame)
{
  Mat image;
  absdiff(prev_frame, gray_frame, image);

  return image;
}

int main()
{
  std::string filepath("../baseballPics/");
  std::string filename("1L35.jpg");
  std::string filename2("1L36.jpg");

  Mat img1 = imread(filepath + filename);
  Mat img2 = imread(filepath + filename2);

  Mat diff = absoluteDifference(img1, img2);
  diff = computeThreshold(diff, 3, 255, 0);

  Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(0, 0));
  erode(diff, diff, element);
  dilate(diff, diff, element);

  imshow("Baseball", diff);

  waitKey(0);

  return 0;
}
