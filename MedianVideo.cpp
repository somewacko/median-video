/*
 * MedianVideo.cpp
 *
 * Takes a video and processes it through a pixelwise median filter.
 *
 * Usage: MedianVideo input_file output_file [filter_length]
 */

#include <opencv2/opencv.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "MedianFilter.hpp"


void fatal_error(const std::string &message)
{
    std::cerr << message << std::endl;
    exit(EXIT_FAILURE);
}


int main(int argc, char** argv )
{
    // Parse args

    if (argc != 3 && argc != 4)
        fatal_error("Usage: MedianVideo input_file output_file [filter_length]");

    std::string input_filename( argv[1] );
    std::string output_filename( argv[2] );
    int filter_length = (argc == 4 ? std::atoi(argv[3]) : 5);

    if (filter_length <= 0)
        fatal_error("Filter length must be a nonzero positive integer");

    // Open video readers and writers

    cv::VideoCapture video_capture;

    if ( ! video_capture.open(input_filename) )
        fatal_error("Unable to read from " + input_filename);

    cv::VideoWriter video_writer;

    int ex = static_cast<int>(video_capture.get(CV_CAP_PROP_FOURCC));
    cv::Size s = cv::Size((int)video_capture.get(CV_CAP_PROP_FRAME_WIDTH),
                          (int)video_capture.get(CV_CAP_PROP_FRAME_HEIGHT));

    video_writer.open(output_filename, ex, video_capture.get(CV_CAP_PROP_FPS), s, true);

    if ( ! video_writer.isOpened() )
        fatal_error("Unable to write to " + output_filename);

    // Loop through and process frames

    // Printing vars
    int count = 0;
    const int total = video_capture.get(CV_CAP_PROP_FRAME_COUNT);
    const int width = std::to_string(total).size();

    std::cout << std::endl;
    std::cout << "'" << input_filename << "' -> '" << output_filename << "'" << std::endl;
    std::cout << "Filter length: " << filter_length << std::endl;
    std::cout << std::endl;

    MedianFilter filter(filter_length);

    while (true)
    {
        std::cout << "\rProcessing frame "
            << std::setw(width) << count++ << "/"
            << std::setw(width) << total   << " " << std::flush;

        cv::Mat frame;
        video_capture >> frame;

        if ( ! frame.empty() )
            video_writer << filter.process_frame(frame);
        else
            break;
    }
    std::cout << std::endl << std::endl;

    return EXIT_SUCCESS;
}

