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

        Mat frame, gray_frame, out_frame;
        while(true)
        {
                video >> frame;
                imshow("Forsgren", frame);

                if(waitKey(30) >= 0)
                {
                        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
                        cvtColor(gray_frame, out_frame, COLOR_GRAY2BGR);
                        imwrite(filename + std::to_string(counter) + filetype, out_frame);
                        counter++;
                }
        }

        return 0;
}
