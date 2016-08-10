cmake_minimum_required(VERSION 2.8)

project( MedianVideo CXX )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( Boost REQUIRED )
find_package( OpenCV REQUIRED )

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( . )

add_executable( MedianVideo MedianVideo.cpp )

target_link_libraries( MedianVideo ${Boost_LIBS} )
target_link_libraries( MedianVideo ${OpenCV_LIBS} )

