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
  dilate(img, img, element);

  return img;
}

Mat findCentroids(Mat diff, Mat img)
{
  Mat canny_out;
  Canny(diff, canny_out, 100, 200, 3);
  std::vector<std::vector<Point>> contours;
  findContours(canny_out, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

  std::vector<Moments> mu(contours.size());
  std::vector<Point2f> mc(contours.size());
  for(int i(0); i< contours.size(); i++)
  {
    mu[i] = moments(contours[i]);
    mc[i] = Point2f(static_cast<float>(mu[i].m10/(mu[i].m00+1e-5)),
                    static_cast<float>(mu[i].m01/(mu[i].m00+1e-5)));
    circle(img, mc[i], 30, Scalar(0, 0, 255), 3, 8);
  }

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
      diff = computeThreshold(diff, 17, 255, 0);
      diff = cleanUpNoise(diff);
      diff = findCentroids(diff, img2);

      imshow("Baseball", diff);

      // img1 = img2;
      waitKey(0);
    }
  }

  return 0;
}
