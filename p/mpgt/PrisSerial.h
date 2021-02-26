/** @file prisserial.h
 *  @brief General functions of PRIS pattern recongnition.
 *
 *  @details This contains the definition of general functions for the
 *  spatial and temporal segmentation.
 *
 */
#ifndef TSEE_PRISSERIAL_HPP
#define TSEE_PRISSERIAL_HPP


#include <cmath>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <vector>
#include <algorithm>

#include "Defs.h"

#define PRIS_32FC1 1
#define PRIS_8UC1 0
#define PRIS_RGB2HSV 1
#define PRIS_GRY2RGB 0

const float EPS = 0.0001;

using namespace std;
using namespace cv;
/*!
 *  \addtogroup pris
 *  @{
 */
//namespace pris
//{
cv::Mat LoadImage( char* argv );
cv::Mat ConvertColor( cv::Mat input, bool colorspace );
cv::Mat Add( cv::Mat input1, cv::Mat input2 );
vector<cv::Mat> Split( cv::Mat input );
cv::Mat Histogram( cv::Mat input, int division_histo, const float* range_histo );
float MaxVal( cv::Mat input );
float MinVal( cv::Mat input );
cv::Mat Normalize( cv::Mat input, bool type);
void WaitKey( int miliseconds );
cv::Mat DrawHisto( cv::Mat input, int histo_length, int histo_height, int histo_bins, cv::Scalar line_color );
cv::Mat OtsuThreshold( cv::Mat input );
cv::Mat Threshold( cv::Mat input, int limit );
cv::Mat ImageAnd( cv::Mat input1, cv::Mat input2 );
cv::Mat MorphologyErode( cv::Mat input );
cv::Mat MorphologyDilate( cv::Mat input );
vector< vector< cv::Point > > Contours( cv::Mat input );
double ContourArea( vector< cv::Point > input );
double ContourPerimeter( vector< cv::Point > input );
cv::Mat DrawContour( vector< vector< cv::Point > > input, int rows, int cols );
vector< cv::Point > ConvexHull( vector< cv::Point > input );
vector< cv::Point > ConvexHull( cv::Mat input );
float ContainedInCountour( vector< cv::Point > input1, cv::Point input2 );
cv::Mat SpatialVariance( cv::Mat input, int ws );
cv::Mat LogVariance( cv::Mat input, int ws );
cv::Mat Hue_histogram(vector<cv::Mat> image_hsv);
cv::Mat Variance_histogram(cv::Mat variance);
vector<cv::Mat> integral_images(cv::Mat input);
double ComparePointsDistance(cv::Point a, cv::Point b);
double ComparePointsDepth(cv::Point a, cv::Point b);
double CompareAreas(double areaA, double areaB);
double CompareHistograms(cv::Mat hist1, cv::Mat hist2);
double CompareContours(vector< cv::Point > contour1,vector< cv::Point > contour2);
double DistanceHausdorff( const std::vector<cv::Point> & a, const std::vector<cv::Point> & b );
double Distance_2( const std::vector<cv::Point> & a, const std::vector<cv::Point>  & b );
double CompareDirections(double directionA, double directionB);
double CompareSpeeds(double speedA, double speedB);
std::string type2str(int type);
cv::Mat colorReduce(cv::Mat image, int div);
std::vector<double> GetProminentHueRangeColor(cv::Mat rgb_roi, int how_many);
std::vector<double> GetProminentHueRangeColor(cv::Mat rgb_roi, int how_many, int kclusters);
std::vector<double> GetProminentHueSaturationRangeColor(cv::Mat rgb_roi, int how_many);
cv::Point MatchBlobTemplate(Mat blob_mat, Point blob_mat_centroid, Mat blob_mat_template, int match_method,double *match_value);
void SimplestCB(Mat& in, Mat& out, float percent);
void imshowHSV(std::string& name, cv::Mat& image);
void show_histogram(std::string const& name, cv::Mat1b const& hist, int bins);
void show_nice_histogram(std::string const& name, cv::Mat& hist, int bins);
void show_2DHistogram(std::string const& name, cv::Mat const& hist2D, int hbins, int sbins);
cv::Mat ColorQuantizationKMeans(cv::Mat rgb_input, int nclusters);
//}

#endif
