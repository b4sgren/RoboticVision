#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
// #include <opencv2/xfeatures2d.hpp>

#include <iostream>
#include <vector>
#include <queue>

void skipFrames(int n_frames)
{
  std::queue<cv::Mat> prev_imgs;
  cv::VideoCapture cap("../MotionFieldVideo.mp4");

  for(int i(0); i < n_frames; i++)
  {
    cv::Mat img, g_img;
    cap >> img;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_imgs.push(g_img);
  }

  int min_hessian(400), max_features(500); //min hessian for SURF. dont have xfeatures2d module
  cv::BFMatcher matcher{cv::NORM_HAMMING};
  cv::Ptr<cv::DescriptorExtractor> extractor = cv::ORB::create();
  cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create(); //detector for ORB features
  while(true)
  {
    cv::Mat prev_img, img, g_img;
    cap >> img;
    if(img.empty())
      break;
    cv::cvtColor(img, g_img, cv::COLOR_BGR2GRAY);
    prev_img = prev_imgs.front();

    //will use BF. Look into using FLANN and why I can use SURF/SIFT features
    std::vector<cv::KeyPoint> pts1, pts2; //1 is for the previous, 2 is for the current image
    cv::Mat descriptors1, descriptors2;

    detector->detect(prev_img, pts1);
    detector->detect(g_img, pts2);

    extractor->compute(prev_img, pts1, descriptors1);
    extractor->compute(g_img, pts2, descriptors2);

    //Matching
    std::vector<cv::DMatch> matches;
    cv::Mat temp;
    matcher.match(descriptors1, descriptors2, matches);

    //Drawing matches. Try good matches thing on SO
    for(cv::DMatch match : matches)
    {
       cv::circle(img, pts1[match.trainIdx].pt, 3, cv::Scalar(0, 255, 0), -1);
       cv::line(img,pts1[match.trainIdx].pt, pts2[match.queryIdx].pt, cv::Scalar(0, 0, 255), 1);
       //Not drawing lines right. Now sure how to get it to match up
    }

    cv::imshow("MotionField", img);
    cv::waitKey(1);
    prev_imgs.pop();
    prev_imgs.push(g_img);
  }
  cap.release();
}

int main()
{
  skipFrames(1);

  return 0;
}
