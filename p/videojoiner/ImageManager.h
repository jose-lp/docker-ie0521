/** @file ImageManager.h
 *  @brief Routines to handle image files.
 *
 *  @details Routines that handle  image files. 
 *  @bug Por determinar
 */
#ifndef MPGT_IMANAGER_HPP
#define MPGT_IMANAGER_HPP

#include "Structs.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;
using namespace boost::filesystem;

/** @class ImageManager
 *  @brief Class for the manipulation of image data
 *  @details Class definition for the manipulation of image data using OpenCV.
 */
class ImageManager
{

private:

	//Data members
	string current_image; //**<  Path to the image*/
	string directory_path;
	int image_index;
	int start_image;
	int stop_image;

	//Methods
	int  LoadImage(string image_path);
	
public:
	
	//Data members
	Mat image; //**< Mat object of the image*/
	std::vector<string> fnames;
	int height;
	int width;
	
	//Methods

	/**Simple contructor*/
	ImageManager();	
	void LoadDirectory(string dir_path);
	Mat  LoadImage(void);
	Mat  LoadPreviousImage(void);
	Mat  GetImage(int nimage);
	void SaveImage(string save_path);
	void SaveImage(Mat simage, string save_path);
	void SaveImage(Mat simage, int id);
	void SaveImage(Mat simage, string save_dir, int id);
	void ShowImage(Mat vimage);
	void ShowImage(void);

	/**Simple distructor*/
	~ImageManager();

			
};

#endif
