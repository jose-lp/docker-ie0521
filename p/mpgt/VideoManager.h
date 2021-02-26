/** @file VideoManager.h
 *  @brief Routines to handle media files.
 *
 *  @details Routines that handle video and image files. 
 *  @bug Por determinar
 */
#ifndef MPGT_VMANAGER_HPP
#define MPGT_VMANAGER_HPP

#include "Structs.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>
#include <string>

using namespace std;
using namespace cv;

/** @class VideoManager
 *  @brief Class for the manipulation of video data
 *  @details Class definition for the manipulation of video data using OpenCV.
 */
class VideoManager
{

private:

	MatBuffer mat_deque; //**< Queue of OpenCV Mat objects */
	VideoCapture ivideo; //**< Input video */
	VideoWriter ovideo; //**<  Output video */
	string current_video; //**<  */

public:
	
	MediaInfo bufVideoInfo; //**< Data structure for basic media info */
	int frame_number; //**< Register for the frame number */

	/**Simple contructor*/
	VideoManager();

	/**
	 * @brief Routine to load a video.
	 * @details This routine loads a video, into a queue of OpenCV Mats
	 * @param char * input_video: Pointer to location of video file.
	 * @bug If input file doesnt exist, fails to exit safety.
	 * */
	void SimpleVideoLoader(string input_video);
	
	/**
	 * @brief Routine to save a frame to a video,.
	 * @param Mat frame; Frame to save in the video file.
	 * */
	void SimpleFrameSaver(Mat frame);

	/**
	 * @brief Routine to start the video writer.
	 * @param std:string output_video; Location to save the video
	 * */	
	void StartVideoSaver(string output_video);

	/**
	 * @brief Returns the requested frame
	 * @param int fnumber: Frame number
	 * */		
	cv::Mat GetFrame(int fnumber);
	
	/**
	 * @brief Returns the next frame pointed by the internal frame number register
	 * */	
	cv::Mat GetNextFrame(void);
	
	/**
	 * @brief Returns the requested frame
	 * @param int fnumber: Frame number
	 * */			
	void SetFrame(int fnumber);

	/**
	 * @brief Reload a frame to the buffer
	 * @details Reloads the frame into the Mat vector buffer on the fnumber position.
	 * 			Not used any more because of memory consumption
	 * @param int fnumber: Frame number
	 * */		
	void ReloadFrameFromVideo(int fnumber);

	/**Simple distructor*/
	~VideoManager();

			
};

#endif
