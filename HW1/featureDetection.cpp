#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

#include <vector>

using namespace cv;

Mat computeThreshold(Mat gray_frame, int thresh, int high_val, int low_val)
{
  Mat image;
  threshold(gray_frame, image, 125, 255, 0);

  return image;
}

Mat computeCannyEdges(Mat gray_frame)
{
  Mat image;
  Canny(gray_frame, image, 100, 200, 3); // Look up what last 3 inputs are

  return image;
}

Mat absoluteDifference(Mat gray_frame, Mat prev_frame)
{
  Mat image;
  absdiff(prev_frame, gray_frame, image);

  return image;
}

Mat computeHoughLines(Mat gray_frame)
{
  Mat image;
  gray_frame.copyTo(image);
  //This is for line detection. Code compiles but doesn't run
  std::vector<Vec2f> lines;
  Mat canny_frame = computeCannyEdges(gray_frame);
  HoughLinesP(canny_frame, lines, 1, CV_PI/180.0, 50, 50, 10);
  std::cout << lines.size() << std::endl;
  for( size_t i = 0; i < lines.size(); i++ )
  {
      float rho = lines[i][0], theta = lines[i][1];
      Point pt1, pt2;
      double a = cos(theta), b = sin(theta);
      double x0 = a*rho, y0 = b*rho;
      pt1.x = cvRound(x0 + 1000*(-b));
      pt1.y = cvRound(y0 + 1000*(a));
      pt2.x = cvRound(x0 - 1000*(-b));
      pt2.y = cvRound(y0 - 1000*(a));
      line(image, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
  }

  return image;
}

Mat findSubCorners(Mat gray_frame)
{
  //This is the sub pixel corner detection. Not working
  // cvtColor(gray_frame, gray_frame, CV_32FC1);
  // cornerHarris(gray_frame, gray_frame, 2, 3, 0.04); //in, out, block size, aperture size, k
  // dilate(gray_frame, gray_frame);
  // TermCriteria criteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 30, 0.001);
  // cornerSubPix(gray_frame, gray_frame, Size(3, 3), Size(-1, -1), criteria);

  return gray_frame;
}

int main()
{
  int counter = 0;
  VideoCapture video(0);
  int ex = static_cast<int>(video.get(CAP_PROP_FOURCC));
  VideoWriter v_out("test.avi", ex, 30, Size(640, 480));

  if(!video.isOpened())
      std::cout << "Cannot open camera\n";

  if(!v_out.isOpened())
  {
    std::cout << "Failed to open video\n";
    return -1;
  }

  Mat prev_frame;
  video >> prev_frame;
  cvtColor(prev_frame, prev_frame, COLOR_BGR2GRAY);
  int key, mode(0);
  while(true)
  {
    //1280x720 resolution
    Mat frame, gray_frame, diff, image;
    video >> frame;
    cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

    key = waitKey(30);
    if(key == (int)('t'))
      mode = 1;
    else if(key == (int)('c'))
      mode = 2;
    else if(key == (int)('a'))
      mode = 3;
    else if(key == (int)('s'))
      mode = 4;
    else if(key == (int)('h'))
      mode = 5;
    else if(key == 'q')
      break;

    if(mode == 1)
      image = computeThreshold(gray_frame, 125, 255, 0);
    else if(mode == 2)
      image = computeCannyEdges(gray_frame);
    else if(mode == 3)
      image = absoluteDifference(gray_frame, prev_frame);
    else if(mode == 4)
      image = findSubCorners(gray_frame);
    else if(mode == 5)
    {
      image == computeHoughLines(gray_frame);
      std::cout << image.size().height;
    }
    else
      image = gray_frame;

    prev_frame = gray_frame;

    imshow("Forsgren", image);

    // v_out << frame;
  }
  v_out.release();
  video.release();

  return 0;
}
