#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;

int main()
{
        VideoCapture video(0);
        std::string filename, filetype;
        filename = "../../HW2/my_calibration_images/AR";
        filetype = ".jpg";
        int counter = 1;

        if(!video.isOpened())
                std::cout << "Cannot open camera\n";

        Mat frame, gray_frame;
        while(true)
        {
                video >> frame;
//                namedWindow("Forsgren", CV_WINDOW_AUTOSIZE);
                imshow("Forsgren", frame);

                if(waitKey(30) >= 0)
                {
                        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
                        imwrite(filename + std::to_string(counter) + filetype, gray_frame);
                        counter++;
                }
        }

        return 0;
}
