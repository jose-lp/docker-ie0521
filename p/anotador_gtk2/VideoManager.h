/** @file VideoManager.h
 *  @brief Routines to handle media files.
 *
 *  @details Routines that handle video and image files. 
 *  @bug Por determinar
 */

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

	MatBuffer mat_deque; //**< Queue of OpenCV Mat objects */
	VideoCapture ivideo;
	string current_video;

public:
	
	MediaInfo bufVideoInfo; //**< Data structure for basic media info */
	int frame_number;

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
	 * @brief Routine to save a buffer of Mat objects as a video,.
	 * @param char * output_video; Pointer to location of video file.
	 * */
	void SimpleVideoSaver(MatBuffer &sequence, string output_video);
	
	cv::Mat GetFrame(int fnumber);
	cv::Mat GetNextFrame(void);
	void SetFrame(int fnumber);
	void ReloadFrameFromVideo(int fnumber);

	/**Simple distructor*/
	~VideoManager();

			
};
