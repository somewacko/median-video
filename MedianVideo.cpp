/*
 * MedianVideo.cpp
 *
 * Takes a video and processes it through a pixelwise median filter.
 *
 * Usage: MedianVideo input_file output_file [filter_length]
 */

#include <opencv2/opencv.hpp>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include "MedianFilter.hpp"


void fatal_error(const std::string &message)
{
    std::cerr << message << std::endl;
    exit(EXIT_FAILURE);
}


std::string sec_to_time(const double time)
{
    std::stringstream ss;
    ss << std::setw(4) << (int)time/60 << ":" << (int)time%60;
    return ss.str();
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
    int fps = video_capture.get(CV_CAP_PROP_FPS);

    video_writer.open(output_filename, ex, fps, s, true);

    if ( ! video_writer.isOpened() )
        fatal_error("Unable to write to " + output_filename);

    // Loop through and process frames

    // Printing vars
    int count = 0;
    const int total = video_capture.get(CV_CAP_PROP_FRAME_COUNT);
    const int width = std::to_string(total).size();
    double total_time = 0;

    std::cout << std::endl;
    std::cout << "'" << input_filename << "' -> '" << output_filename << "'" << std::endl;
    std::cout << "Filter length: " << filter_length << std::endl;
    std::cout << std::endl;

    MedianFilter filter(filter_length);

    while (true)
    {
        std::cout << "\rProcessing frame "
            << std::setw(width) << ++count << "/"
            << std::setw(width) << total   << " " << std::flush;

        cv::Mat frame;
        video_capture >> frame;

        if ( ! frame.empty() )
        {
            clock_t tic, toc;

            if (count == 1)
                filter.init_median_lists(frame.total());

            tic = std::clock();
            video_writer << filter.process_frame(frame);
            toc = std::clock();

            total_time += double(tic-toc) / CLOCKS_PER_SEC;

            std::cout << "| Avg time: "
                << std::fixed << std::setprecision(2)
                << total_time / count << "s ";

            std::cout << "| Est. time left: "
                << std::setw(8)
                << (total-count)*(total_time/count) << "s ";
        }
        else
            break;
    }
    std::cout << std::endl << std::endl;

    return EXIT_SUCCESS;
}

