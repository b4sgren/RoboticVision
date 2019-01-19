#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include <vector>

using namespace cv;

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
  while(true)
  {
    //1280x720 resolution
    Mat frame, gray_frame, diff;
    video >> frame;
    cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

    // int key = (waitKey(0) & 0xFF); //
    //
    // if(key == 'a')
    //   threshold(gray_frame, gray_frame, 125, 255, 0); //For the thresholding part
    // else if(key == 'b')
    //   Canny(gray_frame, gray_frame, 100, 200, 3);
    // else if(key == 'q')
    //   break;

     // threshold(gray_frame, gray_frame, 125, 255, 0); //For the thresholding part

      // Canny(gray_frame, gray_frame, 100, 200, 3); // frame to work on, output, lower bound, upper bound, kernel size? read more about

      //This is the sub pixel corner detection. This isn't working
      // cornerSubPix(gray_frame, gray_frame, Size(5, 5), Size(-1, -1), TermCriteria(2, 100, .001));

      //This is for line detection. Also not working right now
      // std::vector<Vec2f> lines;
      // HoughLines(gray_frame, lines, CV_PI/180.0, 150, 0, 0);
      //
      // for( size_t i = 0; i < lines.size(); i++ )
      // {
      //     float rho = lines[i][0], theta = lines[i][1];
      //     Point pt1, pt2;
      //     double a = cos(theta), b = sin(theta);
      //     double x0 = a*rho, y0 = b*rho;
      //     pt1.x = cvRound(x0 + 1000*(-b));
      //     pt1.y = cvRound(y0 + 1000*(a));
      //     pt2.x = cvRound(x0 - 1000*(-b));
      //     pt2.y = cvRound(y0 - 1000*(a));
      //     line( gray_frame, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
      // }

      //Absolute Difference
      absdiff(prev_frame, gray_frame, diff);

      prev_frame = gray_frame;

      imshow("Forsgren", diff);

      // v_out << frame;
      counter++;

      if(waitKey(30) >= 0)
          break;
  }
  v_out.release();
  video.release();

  return 0;
}
