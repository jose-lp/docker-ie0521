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
 * @brief Structure for the tracking object data
*/
class TrackPoint
{
public:
	cv::Point point; //**< Location point */
	int frame_number; //**< Frame number */
	int label; //**< Id/number assigned as label */
	std::vector< cv::Point > contorno; //**< Contour made of points */
};

/**
 * @class Edge
 * @brief Edge representation for a weighted relation between two nodes
*/
class Edge
{
public:
	int ubid; //**< U node identification */
	int vbid; //**< V node identification */
	int uframe; //**< U node frame number */
	int vframe; //**< V node frame number */
	double weight; //**< Edge weight */
};

using MatBuffer = std::deque<cv::Mat>; //**< Alias for a deque buffer holding Mats */

#endif
