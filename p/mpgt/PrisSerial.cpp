#include "PrisSerial.h"

//namespace pris
//{
//Returns the HSV Mat image form of the source image
cv::Mat LoadImage( char* argv )
{
	cv::Mat output;
	output = cv::imread( argv, CV_LOAD_IMAGE_COLOR );
	cv::normalize( output, output, 0, 1, cv::NORM_MINMAX, CV_32FC3, cv::Mat() );
	return output;
}

//Converts the input image to the selected color space
cv::Mat ConvertColor( cv::Mat input, bool colorspace )
{
	cv::Mat output;
	if( colorspace ) cv::cvtColor( input, output, CV_RGB2HSV );
	else {
		cv::cvtColor( input, output, CV_GRAY2RGB );
		cv::normalize( output, output, 0, 1, cv::NORM_MINMAX, CV_32FC3, cv::Mat() );
	}
	return output;
}

//Returns the sum of the input images
cv::Mat Add( cv::Mat input1, cv::Mat input2 )
{
	cv::Mat output;
	cv::add( input1, input2, output,cv::Mat(), CV_32F);
	return output;
}

//Returns a vector of one channel Mat images, each element been a channel of the input Mat image
vector<cv::Mat> Split( cv::Mat input )
{
	vector<cv::Mat> output;
	cv::split( input, output );
	return output;
}

//Returns the histogram  (one channel images only)
cv::Mat Histogram( cv::Mat input, int divisiones_histo, const float* range_histo )
{
	cv::Mat output;
	cv::calcHist( &input, 1, 0, cv::Mat(), output, 1, &divisiones_histo, &range_histo );
	return output;
}

//Returns the lowest value among the elements of a Mat image (one channel images only)
float MaxVal( cv::Mat input )
{
	double output;
	cv::minMaxLoc( input, NULL, &output, NULL, NULL, cv::Mat() );
	return (float) output;
}

//Returns the highest value among the elements of a Mat image (one channel images only)
float MinVal( cv::Mat input )
{
	double output;
	cv::minMaxLoc( input, &output, NULL, NULL, NULL, cv::Mat() );
	return (float) output;
}

//Returns the normalized outcome of the input Mat image (one channel images only)
//If type = PRIS_32FC1 the output is normalized to 32-bit floating point Mat image with values in the range [0,1]
//If type = PRIS_8UC1 the output is normalized to 8-bit unsigned char Mat image with values in the range [0,255]
cv::Mat Normalize( cv::Mat input, bool type)
{
	cv::Mat output;
	if( type ) cv::normalize( input, output, 0, 1, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );
	else cv::normalize( input, output, 0, 255, cv::NORM_MINMAX, CV_8UC1, cv::Mat() );
	return output;
}

//Introduces a delay of the given miliseconds
void WaitKey( int miliseconds )
{
	cv::waitKey( miliseconds );
	return;
}

//Returns a drawing of a Histogram in a 8-bit unsigned char Mat image (one channel images only)
cv::Mat DrawHisto( cv::Mat input, int histo_length, int histo_height, int histo_bins, cv::Scalar line_color )
{
	int bin_length = round( (double) histo_length/histo_bins );
	cv::normalize( input, input, 0, histo_height, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );
	cv::Mat output( histo_height, histo_length, CV_8UC1, cv::Scalar(255) );
	for( int i = 1; i < histo_bins; i++ ) {
		cv::line( output,
		          cv::Point( bin_length * (i-1), histo_height - round( input.at<float>(i-1) ) ),
		          cv::Point( bin_length * (i), histo_height - round( input.at<float>(i) ) ),
		          line_color );
	};
	return output;
}

//Returns the thresholded image of a 8-bit unsigned char Mat image with a given threshold (one channel images only)
cv::Mat Threshold( cv::Mat input, int limit )
{
	cv::Mat output;
	cv::threshold( input, output, limit, 255, cv::THRESH_BINARY );
	return output;
}

//Returns the Otsu thresholded image of a 8-bit unsigned char Mat image (one channel images only)
cv::Mat OtsuThreshold( cv::Mat input )
{
	cv::Mat output;
	cv::threshold( input, output, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU );
	return output;
}

