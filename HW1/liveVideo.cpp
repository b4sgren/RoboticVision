#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;

int main()
{
        VideoCapture video(0);

        if(!video.isOpened())
                std::cout << "Cannot open camera\n";

        while(true)
        {
                Mat frame;
                video >> frame;
//                namedWindow("Forsgren", CV_WINDOW_AUTOSIZE);
                imshow("Forsgren", frame);

                if(waitKey(30) >= 0)
                        break;
        }

        return 0;
}

