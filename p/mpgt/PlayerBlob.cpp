#include "PlayerBlob.h"
#include "Defs.h"

using namespace cv;
using namespace std;


PlayerBlob::PlayerBlob()
{
	blob_frame_number = 0;
	perimeter = 0;
	area = 0;
	ratio = 0;
	blob_id = 0;
	player_tag = 0;
	blob_type = SINGLE_PLAYER;
	cluster=0;
	direction = 0;
	for(unsigned int phi = 0; phi<5;phi++){
		Point2f pt;
		pt.x = pt.y = 0.0;
		position_history[phi]=pt;
	}
	speed = 0;
}


PlayerBlob::PlayerBlob(int fnumber, int id)
{
	blob_frame_number = fnumber;
	blob_id = id;
	perimeter = 0;
	area = 0;
	ratio = 0;
	player_tag = id;
	blob_type = SINGLE_PLAYER;
	direction = 0;
	for(unsigned int phi = 0; phi<5;phi++){
		Point2f pt;
		pt.x = pt.y = 0.0;
		position_history[phi]=pt;
	}
	speed = 0;
}

void PlayerBlob::FillModel(int fnumber, vector< cv::Point > contorno_blob, Mat field_image)
{
	blob_frame_number = fnumber;	
	centroid = GetMassCenter(contorno_blob);
	contorno = contorno_blob;
	perimeter = ContourPerimeter(contorno_blob);
	area = ContourArea(contorno_blob);
	contourRegion = GetContourRegion(contorno_blob,field_image);
	if(contourRegion.rows>0)
		ratio = contourRegion.cols/contourRegion.rows;
	else
		ratio = 0;
	GetHistogram(contourRegion);
	blob_type = BlobMightBeAPlayer(contorno_blob);
	for(unsigned int phi = 0; phi<5;phi++)
		position_history[phi]=centroid;	
	speed = 0;

}

void PlayerBlob::FillModel(int fnumber, int id, vector< cv::Point > contorno_blob, Mat field_image)
{
	blob_frame_number = fnumber;
	blob_id = id;
	player_tag = id;
	//player_tag_set.insert(id);	
	centroid = GetMassCenter(contorno_blob);
	contorno = contorno_blob;
	perimeter = ContourPerimeter(contorno_blob);
	area = ContourArea(contorno_blob);
	contourRegion = GetContourRegion(contorno_blob,field_image);
	if(contourRegion.rows>0)
		ratio = contourRegion.cols/contourRegion.rows;
	else
		ratio = 0;
	GetHistogram(contourRegion);
	blob_type = BlobMightBeAPlayer(contorno_blob,contourRegion);
	for(unsigned int phi = 0; phi<5;phi++){
/*
		Point2f pt;
		pt.x =0.0; pt.y = 0.0;
		position_history[phi]=pt;
*/		
		position_history[phi]=centroid;
	}
	speed = 0;
}

Mat PlayerBlob::GetContourRegion(vector< cv::Point > contorno_blob, Mat field_image)
{

	// Get bounding box for contour 
    Rect roi = boundingRect(contorno_blob);

    // Create a mask to mask out that region from image.
    Mat mask = Mat::zeros(field_image.size(), CV_8UC1);

	std::vector<std::vector<cv::Point> > contourVec;
	contourVec.push_back(contorno_blob);
	drawContours(mask, contourVec, -1, Scalar(255), CV_FILLED);

    // At this point, mask has value of 255 for pixels within the contour and value of 0 for those not in contour.
    //imshow("Contorno",mask); waitKey(0);

    // Extract region using mask for region
    Mat contourROI;
    Mat imageROI;
    field_image.copyTo(imageROI, mask); 
    contourROI = imageROI(roi);
    //imshow("Contorno",contourRegion); waitKey(0);
    //imshow("Contorno",contourROI); waitKey(0);
    // Mat maskROI = mask(roi); // Save this if you want a mask for pixels within the contour in contourRegion. 
	return contourROI;
}

Point2f PlayerBlob::GetMassCenter(vector< cv::Point > contorno_blob)
{

	Moments mu = cv::moments(contorno_blob);

	///  Get the mass center:
	Point2f mc;
	mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 ); 	
	return mc;
}