//Returns the bit-wise AND of two Mat images of the same format (one channel binary images only)
cv::Mat ImageAnd( cv::Mat input1, cv::Mat input2 )
{
	cv::Mat output;
	cv::bitwise_and( input1, input2, output );
	return output;
}

//Returns a Mat image of the result of aplying the Erode Morphology operation to the input Mat image
// (one channel binary images only)
cv::Mat MorphologyErode( cv::Mat input )
{
	cv::Mat output;
	cv::erode( input, output, cv::Mat() );
	return output;
}

//Returns a Mat image of the result of aplying the Dilate Morphology operation to the input Mat image
// (one channel binary images only)
cv::Mat MorphologyDilate( cv::Mat input )
{
	cv::Mat output;
	cv::dilate( input, output, cv::Mat() );
	return output;
}

//Returns an array of arrays describing the outer most contours of the input image (one channel binary images only)
vector< vector< cv::Point > > Contours( cv::Mat input )
{
	vector< vector< cv::Point > >  output;
	//cv::findContours( input.clone(), output, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
	cv::findContours( input.clone(), output, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
	return output;
}

//Return the area of an input contour (in a array of points format)
double ContourArea( vector< cv::Point > input )
{
	return cv::contourArea( input );
}

//Return the perimeter of an input contour (in a array of points format)
double ContourPerimeter( vector< cv::Point > input )
{
	return cv::arcLength(input, true);
}



//Return the drawing of the input contours (in the array of arrays format)
cv::Mat DrawContour( vector< vector< cv::Point > > input, int rows, int cols )
{
	cv::Mat output( rows, cols, CV_8UC1, cv::Scalar(0) );
	cv::drawContours( output, input, -1, cv::Scalar(255) );
	return output;
}

//Returns the Convex Hull of an input contour
vector< cv::Point > ConvexHull( vector< cv::Point > input )
{
	vector< cv::Point > output;
	cv::convexHull( cv::Mat(input), output );
	return output;
}

//Returns the Convex Hull of an input binary Mat
vector< cv::Point > ConvexHull( cv::Mat input )
{
	vector< cv::Point > output;
	cv::convexHull( input, output );
	return output;
}

//Checks if input point is inside or outside of an input contour
//Return 1 if inside, 0 if on the edge and -1 if outside
float ContainedInCountour( vector< cv::Point > input1, cv::Point input2 )
{
	return cv::pointPolygonTest( input1, input2, false );
}

//Returns the logarithm of the Local Spatial Variance of an input image
//with a given kernel size, using the Integral Images method
cv::Mat LogVariance( cv::Mat input, int ws )
{
	int wl = ( ws - 1 ) / 2;
	int x_neg, x_pos, y_pos, y_neg;
	float N = (float) ( ws * ws );
	cv::Mat integ, integ_sq;
	cv::Mat sum( input.rows, input.cols, CV_64FC1, cv::Scalar(0) );
	cv::Mat sum_sq( input.rows, input.cols, CV_64FC1, cv::Scalar(0) );
	cv::Mat variance( input.rows, input.cols, CV_32FC1, cv::Scalar(0) );
	cv::Mat log_variance( input.rows, input.cols, CV_32FC1, cv::Scalar(0) );
	cv::integral( input, integ, integ_sq, CV_64F );
	for( int i=0; i < input.rows; i++) {
		for( int j=0; j < input.cols; j++) {
			if( i-wl < 0 ) x_neg = i + 1;
			else x_neg = wl + 1;
			if( i+wl >= input.rows ) x_pos = input.rows - i - 1;
			else x_pos = wl;
			if( j-wl < 0 ) y_neg = j + 1;
			else y_neg = wl + 1;
			if( j+wl >= input.cols ) y_pos = input.cols - j - 1;
			else y_pos = wl;
			sum.at<double>(i,j) = integ.at<double>( i+1+x_pos, j+1+y_pos )
			                      + integ.at<double>( i+1-x_neg, j+1-y_neg )
			                      - integ.at<double>( i+1+x_pos, j+1-y_neg )
			                      - integ.at<double>( i+1-x_neg, j+1+y_pos );
			sum_sq.at<double>(i,j) = integ_sq.at<double>( i+1+x_pos, j+1+y_pos )
			                         + integ_sq.at<double>( i+1-x_neg, j+1-y_neg )
			                         - integ_sq.at<double>( i+1+x_pos, j+1-y_neg )
			                         - integ_sq.at<double>( i+1-x_neg, j+1+y_pos );
			variance.at<float>(i,j) = (float) ( (1/N) * sum_sq.at<double>(i,j) - (1/(N*N)) * sum.at<double>(i,j) * sum.at<double>(i,j) );
			log_variance.at<float>(i,j) = log10( variance.at<float>(i,j) + EPS );
		}
	}
	return log_variance;
}



//Returns the Local Spatial Variance of an input image
//with a given kernel size, using the Integral Images method of opencv
cv::Mat SpatialVariance( cv::Mat input, int ws )
{
	int wl = ( ws - 1 ) / 2;
	int x_neg, x_pos, y_pos, y_neg;
	float N = (float) ( ws * ws );
	cv::Mat integ, integ_sq;
	cv::Mat sum( input.rows, input.cols, CV_64FC1, cv::Scalar(0) );
	cv::Mat sum_sq( input.rows, input.cols, CV_64FC1, cv::Scalar(0) );
	cv::Mat variance( input.rows, input.cols, CV_32FC1, cv::Scalar(0) );
	cv::Mat log_variance( input.rows, input.cols, CV_32FC1, cv::Scalar(0) );
	//cv::integral( input, integ, integ_sq, CV_64F );
	for( int i=0; i < input.rows; i++) {
		for( int j=0; j < input.cols; j++) {
			if( i-wl < 0 ) x_neg = i + 1;
			else x_neg = wl + 1;
			if( i+wl >= input.rows ) x_pos = input.rows - i - 1;
			else x_pos = wl;
			if( j-wl < 0 ) y_neg = j + 1;
			else y_neg = wl + 1;
			if( j+wl >= input.cols ) y_pos = input.cols - j - 1;
			else y_pos = wl;
			sum.at<double>(i,j) = integ.at<double>( i+1+x_pos, j+1+y_pos )
			                      + integ.at<double>( i+1-x_neg, j+1-y_neg )
			                      - integ.at<double>( i+1+x_pos, j+1-y_neg )
			                      - integ.at<double>( i+1-x_neg, j+1+y_pos );
			sum_sq.at<double>(i,j) = integ_sq.at<double>( i+1+x_pos, j+1+y_pos )
			                         + integ_sq.at<double>( i+1-x_neg, j+1-y_neg )
			                         - integ_sq.at<double>( i+1+x_pos, j+1-y_neg )
			                         - integ_sq.at<double>( i+1-x_neg, j+1+y_pos );
			variance.at<float>(i,j) = (float) ( (1/N) * sum_sq.at<double>(i,j) - (1/(N*N)) * sum.at<double>(i,j) * sum.at<double>(i,j) );
		}
	}
	return variance;
}
	
// Compute the Hue histogram
cv::Mat Hue_histogram(vector<cv::Mat> image_hsv){
	/// Establish the number of bins
	int histSize = 360;

	/// Set the ranges ( for H )
	float range[] = { 0, 360 } ;
	const float* histRange = { range };

	//bool uniform = true; bool accumulate = false;

	cv::Mat h_hist;

	/// Compute the histogram:
	calcHist( &image_hsv[0], 1, 0, cv::Mat(), h_hist, 1, &histSize, &histRange);//, uniform, accumulate );
	return h_hist;
}

	
cv::Mat Variance_histogram(cv::Mat variance){
	/// Establish the number of bins
	int histSize = 500;

	/// Set the ranges ( for H )
	float range[] = { 0, 1 } ;
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	cv::Mat v_hist;

	/// Compute the histogram:
	calcHist( &variance, 1, 0, cv::Mat(), v_hist, 1, &histSize, &histRange, uniform, accumulate );
	return v_hist;
}
	
	
// Compute the integral images from a frame
vector<cv::Mat> integral_images(cv::Mat input){
	cv::Mat suma, sqsuma, output;
	
	integral(output,suma,sqsuma);
	
	vector<cv::Mat> result;
	
	result.push_back(suma.clone());
	result.push_back(sqsuma.clone());
	
	return result;
}

//}

double ComparePointsDistance(cv::Point a, cv::Point b)
{
	double res = cv::norm(a-b);
	return res;
}

double ComparePointsDepth(cv::Point a, cv::Point b)
{
	double res = abs(a.y-b.y);
	return res;
}


double CompareAreas(double areaA, double areaB)
{
	double res = abs(areaA-areaB);
	return res;
}

double CompareDirections(double directionA, double directionB)
{
	D(cout << "Direction 1 = " << directionA << " Direction 2 = " << directionB << endl;)
	double res = abs(directionA-directionB);
	return res;
}

double CompareSpeeds(double speedA, double speedB)
{
	D(cout << "Speed 1 = " << speedA << " Speed 2 = " << speedB << endl;)
	double res = abs(speedA-speedB);
	return res;
}

double CompareHistograms(cv::Mat hist1, cv::Mat hist2)
{
/*
	string ty1 =  type2str( hist1.type() );
	string ty2 =  type2str( hist2.type() );
	cout << "Histogram 1 (type,col,row, channels, elements): " << ty1 << " " << hist1.cols << " " << hist1.rows << " " << hist1.channels() << endl;
	cout << "Histogram 2 (type,col,row, channels, elements): " << ty2 << " " << hist2.cols << " " << hist2.rows << " " << hist2.channels() << endl;
*/
	double res = 999999999.0;
	if(hist1.type()==hist2.type())
		res = compareHist(hist1,hist2, CV_COMP_BHATTACHARYYA);
	return res;
}

double CompareContours(vector< cv::Point  > contour1,vector< cv::Point >  contour2)
{
	double res = 999999999.0;
	if(contour1.size() > 0 && contour2.size() > 0 )
		res = matchShapes(contour1, contour2, CV_CONTOURS_MATCH_I3,0.0);
	return res;
}

double Distance_2( const std::vector<cv::Point> & a, const std::vector<cv::Point>  & b )
{
    double maxDistAB = 0;
    for (size_t i=0; i<a.size(); i++)
    {
        double minB = 1000000;
        for (size_t j=0; j<b.size(); j++)
        {
            double dx = (a[i].x - b[j].x);     
            double dy = (a[i].y - b[j].y);     
            double tmpDist = dx*dx + dy*dy;

            if (tmpDist < minB)
            {
                minB = tmpDist;
            }
            if ( tmpDist == 0 )
            {
                break; // can't get better than equal.
            }
        }
        maxDistAB += minB;
    }
    return maxDistAB;
}

double DistanceHausdorff( const std::vector<cv::Point> & a, const std::vector<cv::Point> & b )
{
    double maxDistAB = Distance_2( a, b );
    double maxDistBA = Distance_2( b, a );   
    double maxDist = std::max(maxDistAB,maxDistBA);

    return std::sqrt((double)maxDist);
}

std::string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

cv::Mat colorReduce(cv::Mat image, int div)
{    
    int nl = image.rows;                    // number of lines
    int nc = image.cols * image.channels(); // number of elements per line

    for (int j = 0; j < nl; j++)
    {
        // get the address of row j
        uchar* data = image.ptr<uchar>(j);

        for (int i = 0; i < nc; i++)
        {
            // process each pixel
            data[i] = data[i] / div * div + div / 2;
        }
    }
    
    return image;
}

std::vector<double> GetProminentHueRangeColor(cv::Mat rgb_roi, int how_many){

	//imshow("blob", rgb_roi); waitKey(0);

	cv::Mat hsv_roi = ConvertColor( rgb_roi, PRIS_RGB2HSV );
	
	vector<cv::Mat> canales = Split( hsv_roi );
	cv::Mat hue = canales[0]; 
	//imshow("hue", hue); waitKey(0);
	
	int hbins = 90; //bins
	/// Set the ranges ( for H )
	float range[] = { 0, 180};//El max en opencv es 180 en lugar de 360 rojo
	const float* histRange = { range };
	int channels[] = {0};

	cv::Mat h_hist;
	/// Compute the histogram:
	cv::calcHist( &hue, 1, channels, cv::Mat(), h_hist, 1, &hbins, &histRange, true, false);//calcHist( &image_hsv[0], 1, 0, cv::Mat(), h_hist, 1, &histSize, &histRange);//, uniform, accumulate );
	//normalize(h_hist, h_hist, 0, 255, CV_MINMAX);
	
	///OJO!!!!!!!!!!!!!!!!!!!!!:Por alguna razon que no se el bin 0 tiene un monton de pixeles, creo que son los negros
	//h_hist.data[0]=0;
	h_hist.at<float>(0)=0;

	//show_histogram("histogram", h_hist,hbins);
	//show_nice_histogram("histogram", h_hist,hbins);
	
	vector< pair <float,int> > hbinvec; //key,value
	
	//Change the data structure from histogram to a vector of pairs for
	//sorting the color ranges according to presence
	for(int bin = 0; bin < hbins; bin++){
		
		
		float color_count = hue.at<float>(bin);
		hbinvec.push_back( make_pair(color_count,bin) );
		// Using simple sort() function to sort descending 
		std::sort(hbinvec.begin(), hbinvec.end()); 
	}
	
	///Insert prominent colors at output vector at the beggining of the vector
	std::vector<double> prominent_range_color;
	
	if(how_many > hbins)
		how_many = hbins;
	
	for(int crb = how_many; crb > 0; crb--){//crb: color range bin
			
		prominent_range_color.push_back((double)hbinvec[crb].second); //Asi el color con mas incedencia esta en [0], el que siguen en [1]
	}
		
	return prominent_range_color;

}

std::vector<double> GetProminentHueRangeColor(cv::Mat rgb_roi, int how_many, int kclusters){

	//imshow("blob rgb", rgb_roi); waitKey(0);
	Mat rgbk_roi = ColorQuantizationKMeans(rgb_roi, kclusters);
	//imshow("blob rgbk", rgbk_roi); waitKey(0);

	cv::Mat hsv_roi = ConvertColor( rgbk_roi, PRIS_RGB2HSV );
	
	vector<cv::Mat> canales = Split( hsv_roi );
	cv::Mat hue = canales[0]; 
	//imshow("hue", hue); waitKey(0);
	
	int hbins = 90; //bins
	/// Set the ranges ( for H )
	float range[] = { 0, 180};//El max en opencv es 180 en lugar de 360 rojo
	const float* histRange = { range };
	int channels[] = {0};

	cv::Mat h_hist;
	/// Compute the histogram:
	cv::calcHist( &hue, 1, channels, cv::Mat(), h_hist, 1, &hbins, &histRange, true, false);//calcHist( &image_hsv[0], 1, 0, cv::Mat(), h_hist, 1, &histSize, &histRange);//, uniform, accumulate );
	//normalize(h_hist, h_hist, 0, 255, CV_MINMAX);
	
	///OJO!!!!!!!!!!!!!!!!!!!!!:Por alguna razon que no se el bin 0 tiene un monton de pixeles, creo que son los negros
	//h_hist.data[0]=0;
	h_hist.at<float>(0)=0;

	//show_nice_histogram("histogram", h_hist,hbins);
	
	vector< pair <float,int> > hbinvec; //key,value
	
	//Change the data structure from histogram to a vector of pairs for
	//sorting the color ranges according to presence
	for(int bin = 0; bin < hbins; bin++){
		
		
		float color_count = hue.at<float>(bin);
		hbinvec.push_back( make_pair(color_count,bin) );
		// Using simple sort() function to sort descending 
		std::sort(hbinvec.begin(), hbinvec.end()); 
	}
	
	///Insert prominent colors at output vector at the beggining of the vector
	std::vector<double> prominent_range_color;
	
	if(how_many > hbins)
		how_many = hbins;
	
	for(int crb = how_many; crb > 0; crb--){//crb: color range bin
			
		prominent_range_color.push_back((double)hbinvec[crb].second); //Asi el color con mas incedencia esta en [0], el que siguen en [1]
	}
		
	return prominent_range_color;

}


std::vector<double> GetProminentHueSaturationRangeColor(cv::Mat rgb_roi, int how_many){

	//imshow("blob", rgb_roi); waitKey(0);
	
	cv::Mat hsv_roi = ConvertColor( rgb_roi, PRIS_RGB2HSV );
	
	//imshow("hsv", hsv_roi); waitKey(0);

	int hbins = 90; int sbins = 64;
	int histSize[] = { hbins, sbins };

	float hrange[] = { 0, 180 };
	float srange[] = { 0, 256 };

	const float* ranges[] = { hrange, srange };

	//calcHist( &hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false );
	//normalize( hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat() );
	int channels[] = { 0, 1 };
	cv::MatND hs_hist;
	calcHist( &hsv_roi, 1, channels, Mat(), hs_hist, 2, histSize, ranges,true,false);
	///OJO!!!!!!!!!!!!!!!!!!!!!:Por alguna razon que no se el bin 0 tiene un monton de pixeles, creo que son los negros
	hs_hist.at<float>(0,0)=0;	
	//normalize( hs_hist, hs_hist, 0, 1, NORM_MINMAX, -1, Mat() );
	//show_2DHistogram("histogram",hs_hist,hbins,sbins);
	
	vector<std::array<double, 3> > histvec;
	
	//Change the data structure from histogram to a vector of pairs for
	//sorting the color ranges according to presence
	for(int hb = 0; hb < hbins; hb++)
		for( int sb = 0; sb < sbins; sb++ ){
		
		float color_count = hs_hist.at<float>(hb, sb);
		//cout << "Color count es " << color_count << " en hb " << hb << " en sb " << sb << endl;
		if(color_count != 0){
			array<double, 3> hvals { {(double)color_count, (double)hb, (double)sb} };
			histvec.push_back(hvals);
		}
	}

	// Using simple sort() function to sort descending 
	std::sort(histvec.begin(), histvec.end()); 

	
	///Insert prominent colors at output vector at the beggining of the vector
	std::vector<double> prominent_range_color;
	
	for(int crb = how_many; crb > 0; crb--){//crb: color range bin
		
		array<double, 3> hvals = histvec[crb];
		//cout << "Prominent color count " << hvals[0] << " en hb " << hvals[1] << " en sb " << hvals[2] << endl;
			
		prominent_range_color.push_back((double)hvals[1]); 
		prominent_range_color.push_back((double)hvals[2]); 

	}
		
	return prominent_range_color;

}

///https://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
cv::Point MatchBlobTemplate(Mat blob_mat, Point blob_mat_centroid, Mat blob_mat_template, int match_method, double *match_value)
{
/*	
	cout << "ROI image dimensions: " << blob_mat.cols << " x " << blob_mat.rows <<endl;
	cout << "Template image dimensions: " << blob_mat_template.cols << " x " << blob_mat_template.rows <<endl;
	
	imshow("ROI image", blob_mat); waitKey(0);
	imshow("Template", blob_mat_template); waitKey(0);
*/		
	
	Mat result;
	
	/// Create the result matrix
	int result_cols =  blob_mat.cols; //- blob_template.cols + 1;
	int result_rows = blob_mat.rows; //- blob_template.rows + 1;	
	
	result.create( result_rows, result_cols, CV_32FC1 );
	
	/// Do the Matching and Normalize
	matchTemplate( blob_mat, blob_mat_template, result, match_method );
	normalize( result, result, 0, 100, NORM_MINMAX, -1, Mat() );
	
	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	
	minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );	
	
	if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ){
		matchLoc = minLoc;
		*match_value = minVal;
	 }
	else{
		matchLoc = maxLoc; 
		*match_value = maxVal;
	}	
	
	Point real_matchLoc;
	
	matchLoc.x = matchLoc.x + blob_mat_centroid.x;
	matchLoc.y = matchLoc.y + blob_mat_centroid.y; 
	
	return matchLoc;
}

