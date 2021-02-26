/** @file VideoManager.h
 *  @brief Routines to handle media files.
 *
 *  @details Routines that handle video and image files. 
 *  @bug Por determinar
 */

#ifndef VMANAGER_HPP
#define VMANAGER_HPP

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

class VideoManager
{

private:
	
	string current_video;

public:
	
	MediaInfo bufVideoInfo; //**< Data structure for basic media info */
	int frame_number;
	VideoCapture ivideo;
	VideoWriter ovideo;

	/**Simple contructor*/
	VideoManager();
	VideoManager(string output_video);

	/**
	 * @brief Routine to load a video.
	 * @details This routine loads a video, into a queue of OpenCV Mats
	 * @param char * input_video: Pointer to location of video file.
	 * @bug If input file doesnt exist, fails to exit safety.
	 * */
	void SimpleVideoLoader(string input_video);
	
	/**
	 * @brief Routine to start a video recorder.
	 * @param string output_video; Location to output video.
	 * */
	void StartVideoSaver(string output_video);
	void StartVideoSaver(string output_video, int width, int height);

	/**
	 * @brief Routine to save a Mat objects into a video.
	 * @param Mat frame: Frame to save
	 * */
	void SimpleFrameSaver(Mat frame);
	
	cv::Mat GetFrame(int fnumber);
	cv::Mat GetNextFrame(void);
	void SetFrameNumber(int fnumber);
	void ReloadFrameFromVideo(int fnumber);

	/**Simple distructor*/
	~VideoManager();

			
};

#endif
