/*
 * MedianFilter.hpp
 *
 * Class to implement a pixelwise median filter.
 *
 * (boo hoo c++ in a header wahh it's only like 100 lines of code)
 */

#ifndef __MEDIAN_FILTER_HPP__
#define __MEDIAN_FILTER_HPP__

#include <opencv2/opencv.hpp>

#include <list>


// Struct to store pixel information

typedef struct
{
    cv::Vec3b color;
    uchar gray;
} pixel_t;


// Class definition

class MedianFilter
{
public:

    MedianFilter(int filter_length);
    ~MedianFilter();

    cv::Mat process_frame(const cv::Mat &frame);

private:
    cv::Mat frame;
    int filter_length;
    int rows, cols, size;
    bool pop_front;
    std::list<pixel_t>* median_lists;

    void init_median_lists(int rows, int cols);
};


// MedianFilter public methods

MedianFilter::MedianFilter(int filter_length)
    : filter_length(filter_length)
    , median_lists(nullptr)
    , rows(0)
    , cols(0)
    , size(0)
{ }


MedianFilter::~MedianFilter()
{
    if (median_lists != nullptr)
        delete [] median_lists;
}


cv::Mat MedianFilter::process_frame(const cv::Mat &frame)
{
    // Get grayscale image to do sorting with
    cv::Mat gray_frame;
    cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

    // Initialize the filter if we haven't yet
    if (median_lists == nullptr)
        init_median_lists(frame.rows, frame.cols);

    // Init iterators
    int i = 0;
    cv::MatConstIterator_<cv::Vec3b> bgr_it  = frame.begin<cv::Vec3b>(),
                                     bgr_end = frame.end  <cv::Vec3b>();

    cv::MatConstIterator_<uchar>     gray_it = gray_frame.begin<uchar>();

    // Matrix to stuff the "current frame" into.
    cv::Mat dst(rows, cols, CV_8UC3);
    cv::MatIterator_<cv::Vec3b> dst_it = dst.begin<cv::Vec3b>();

    // Iterate through all pixels in each image together
    for ( ; bgr_it != bgr_end; bgr_it++, gray_it++, dst_it++, i++)
    {
        assert( i < size );

        pixel_t pixel;
        pixel.color = *bgr_it;
        pixel.gray = *gray_it;

        bool was_inserted = false;
        const int med_idx = floor( median_lists[i].size()/2 );

        int count = 0;
        for (auto it = median_lists[i].begin(); it != median_lists[i].end(); it++)
        {
            // Insertion sort the current pixel
            if ( *gray_it < (*it).gray && !was_inserted )
            {
                median_lists[i].insert(it, pixel);
                was_inserted = true;
            }

            // If this is the median in the list, store it now so we don't have
            // to iterate through the list again
            if (count == med_idx)
                *dst_it = (*it).color;

            // If we've inserted the current pixel and found the pixel for the
            // current frame, move on to the next pixel
            if (count >= med_idx && was_inserted)
                break;

            count++;
        }
        if ( ! was_inserted )
            median_lists[i].push_back(pixel);

        // Get rid of extraneous pixels
        if (median_lists[i].size() > filter_length)
        {
            if (pop_front)
                median_lists[i].pop_front();
            else
                median_lists[i].pop_back();
        }
    }

    pop_front = ! pop_front;

    return dst;
}


// MedianFilter private methods

void MedianFilter::init_median_lists(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
    this->size = rows*cols;

    median_lists = new std::list<pixel_t>[size];
}



#endif