void SimplestCB(Mat& in, Mat& out, float percent) {
    assert(in.channels() == 3);
    assert(percent > 0 && percent < 100);

    float half_percent = percent / 200.0f;
    
    //Mat inrgb;
    //cv::cvtColor( in, inrgb, CV_HSV2RGB );

    vector<Mat> tmpsplit; split(in,tmpsplit);
    for(int i=0;i<3;i++) {
        //find the low and high precentile values (based on the input percentile)
        Mat flat; tmpsplit[i].reshape(1,1).copyTo(flat);
        cv::sort(flat,flat,CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
        int lowval = flat.at<uchar>(cvFloor(((float)flat.cols) * half_percent));
        int highval = flat.at<uchar>(cvCeil(((float)flat.cols) * (1.0 - half_percent)));
       // cout << lowval << " " << highval << endl;
        
        //saturate below the low percentile and above the high percentile
        tmpsplit[i].setTo(lowval,tmpsplit[i] < lowval);
        tmpsplit[i].setTo(highval,tmpsplit[i] > highval);
        
        //scale the channel
        normalize(tmpsplit[i],tmpsplit[i],0,255,NORM_MINMAX);
    }
    merge(tmpsplit,out);
}

void imshowHSV(std::string& name, cv::Mat& image)
{
  cv::Mat hsv;
  cv::cvtColor(image, hsv, CV_HSV2BGR);
  cv::imshow(name, hsv);
}

void show_histogram(std::string const& name, cv::Mat1b const& hist, int bins)
{
    // create matrix for histogram visualization
    int const hist_height = 256;

    double max_val=0;
    minMaxLoc(hist, 0, &max_val);

	cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);

    // visualize each bin
    for(int b = 0; b < bins; b++) {
        float const binVal = hist.at<float>(b);
        int   const height = cvRound(binVal*hist_height/max_val);
        cv::line
            ( hist_image
            , cv::Point(b, hist_height-height), cv::Point(b, hist_height)
            , cv::Scalar::all(255)
            );
    }
    cv::imshow(name, hist_image);waitKey(0);
}

