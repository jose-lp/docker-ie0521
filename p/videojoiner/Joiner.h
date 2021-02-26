/** @file Joiner.h
 *  @brief Routines to join video files.
 *
 *  @details Routines to join video files. 
 *  @bug Por determinar
 */
 
#ifndef JOINER_HPP
#define JOINER_HPP

#include "Structs.h"
#include "VideoManager.h"
#include "YamlParser.h"
#include "JGpu.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <opencv2/stitching/stitcher.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include "cv.h"
#include "Defs.h"

using namespace std;
using namespace cv;

class Joiner
{

private:
	Mat shear(const cv::Mat & input, float Bx, float By);
	Mat equalizeColor(const Mat& inputImage);
	Mat ltransform_mat, sltransform_mat;
	Mat rtransform_mat, srtransform_mat;
	int frame2skip;

public:

	bool dropFrame;
	float framerate;
	std::map<std::string,int> config;
	MediaInfo bufVideoInfo;
	
	/**Simple contructor*/
	Joiner();
	Joiner(std::map<std::string,int>  start_config);
	Joiner(std::map<std::string,int>  start_config, MediaInfo vidinfo);
	Joiner(bool df, float fr);
	
	void SetJoiner(bool df, float fr);
	int frameNumber(std::string timecode);
	void synch(VideoCapture &lvid, std::string ltc, VideoCapture &rvid, std::string rtc);
	void synch(VideoManager *lvid, VideoManager *rvid, YamlParser *pyp);
	void synch_manual(VideoCapture &lvid, int left_start_frame, VideoCapture &rvid, int right_start_frame);
	void synch_graph(VideoCapture &lvid, VideoCapture &rvid);
	void synch_graph2(VideoCapture &lvid, VideoCapture &rvid, int start_frame);
	int color_match(Mat *ref_frame, Mat *tgt_frame);
	void histMatch(const Mat &reference, const Mat &target, Mat &result);
	void SimplestCB(Mat& in, Mat& out, float percent);	
	
	/**
	* @brief Pega dos imágenes usando la image stitching API de OpenCV
	*/
	int stitch1(const Mat &left, const Mat &right, Mat &pano);	
	int stitchf(const Mat &left, const Mat &right, Mat &pano);	
	int stitchs1(const Mat &left, const Mat &right, Mat &pano);
	int stitchs2(const Mat &left, const Mat &right, Mat &pano);
	int stitchsg(const Mat &left, const Mat &right, Mat &pano);
	int stitchsgrs(const Mat &left, const Mat &right, Mat &pano);
	int stitchsp1(const Mat &left, const Mat &right, Mat &pano);
	int stitchsp2(const Mat &left, const Mat &right, Mat &pano);
	void Find_Transform_Matrixs(void);

	/**Simple distructor*/
	~Joiner();

			
};

#endif