void PlayerBlob::GetHistogram(Mat player_blob)
{
	cv::Mat hsv_player_blob = ConvertColor( player_blob, PRIS_RGB2HSV );
	int h_bins = 90; int s_bins = 64; //int v_bins = 10;
	int histSize[] = { h_bins, s_bins };

	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	//float v_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges };

	int channels[] = { 0, 1 };

	calcHist( &hsv_player_blob, 1, channels, Mat(), histogram, 2, histSize, ranges, true, false );
	///OJO!!!!!!!!!!!!!!!!!!!!!:Por alguna razon que no se el bin 0 tiene un monton de pixeles, creo que son los negros
	histogram.at<float>(0,0)=0;	
	normalize( histogram, histogram, 0, 1, NORM_MINMAX, -1, Mat() );

/*
	int h_bins = 50; 
	int channels[] = {0};
	int histSize[] = {h_bins};
	float hranges[] = { 0, 180 };
	const float* ranges[] = { hranges}; 
	
	calcHist( &player_blob, 1, channels, Mat(), histogram, 1, histSize, ranges, true, false );
	normalize( histogram, histogram, 0, 255, NORM_MINMAX);
	
	//Mat for drawing 
	Mat histimg = Mat::zeros(200, 320, CV_8UC3); 
	histimg = Scalar::all(0);
	int binW = histimg.cols / h_bins;
	Mat buf(1, h_bins, CV_8UC3);
 
	//Set RGB color
	for( int i = 0; i <  h_bins; i++ )
		buf.at< Vec3b>(i) = Vec3b(saturate_cast< uchar>(i*180./h_bins), 255, 255);
	
	cvtColor(buf, buf, CV_HSV2BGR);
 
	//drawing routine
	for( int i = 0; i <  h_bins; i++ )
	{
		int val = saturate_cast< int>(histogram.at< float>(i)*histimg.rows/255);
  
		rectangle( histimg, Point(i*binW,histimg.rows),
		Point((i+1)*binW,histimg.rows - val),
		Scalar(buf.at< Vec3b>(i)), -1, 8 );
		int r,g,b;
		b =  buf.at< Vec3b>(i)[0];
		g =  buf.at< Vec3b>(i)[1];
		r =  buf.at< Vec3b>(i)[2];

		//show bin and RGB value
		printf("[%d] r=%d, g=%d, b=%d , bins = %d \n",i , r, g, b, val);
	}
	
	imshow("Blob source for histogram", player_blob); //waitKey();
	imshow( "Histogram", histimg );waitKey();
*/
}

bool PlayerBlob::BlobMightBeAPlayer(vector< cv::Point > contorno_blob)
{
	double descriminar = 0.1; //Magic number
	double areac = ContourArea(contorno_blob);
	double perimetroc = ContourPerimeter(contorno_blob);
	double circularity = 4.0 * M_PI * areac/(perimetroc*perimetroc);
	if(circularity < descriminar)
		return NO;
	else
		return YES;
}

bool PlayerBlob::BlobMightBeAPlayer(vector< cv::Point > contorno_blob, cv::Mat contour_histogram)
{
	double descriminar = 0.01; //Magic number
	double areac = ContourArea(contorno_blob);
	double perimetroc = ContourPerimeter(contorno_blob);
	double circularity = 4.0 * M_PI * areac/(perimetroc*perimetroc);
	double max_value = 0.0;
	int maxIdx[2]= {255, 255}; //Esto es porque tiene que ser 2D: y,x
	
	//Find the predominant colour
	std::vector<cv::Mat> hschannels;
	split(contour_histogram, hschannels); //Canal 0 es hue
	cv::minMaxIdx(hschannels[0], NULL, &max_value, NULL, maxIdx);
	
	if((circularity < descriminar) || ((70 < maxIdx[1]) && (maxIdx[1] < 170))){ //If predominant colour is green
		//std::cout << "Blob id " << blob_id << " max value = " << max_value << " at Idx " << maxIdx[1] << endl;
		return NON_PLAYER;
	}
	else
		return SINGLE_PLAYER;
}

bool PlayerBlob::BlobMightBeALine(Mat player_blob)
{
	
	double descriminar = 0.1; //Magic number
	double areac = ContourArea(contorno);
	double perimetroc = ContourPerimeter(contorno);
	double circularity = 4.0 * M_PI * areac/(perimetroc*perimetroc);
	
	//GetHistogram(player_blob);
	double maxVal = 0.0;
    minMaxLoc(histogram, 0, &maxVal, 0, 0);
    //Falta hacer algo con maxValue  https://stackoverflow.com/questions/20567643/getting-dominant-colour-value-from-hsv-histogram

	if(circularity > descriminar)
		return YES;
	else
		return NO;
	
	return 0;
}

void PlayerBlob::ShowBlobInfo(void)
{
		if((area != 0.0)&(perimeter != 0)){
			cout << "Blob info:" << endl;
			cout << "Frame number = " << blob_frame_number << endl;
			cout << "Id = " << blob_id << endl;
			cout << "Area = " << area << endl;
			cout << "Perimeter = " << perimeter << endl;
		}
		else{
				cout << "Blob info not yet ready" << endl;
		}
}