void show_nice_histogram(std::string const& name, cv::Mat& hist, int bins)
{
	normalize(hist, hist, 0, 255, CV_MINMAX);

	//Mat for drawing
	Mat histimg = Mat::zeros(200, 320, CV_8UC3);
	histimg = Scalar::all(0);
	int binW = histimg.cols / bins;
	Mat buf(1, bins, CV_8UC3);
	//Set RGB color
	for (int i = 0; i < bins; i++)
		buf.at< Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / bins), 255, 255);
	cvtColor(buf, buf, CV_HSV2BGR);
	//drawing routine
	for (int i = 0; i < bins; i++)
	{
		int val = saturate_cast<int>(hist.at< float>(i)*histimg.rows / 255);

		rectangle(histimg, Point(i*binW, histimg.rows),
			Point((i + 1)*binW, histimg.rows - val),
			Scalar(buf.at< Vec3b>(i)), -1, 8);
		int r, g, b;
		b = buf.at< Vec3b>(i)[0];
		g = buf.at< Vec3b>(i)[1];
		r = buf.at< Vec3b>(i)[2];

		//show bin and RGB value
		printf("[%d] r=%d, g=%d, b=%d , bins = %d \n", i, r, g, b, val);
	}
	imshow(name, histimg); waitKey(0);
}

