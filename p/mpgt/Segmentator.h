/** @file Segmentator.h
 *  @brief Routines for segmentation of soccer frames.
 *
 *  @details Routines for segmentation of soccer frames
 *  @bug Por determinar
 */
#ifndef MGT_SEGMENTATOR_HPP
#define MGT_SEGMENTATOR_HPP

#include "Structs.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>
#include <string>
#include "PrisSerial.h"

#define PRISMODE 1
#define MOG2MODE 2
#define PRISMOG2MODE 3

using namespace std;
using namespace cv;

/**
 * @class Segmentator
 * @brief Class for the segmentation procedures and data structures.
*/
class Segmentator
{

private:
	///Data members
	int mode; //**< Segmentation mode */	
	
	///Methods
	vector< vector< cv::Point > > PrisSegmentator( cv::Mat img_rgb );
	void MOG2UpdateFgModel( cv::Mat frame );
	vector< vector< cv::Point > > MOG2Segmentator( cv::Mat img_rgb );
	vector< vector< cv::Point > > PrisMOG2Segmentator( cv::Mat img_rgb );
public:
	///Data members
	int hue_min; //**< Min hue value considered as green */	
	int hue_max; //**< Max hue value considered as green */	
	int porcentaje_espurias; //**< Spurious percent of contours to filter */	
	int area_max_threshold; //**<  */	
	int porcentaje_espuriasmog; //**< Spurious percent of contours to filter for MOG segmentator */	
	int tamano; //**< Kernel size */	
	int umbral_hue; //**< Hue threshold */	
	int history; //**< History value for MOG */	
	int learningRate; //**< Learning rate for MOG */	
	int varThreshold; //**< varThreshold for MOG */	
	int bShadowDetection; //**< Enable shadow detection for MOG */	
	int gaussian_kernel; //**< Gaussian blur kernel size for MOG */	
	int enable_morph_line_remover; //**< Enable line removal using morphological operations */	
	int hmorph_axis; //**< Horizontal morph line kernel size remover */	
	int vmorph_axis; //**< Vertical morph line kernel size remover */	
	int enable_hough_line_remover; //**< Enable line removal using the Hough transformation */	
	int line_kernel; //**< Kernel for the morphologic operation on lines */	
	int minLineLength; //**< The minimum number of points that can form a line */	
	int maxLineGap; //**< The maximum gap between two points to be considered in the same line. */	
	int shape_contour_remover; //**< Enable shape contour remover using a circularity criterion */	
	int enable_morph_operations; //**< Enable morphological operations */	
	int morph_erode_kernel; //**< Morphological erode kernel size */	
	int morph_dilate_kernel; //**< Morphological dilate kernel size */	
	int video_height; //**< Video height */	
	int video_width; //**< Video width */	
	int	enable_manual_field_selection; //**< Enable filter of blobs out of the corner points */	
	int enable_white_remover;  //**< Enable white color remover, used to remove field lines */	
	Point	top_left_point; //**< Top left corner point for filtering */	
	Point	top_right_point; //**< Top right corner point for filtering */	
	Point	bottom_left_point; //**< Bottom left corner point for filtering */	
	Point	bottom_right_point;	//**< Bottom right corner point for filtering */	
	Mat foreground_model; //**< Foreground model from MOG */	
	Ptr<BackgroundSubtractor> mogbg; //**< MOG object */	

	///Methods
	Segmentator();
	Segmentator(std::map<std::string,int> parameters);

	vector< vector< cv::Point > > Segment( cv::Mat img_rgb );
	~Segmentator();

			
};

#endif
