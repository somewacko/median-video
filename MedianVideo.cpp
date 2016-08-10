/*
 * MedianVideo.cpp
 *
 * Takes a video and processes it through a pixelwise median filter.
 *
 * Usage:
 *      ./MedianVideo [filename | live]
 */

#include <opencv2/opencv.hpp>

#include <iostream>

#include "MedianFilter.hpp"


int main(int argc, char** argv )
{
    // TODO: Option to process video instead


    // Open camera stream

    cv::VideoCapture video_capture;

    if ( ! video_capture.open(0) )
    {
        std::cout << "Error: Unable to open default camera!" << std::endl;
        return EXIT_FAILURE;
    }

    // TODO: Make command-line arg to change filter length
    MedianFilter filter(10);

    while (true)
    {
        cv::Mat frame;
        video_capture >> frame;

        // TODO: Make command-line arg to change size
        cv::Size size(320, 240);
        cv::resize(frame, frame, size);

        if ( ! frame.empty() )
            cv::imshow("Display", filter.process_frame(frame));
        else
            break;

        if ( cv::waitKey(1) == 27 )
            break;
    }

    return EXIT_SUCCESS;
}