void show_2DHistogram(std::string const& name, cv::Mat const& hist2D, int hbins, int sbins)
{
	double maxVal=0;
	minMaxLoc(hist2D, 0, &maxVal, 0, 0);

	int scale = 10;
	Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8UC3);

	for( int h = 0; h < hbins; h++ )
		for( int s = 0; s < sbins; s++ ){
			float binVal = hist2D.at<float>(h, s);
			int intensity = cvRound(binVal*255/maxVal);
			rectangle( histImg, Point(h*scale, s*scale), Point( (h+1)*scale - 1, (s+1)*scale - 1), Scalar::all(intensity), CV_FILLED);
		}
  
	//namedWindow( "H-S Histogram", 1 );
	namedWindow( name, CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	imshow( name, histImg ); waitKey(0);
}

cv::Mat ColorQuantizationKMeans(cv::Mat rgb_input, int nclusters)
{
	Mat ocv = rgb_input;

	// convert to float & reshape to a [3 x W*H] Mat 
	//  (so every pixel is on a row of it's own)
	Mat data;
	ocv.convertTo(data,CV_32F);
	data = data.reshape(1,data.total());

	// do kmeans
	Mat labels, centers;
	kmeans(data, nclusters, labels, TermCriteria(CV_TERMCRIT_ITER, 10, 1.0), 3, 
		   KMEANS_PP_CENTERS, centers);

	// reshape both to a single row of Vec3f pixels:
	centers = centers.reshape(3,centers.rows);
	data = data.reshape(3,data.rows);

	// replace pixel values with their center value:
	Vec3f *p = data.ptr<Vec3f>();
	for (int i=0; i<data.rows; i++) {
		int center_id = labels.at<int>(i);
		p[i] = centers.at<Vec3f>(center_id);
	}

	// back to 2d, and uchar:
	ocv = data.reshape(3, ocv.rows);
	ocv.convertTo(ocv, CV_8U);
	
	return ocv;	
}
