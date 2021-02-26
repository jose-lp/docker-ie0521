/** @file Joiner.h
 *  @brief Routines to join video files.
 *
 *  @details Routines to join video files. 
 *  @bug Por determinar
 */
 
#ifndef JGPU_HPP
#define JGPU_HPP

#include "Structs.h"
#include "VideoManager.h"
#include "YamlParser.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <opencv2/stitching/stitcher.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/gpu/gpumat.hpp>
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include "cv.h"
#include "Defs.h"

using namespace std;
using namespace cv;
using namespace cv::gpu;

class JGpu
{

private:
	int CheckGpu(void);

public:
	int gpu_enabled;

	/**Simple contructor*/
	JGpu();	
	void Set(void);
	void gpuwarpPerspective(const Mat& src_img, Mat& dst_img, const Mat& MTrans, Size dsize);
	void gpuwarpAffine(const Mat& src_img, Mat& dst_img, const Mat& MTrans, Size dsize);
	void gpucvtColor(const Mat& src_img, Mat& dst_img, int code);
	void gpuequalizeHist(const Mat& src_img, Mat& dst_img);


	/**Simple distructor*/
	~JGpu();
};

#endif
