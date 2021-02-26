#ifndef TRACKST_HPP
#define TRACKST_HPP

#include <cstdlib>
#include <vector>
#include <queue>
#include <deque>
#include <opencv2/opencv.hpp>
#include <chrono>

/**
 * @class MediaInfo
 * @brief Class for video properties
*/
class MediaInfo
{
public:
	double fps; //**< Frames per second */
	double frames; //**< Total video frames */
	int width; //**< Video width */
	int height; //**< Video hight */
	int channels; //**< Number of color channels */
};

/**
 * @class FrameRGB
 * @brief Structure for frame of type RGB
*/
class FrameRGB
{
public:
	int width;//**< Frame width */
	int height;//**< Frame hight */
	int channels;//**< Number of color channels */
	double frame_number;//**< Position of frame on a video */
	unsigned char *data; //**< Data pointer */
};

/**
 * @class TrackPoint
 * @brief Class for a tracking point
*/
class TrackPoint
{
public:
	cv::Point point;
	int frame_number;
	int label;
	int state;
	std::string timestamp;
	
	//overload of < to use it in sorting of a vector  
	int operator < (const TrackPoint& tp) 
	{
		 return ((frame_number < tp.frame_number));
	}

	//overload of == for comparison
	bool operator == (const TrackPoint& tp) 
	{
		return ((label==tp.label) && (frame_number==tp.frame_number));
	}

};

using MatBuffer = std::deque<cv::Mat>; //**< Alias for a queue buffer holding Mats */

#endif