void PlayerBlob::AssignInitTag(vector< TrackPoint > init_tpoints, int init_tkp_threshold)
{
	vector<TrackPoint>::iterator myTrkPtVectorIterator;
	
	double min_dist = 99999999999.9;
	unsigned int itindex=0;
	
	for(myTrkPtVectorIterator = init_tpoints.begin(); 
		myTrkPtVectorIterator != init_tpoints.end();
		myTrkPtVectorIterator++){
	
		if(myTrkPtVectorIterator->frame_number == blob_frame_number){
			///Obtain euclidian distance
			double dist;
			double x = (double) (myTrkPtVectorIterator->point.x - centroid.x);
			double y = (double) (myTrkPtVectorIterator->point.y - centroid.y);				
		
			dist = pow(x, 2) + pow(y, 2);       
			dist = sqrt(dist);
			
			if(dist <= min_dist){
				min_dist = dist;
				itindex = distance(init_tpoints.begin(), myTrkPtVectorIterator);
			}
		}
	}
	
	///Assign closest label
	if((init_tpoints[itindex].frame_number == blob_frame_number) && (min_dist <= init_tkp_threshold) ){
		player_tag = init_tpoints[itindex].label;
		if(player_tag_set.size()==1){
			std::set<int>::iterator it;
			it = player_tag_set.begin();
			 player_tag_set.erase (it);
		}
		merged_centroids.clear();
		merged_centroids.insert(std::make_pair(init_tpoints[itindex].label, centroid));
		player_tag_set.insert(init_tpoints[itindex].label);
	}
}

void PlayerBlob::SetBlobType(int type){
	blob_type = type;
	//cout << "Setting blob to type: " << type << endl;
}

void PlayerBlob::SetDirection(Point2f start_point,Point2f end_point)
{
	
	//double dx = end_point.x - start_point.x;
	//double dy = end_point.y - end_point.y;
	//double angleInRadians = std::atan2(y, x);
	
	double dot = start_point.x*end_point.x + end_point.y*end_point.y;		/// dot product between [x1, y1] and [x2, y2]
	double det = start_point.x*end_point.y - end_point.y*end_point.x;		/// determinant
	direction = std::atan2(det, dot);	/// atan2(y, x) or atan2(sin, cos)
/*	
	cout << "Componentes start point: x = " << start_point.x << " y = " << start_point.y << endl;
	cout << "Componentes end point: x = " << end_point.x << " y = " << end_point.y << endl;
	cout << "Producto punto = " << dot << " Determinante = " << det << endl;
*/
}

void PlayerBlob::UpdateDirection(Point2f new_position)
{
	UpdatePositionHistory(new_position);
	SetDirection(position_history[0],position_history[4]);
}

void PlayerBlob::UpdateDirection(void)
{
	UpdatePositionHistory();
	SetDirection(position_history[0],position_history[4]);
}

void PlayerBlob::UpdatePositionHistory(Point2f new_position)
{
	//cout << "UpdatePositionHistory:" << endl;
	for(unsigned int ihp = 0; ihp<4; ihp++){
		position_history[ihp]=position_history[ihp+1];
		D(cout << "Position " << ihp << " has point: x = " << position_history[ihp].x << " y = " << position_history[ihp].y << endl; )
	}
	position_history[4]=new_position;
	//cout << "Position " << 4 << " has point: x = " << position_history[4].x << " y = " << position_history[4].y << endl;
}

void PlayerBlob::UpdatePositionHistory(void)
{
	//cout << "UpdatePositionHistory:" << endl;
	for(unsigned int ihp = 0; ihp<4; ihp++){
		position_history[ihp]=position_history[ihp+1];
		D(cout << "Position " << ihp << " has point: x = " << position_history[ihp].x << " y = " << position_history[ihp].y << endl; )
	}
	position_history[4]=centroid;
	//cout << "Position " << 4 << " has point: x = " << position_history[4].x << " y = " << position_history[4].y << endl;
	
}

void PlayerBlob::ChangeBlobStartPointHistory(Point2f new_start_point)
{
	position_history[0]=new_start_point;
}

void PlayerBlob::UpdateSpeed(void)
{
	double distance = ComparePointsDistance(position_history[0],position_history[4]);
	double time=(1.0/30.0)*5.0; ///5 frames a un 30FPS
	
	speed=distance/time;
}

void PlayerBlob::UpdateSpeed(int frames)
{
	double distance = ComparePointsDistance(position_history[0],position_history[4]);
	double time=(1.0/30.0)*(double)frames; ///5 frames a un 30FPS
	
	speed=distance/time;
}

PlayerBlob::~PlayerBlob()
{
	histogram.release();
	contorno.clear(); //change for shrink to fit
	contourRegion.release();
}
