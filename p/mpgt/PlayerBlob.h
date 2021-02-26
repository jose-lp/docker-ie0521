/** @file PlayerBlob.h
 *  @brief Class to contain blob features of players
 *
 *  @details Class to contain blob features of players
 *  @bug Por determinar
 */
#ifndef MGT_PLAYERBLOB_HPP
#define MGT_PLAYERBLOB_HPP

#include "Structs.h"
#include "Defs.h"
#include "PrisSerial.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>
#include <string>
#include <math.h>

using namespace std;
using namespace cv;

/**
 * @class PlayerBlob
 * @brief Class for the abstraction of blobs as soccer players, including
 * the properties used as descriptors and the actions related to the construction
 * of the objects.
*/
class PlayerBlob
{

private:

	///data members

	///methods

	/**
	 * @brief ...
	 * @details ...
	 * @param ...
	 * @return ...
	 **/	
	Point2f GetMassCenter(vector< cv::Point > contorno_blob);

	/**
	 * @brief ...
	 * @details ...
	 * @param ...
	 * @return ...
	 **/	
	bool BlobMightBeAPlayer(vector< cv::Point > contorno_blob);
	bool BlobMightBeAPlayer(vector< cv::Point > contorno_blob, cv::Mat contour_histogram);

	/**
	 * @brief ...
	 * @details ...
	 * @param ...
	 * @return ...
	 **/	
	bool BlobMightBeALine(Mat player_blob);
	
public:

	///data members
	int blob_frame_number;
	int blob_id;
	int player_tag;
	int cluster;
	int blob_type;
	double area; //**< Area of the blob object */
	double perimeter; //**< Perimeter of the blob object */
	double ratio; //**< Aspect ratio
	Point2f centroid;
	set< int > blob_labels;
	map< int,int > player_tags;
	set<int> player_tag_set;
	vector< cv::Point > contorno; //**< Contour of the blob
	Mat contourRegion;//**< Contains the color information (RGB) wrapped by the contour
	Mat histogram; //**< Histogram of the HSV ROI 
	std::array<Point2f,5> position_history;
	map< int,Point2f > merged_centroids; //**< Mapaa con los tags y centroides, usado cuando es un merge blob
	double direction;
	double speed;

	///methods
	/**Simple contructor*/
	PlayerBlob();
	PlayerBlob(int fnumber, int id);
	
	/**
	 * @brief Fill the properties of a blob object.
	 * @details Fill the properties that describe a blob object such as shape,
	 *          color and position.
	 * @param int fnumber: Frame number of the blob
	 * @param vector< cv::Point > contorno_blob: Contour of the blob
	 * @param Mat field_image: Mat which represents the frame/field
	 **/			
	void FillModel(int fnumber, vector< cv::Point > contorno_blob, Mat field_image);

	/**
	 * @brief Fill the properties of a blob object.
	 * @details Fill the properties that describe a blob object such as shape,
	 *          color and position.
	 * @param int fnumber: Frame number of the blob
	 * @param int id: Blob id
	 * @param vector< cv::Point > contorno_blob: Contour of the blob
	 * @param Mat field_image: Mat which represents the frame/field
	 **/
	void FillModel(int fnumber, int id, vector< cv::Point > contorno_blob, Mat field_image);

	/**
	 * @brief Get the contour region information
	 * @details Get the contour region information including the color information
	 *          within the contour
	 * @param vector< cv::Point > contorno_blob: Contour of blob
	 * @param Mat field_image: Mat of the field/frame
	 * @return Mat: A mat with the color information in HSV format within the blob contour
	 **/	
	Mat GetContourRegion(vector< cv::Point > contorno_blob, Mat field_image);

	/**
	 * @brief Get the histogram for a blob
	 * @details Get the histogram from a player blob
	 * @param Mat player: Mat that contains color information
	 **/	
	void GetHistogram(Mat player_blob);	

	/**
	 * @brief Show blob information
	 * @details Show the id, area, perimeter and frame number of a blob
	 **/	
	void ShowBlobInfo(void);

	/**
	 * @brief Assign a initialization tag to the blob
	 * @details Assign the closest tag from a list of points of the same
	 *          frame number to the blob.
	 * @param vector< TrackPoint > init_tpoints:
	 * @param int init_tkp_error: Pixel error to consider between the centroid
	 *        of the blob and a tracking point.
	 **/
	void AssignInitTag(vector< TrackPoint > init_tpoints, int init_tkp_threshold);
	
	void SetDirection(Point2f start_point, Point2f end_point);
	void SetBlobType(int type);
	void UpdateDirection(Point2f new_position);
	void UpdateDirection(void);
	void UpdatePositionHistory(Point2f new_position);
	void UpdatePositionHistory(void);
	void ChangeBlobStartPointHistory(Point2f new_start_point);
	void UpdateSpeed(int frames);
	void UpdateSpeed(void);
	
	/**Simple distructor*/
	~PlayerBlob();
};

#endif
