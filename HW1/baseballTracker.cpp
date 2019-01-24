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

Mat cleanUpNoise(Mat noisy_img)
{
  Mat img;
  Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
  erode(noisy_img, img, element);
  // element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(0, 0));
  dilate(img, img, element);

  return img;
}

int main()
{
  std::string filepath("../baseballPics/");
  std::string filename("1L");
  std::string filetype(".jpg");
  std::string number("05");

  Mat img1, img2, diff;

  for(int i(0); i<2; i++)
  {
    if(i == 1)
    {
      filename = "1R";
      number = "05";
    }

    img1 = imread(filepath + filename + number + filetype);
    for(int j(6); j<41; j++)
    {
      if(j<10)
        number = "0" + std::to_string(j);
      else
        number = std::to_string(j);

      img2 = imread(filepath + filename + number + filetype);
      diff = absoluteDifference(img1, img2);
      diff = computeThreshold(diff, 15, 255, 0);

      diff = cleanUpNoise(diff);
      imshow("Baseball", diff);

      // img1 = img2;
      waitKey(0);
    }
  }

  return 0;
}
