#include "Joiner.h"

using namespace std;
using namespace cv;

Joiner::Joiner()
{
	dropFrame = true;
	framerate = 29.97;
	frame2skip = 30;
		
}

Joiner::Joiner(std::map<std::string,int> start_config)
{
	dropFrame = true;
	framerate = 29.97;
	frame2skip = 30;
	config = start_config;
		
}

Joiner::Joiner(std::map<std::string,int> start_config, MediaInfo vidinfo)
{
	bufVideoInfo = vidinfo;
	dropFrame = true;
	framerate = bufVideoInfo.fps;
	config = start_config;
	frame2skip = config["frame2skip"];
	//cout << "Frame 2 skip = " << config["frame2skip"] << endl;
		
}

Joiner::Joiner(bool df, float fr)
{
	dropFrame = df;
	framerate = fr;
		
}

void Joiner::SetJoiner(bool df, float fr)
{
	dropFrame = df;
	framerate = fr;
	cout << "Frame rate set to: " << framerate << endl;
		
}

//http://andrewduncan.net/timecodes/
int Joiner::frameNumber(std::string timecode) {	
	int fps = round(framerate);
	int dropFrames = round( framerate / 15 );
	int hourFrames = fps * 60 * 60;
	int minuteFrames = fps * 60;
	int hours = stoi( timecode.substr(0, 2) );
	int minutes = stoi( timecode.substr(3, 2) );
	int seconds = stoi( timecode.substr(6, 2) );
	int frames = stoi( timecode.substr(9, 2) );
	int totalMinutes = 60 * hours + minutes;
	int frameNum = hourFrames * hours + minuteFrames * minutes + fps * seconds + frames;
	if (!dropFrame) return frameNum;
	else return frameNum - dropFrames * (totalMinutes - totalMinutes / 10);
}

void Joiner::synch(VideoCapture &lvid, std::string ltc, VideoCapture &rvid, std::string rtc) {
	int tcl = frameNumber(ltc);
	int tcr = frameNumber(rtc);
	int tc = max(tcl, tcr);
	int lskip = tc - tcl;
	int rskip = tc - tcr;
	cout << "Left  frame offset: " << lskip << endl;
	cout << "Right frame offset: " << rskip << endl;

	ofstream cpar("out/cluster.tc");
	cpar << lskip << "\t" << rskip << endl;
	cpar.close();

	config["left_video_start_frame"] = tcl;
	config["right_video_start_frame"] = tcr;	

	while (lskip--) lvid.grab();
	while (rskip--) rvid.grab();
	
}

void Joiner::synch(VideoManager *lvid, VideoManager *rvid, YamlParser *pyp) {
	std::string ltc = pyp->joiner_sparms["left_timecode"];
	std::string rtc = pyp->joiner_sparms["right_timecode"];
	
	int tcl = frameNumber(ltc);
	int tcr = frameNumber(rtc);
	int tc = max(tcl, tcr);
	int lskip = tc - tcl;
	int rskip = tc - tcr;
	cout << "Left  frame offset: " << lskip << endl;
	cout << "Right frame offset: " << rskip << endl;

	config["left_video_start_frame"] = rskip;
	config["right_video_start_frame"] = lskip;	
	
	pyp->joiner_parms["left_video_start_frame"] = rskip;
	pyp->joiner_parms["right_video_start_frame"] = lskip;
	
	lvid->SetFrameNumber(lskip);
	rvid->SetFrameNumber(rskip);
	
}

void Joiner::synch_manual(VideoCapture &lvid, int left_start_frame, VideoCapture &rvid, int right_start_frame) {
	int tcl = left_start_frame;
	int tcr = right_start_frame;
	int tc = max(tcl, tcr);
	int lskip = tc - tcl;
	int rskip = tc - tcr;
	cout << "Left  frame offset: " << lskip << endl;
	cout << "Right frame offset: " << rskip << endl;
/*
	ofstream cpar("out/cluster.tc");
	cpar << lskip << "\t" << rskip << endl;
	cpar.close();
*/
	while (lskip--) lvid.grab();
	while (rskip--) rvid.grab();
}


void Joiner::synch_graph(VideoCapture &lvid, VideoCapture &rvid) {


	msgBox("Sincronizacion grafica ( Enter:sale  <:izq+1  >:der+1  ^:izq+20  v:der+20 )");
	cvWindow("left", 0, 0, 640, 360);
	cvWindow("right", 641, 0, 640, 360);

	int lf = 0, rf = 0;
	Mat lframe, rframe;
	lvid >> lframe;
	rvid >> rframe;
	
	while (true) {
		if (lframe.empty() || rframe.empty()) break;	// fin de video
		imshow("left", lframe);
		imshow("right", rframe);
		int key = waitKey();
		//cout << "key: " << key;
		if (key == config["left_key"]) {
			lvid >> lframe;
			++lf; cout << "Left frame number: " << lf << endl;
		}
		else if (key == config["right_key"]) {
			rvid >> rframe;
			++rf; cout << "Right frame number: " << rf << endl;
		}
		else if (key == config["up_key"]) {
			for (int i = 0; i < config["frames2skip"]; i++)
				lvid.grab();
			lf += config["frames2skip"]; cout << "Left frame number: " << lf << endl;
			lvid.retrieve(lframe);
		}
		else if (key == config["down_key"]) {
			for (int i = 0; i < config["frames2skip"]; i++)
				rvid.grab();
			rf += config["frames2skip"]; cout << "Right frame number: " << rf << endl;
			rvid.retrieve(rframe);
		}
		else if (key == config["enter_key"]) break;
		cout << "left: " << lf;
		cout << "\t\tright: " << rf << "\r" << flush;
	}
	destroyAllWindows();
	cout << endl;

	ofstream cpar("out/cluster.tc", ios_base::app);
	cpar << lf << "\t" << rf << endl;
	cpar.close();
	config["left_video_start_frame"] = lf;
	config["right_video_start_frame"] = rf;

}

void Joiner::synch_graph2(VideoCapture &lvid, VideoCapture &rvid, int start_frame) {


	msgBox("Sincronizacion grafica ( Enter:sale  <:izq+1  >:der+1  ^:izq+config_skip  v:der+config_skip )");
	cout << "Sincronizacion grafica ( Enter:sale  <:izq+1  >:der+1  ^:izq+config_skip  v:der+config_skip )" << endl;

	lvid.set(CV_CAP_PROP_POS_FRAMES, start_frame);
	rvid.set(CV_CAP_PROP_POS_FRAMES, start_frame);
	
	int lf = start_frame, rf = start_frame;
	Mat lframe, rframe;
	lvid >> lframe;
	rvid >> rframe;

/*
	Mat combine(max(lframe.size().height, rframe.size().height), lframe.size().width + rframe.size().width, CV_8UC3);
	Mat left_roi(combine, Rect(0, 0, lframe.size().width, lframe.size().height));
	Mat right_roi(combine, Rect(rframe.size().width, 0, rframe.size().width, rframe.size().height));
	lframe.copyTo(left_roi);
	rframe.copyTo(right_roi);
*/	
	namedWindow("Combined", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	
	
	int run = true;
	while (run) {
		if (lframe.empty() || rframe.empty()){
			run = false; 
			break;	// fin de video
		}

		Mat combine(max(lframe.size().height, rframe.size().height), lframe.size().width + rframe.size().width, CV_8UC3);
		Mat left_roi(combine, Rect(0, 0, lframe.size().width, lframe.size().height));
		Mat right_roi(combine, Rect(rframe.size().width, 0, rframe.size().width, rframe.size().height));
		lframe.copyTo(left_roi);
		rframe.copyTo(right_roi);
	
		cout << "New combined frame" << endl;
		imshow("Combined",combine);//waitKey(0);

		int key = waitKey(0);
	
		cout << "key: " << key << endl;
		if (key == config["left_key"]) {
			lvid >> lframe;
			//lframe.copyTo(left_roi);
			++lf; 
			cout << "Left frame number: " << lf << endl;
			//cout << "Left frame on video: " << lvid.get(CV_CAP_PROP_POS_FRAMES) << endl;
		}
		else if (key == config["right_key"]) {
			rvid >> rframe;
			//rframe.copyTo(right_roi);
			++rf; 
			cout << "Right frame number: " << rf << endl;
			//cout << "Right frame on video: " << rvid.get(CV_CAP_PROP_POS_FRAMES) << endl;
		}
		else if (key == config["up_key"]) {
			//for (int i = 0; i < config["frames2skip"]; i++)
			//	lvid.grab();
				
			lf += frame2skip; 
			cout << "Skip left with " << frame2skip << " frames." << endl;
			cout << "Skip - Left frame number: " << lf << endl;
			lvid.set(CV_CAP_PROP_POS_FRAMES, lf);
			lvid.retrieve(lframe);
			//lframe.copyTo(left_roi);
			cout << "Left frame number: " << lf << endl;
		}
		else if (key == config["down_key"]) {
			//for (int i = 0; i < config["frames2skip"]; i++)
				//rvid.grab();
			rf += frame2skip; 
			cout << "Skip right with " << frame2skip << " frames." << endl;
			cout << "Skip - Right frame number: " << rf << endl;
			rvid.set(CV_CAP_PROP_POS_FRAMES, rf);
			rvid.retrieve(rframe);
			//rframe.copyTo(right_roi);
			cout << "Right frame number: " << rf << endl;
		}
		else if (key == config["enter_key"]) 
			break;

		else
			cout << "Key pressed: " << key << endl;

		cout << "left: " << lf << "\t\tright: " << rf << "\r" << endl;
	}
	destroyAllWindows();
	cout << endl;
/*
	ofstream cpar("out/cluster.tc", ios_base::app);
	cpar << lf << "\t" << rf << endl;
	cpar.close();
*/
	config["left_video_start_frame"] = lf;
	config["right_video_start_frame"] = rf;

}

int Joiner::color_match(Mat *ref_frame, Mat *tgt_frame) {
	if (DEBUG_PANO) {
		cvWindow("Reference");
		imshow("Reference", *ref_frame);
		waitKey(1000);
		vector<Mat> channels;
		split(*ref_frame, channels);

		Mat blank = Mat::zeros(ref_frame->rows, ref_frame->cols, CV_8UC1);
		vector<Mat> Rc, Gc, Bc;
		Rc.push_back(blank);
		Rc.push_back(blank);
		Rc.push_back(channels[2]);
		Gc.push_back(blank);
		Gc.push_back(channels[1]);
		Gc.push_back(blank);
		Bc.push_back(channels[0]);
		Bc.push_back(blank);
		Bc.push_back(blank);
		Mat R, G, B;
		merge(Rc, R);
		merge(Gc, G);
		merge(Bc, B);

		cvWindow("R");
		imshow("R", R);
		waitKey(1000);
		cvWindow("G");
		imshow("G", G);
		waitKey(1000);
		cvWindow("B");
		imshow("B", B);
		waitKey(1000);
		destroyAllWindows();
		imwrite("dbg/R.jpg", R);
		imwrite("dbg/G.jpg", G);
		imwrite("dbg/B.jpg", B);
	}


	//cvWindow("Reference");
	//imshow("Reference", *ref_frame);
/*
	/// Select ROI
	bool showCrosshair = false;
	bool fromCenter = false;
	msgBox("Seleccione la region de interes");
	Rect r = selectROI("Reference", *ref_frame, fromCenter, showCrosshair);

	// Crop image
	Mat rCrop = (*ref_frame)(r);
	if (DEBUG_PANO) {
		imshow("Crop", rCrop);
		waitKey(1000);
		imwrite("dbg/rorig.jpg", *ref_frame);
		imwrite("dbg/rroi.jpg", rCrop);
	}
	destroyAllWindows();

	cvWindow("Target");
	imshow("Target", *tgt_frame);

	// Select ROI
	msgBox("Seleccione la region de interes");
	r = selectROI("Target", *tgt_frame, fromCenter, showCrosshair);

	// Crop image
	Mat tCrop = (*tgt_frame)(r);
	if (DEBUG_PANO) {
		imshow("Crop", tCrop);
		waitKey(1000);
		imwrite("dbg/torig.jpg", *tgt_frame);
		imwrite("dbg/troi.jpg", tCrop);
	}
	destroyAllWindows();

	// Corrige los colores de la imagen destino
	histMatch(rCrop, tCrop, *tgt_frame);

	cvWindow("Result");
	imshow("Result", *tgt_frame);
	waitKey();
	destroyAllWindows();
	if (DEBUG_PANO)
		imwrite("dbg/thm.jpg", *tgt_frame);

*/
	return 0;
	
}

void Joiner::histMatch(const Mat &reference, const Mat &target, Mat &result) {
/*	
	const double HISTMATCH = 0.000001;
	double min, max;

	vector<Mat> ref_channels;
	split(reference, ref_channels);
	vector<Mat> tgt_channels;
	split(target, tgt_channels);
	vector<Mat> res_channels;
	split(result, res_channels);

	int histSize = 256;
	float range[] = {0, 256};
	const float* histRange = { range };
	bool uniform = true;
	ofstream cpar("out/cluster.lut");

	// Para cada canal (B, G, R)
	for ( int i=0 ; i<3 ; i++ ) {
		Mat ref_hist, tgt_hist;
		Mat ref_hist_accum, tgt_hist_accum;

		// Calcula histogramas de frecuencia
		calcHist(&ref_channels[i], 1, 0, Mat(), ref_hist, 1, &histSize,
		&histRange, uniform, false);
		calcHist(&tgt_channels[i], 1, 0, Mat(), tgt_hist, 1, &histSize,
		&histRange, uniform, false);

		// Normaliza: calcula densidad de probabilidad
		minMaxLoc(ref_hist, &min, &max);
		if (max==0) continue;
		ref_hist = ref_hist / max;
		minMaxLoc(tgt_hist, &min, &max);
		if (max==0) continue;
		tgt_hist = tgt_hist / max;

		int histSize = 256;
		int hist_w = 512, hist_h = 400;
		int bin_w = cvRound( (double) hist_w/histSize );
		Mat histImage1(hist_h, hist_w, CV_8UC3, Scalar::all(255));
		Mat histImage2(hist_h, hist_w, CV_8UC3, Scalar::all(255));

		if (DEBUG_PANO and i==0) {
			//normalize(lhist, lhist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
			//normalize(rhist, rhist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
			for( int i = 1; i < histSize; i++ ) {
				line( histImage1, Point( bin_w*(i-1), cvRound(hist_h * (1-ref_hist.at<float>(i-1)) )),
					  Point( bin_w*(i), hist_h - cvRound(ref_hist.at<float>(i)) ),
					  Scalar( 255, 0, 0), 2, 8, 0  );
				line( histImage2, Point( bin_w*(i-1), cvRound(hist_h * (1-tgt_hist.at<float>(i-1))) ),
					  Point( bin_w*(i), hist_h - cvRound(tgt_hist.at<float>(i)) ),
					  Scalar( 0, 0, 255), 2, 8, 0  );
			}

			cvWindow("Ref hist", 0, 0, 512, 400);
			cvWindow("Tgt hist", 513, 0, 512, 400);
			imshow("Ref hist", histImage1 );
			imshow("Tgt hist", histImage2 );
			imwrite("dbg/rhist.jpg", histImage1);
			imwrite("dbg/thist.jpg", histImage2);
			waitKey();
			destroyAllWindows();
		}

		// Calcula distribución acumulada
		ref_hist.copyTo(ref_hist_accum);
		tgt_hist.copyTo(tgt_hist_accum);

		float *src_cdf_data = ref_hist_accum.ptr<float>();
		float *dst_cdf_data = tgt_hist_accum.ptr<float>();

		for ( int j=1 ; j < 256 ; j++ ) {
			src_cdf_data[j] = src_cdf_data[j] + src_cdf_data[j-1];
			dst_cdf_data[j] = dst_cdf_data[j] + dst_cdf_data[j-1];
		}

		// Normaliza cdf
		minMaxLoc(ref_hist_accum, &min, &max);
		ref_hist_accum = ref_hist_accum / max;
		minMaxLoc(tgt_hist_accum, &min, &max);
		tgt_hist_accum = tgt_hist_accum / max;

		if (DEBUG_PANO and i==0) {
			histImage1.setTo(255);
			histImage2.setTo(255);
			for( int i = 1; i < histSize; i++ ) {
				line( histImage1, Point( bin_w*(i-1), cvRound(hist_h * (1-ref_hist_accum.at<float>(i-1)) )),
					  Point( bin_w*(i), hist_h - cvRound(ref_hist_accum.at<float>(i)) ),
					  Scalar( 255, 0, 0), 1, 8, 0  );
				line( histImage2, Point( bin_w*(i-1), cvRound(hist_h * (1-tgt_hist_accum.at<float>(i-1))) ),
					  Point( bin_w*(i), hist_h - cvRound(tgt_hist_accum.at<float>(i)) ),
					  Scalar( 0, 0, 255), 1, 8, 0  );
			}

			cvWindow("Ref cdf", 0, 0, 512, 400);
			cvWindow("Tgt cdf", 513, 0, 512, 400);
			imshow("Ref cdf", histImage1 );
			imshow("Tgt cdf", histImage2 );
			imwrite("dbg/rcdf.jpg", histImage1);
			imwrite("dbg/tcdf.jpg", histImage2);
			waitKey();
			destroyAllWindows();
		}

		// Calcula función de mapeo
		Mat Mv(1, 256, CV_8UC1);
		uchar *M = Mv.ptr<uchar>();
		uchar last = 0;
		for ( int j=0 ; j < tgt_hist_accum.rows ; j++ ) {
			float F1 = dst_cdf_data[j];

			for ( uchar k=last ; k < ref_hist_accum.rows ; k++ ) {
				float F2 = src_cdf_data[k];
				if ( std::abs(F2 - F1) < HISTMATCH || F2 > F1 ) {
					M[j] = k;
					last = k;
					break;
				}
			}
		}
		Mat lut(1, 256, CV_8UC1, M);
		for ( int j=0 ; j < 256 ; j++ )
			cpar << int(lut.data[j]) << endl;

		// Aplica función de mapeo
		LUT(res_channels[i], lut, res_channels[i]);

		if (DEBUG_PANO and i==0) {
			Mat rrr;
			calcHist(&res_channels[i], 1, 0, Mat(), rrr, 1, &histSize,
			&histRange, uniform, false);
			float *res_cdf_data = rrr.ptr<float>();
			for ( int j=1 ; j < 256 ; j++ ) {
				res_cdf_data[j] = res_cdf_data[j] + res_cdf_data[j-1];
			}
			minMaxLoc(rrr, &min, &max);
			rrr = rrr / max;

			//histImage2.setTo(255);
			for( int i = 1; i < histSize; i++ ) {
				line( histImage2, Point( bin_w*(i-1), cvRound(hist_h * (1-rrr.at<float>(i-1)) )),
					  Point( bin_w*(i), cvRound(hist_h * (1-rrr.at<float>(i)) )),
					  Scalar( 255, 0, 0), 1, 8, 0  );
			}
			cvWindow("Tgt corrected", 513, 0, 512, 400);
			imshow("Tgt corrected", histImage2 );
			imwrite("dbg/corr.jpg", histImage2);
			waitKey();
			destroyAllWindows();
		}
	}

	// Junta los 3 canales
	merge(res_channels, result);
	cpar.close();
*/	
}

int Joiner::stitchf(const Mat &left, const Mat &right, Mat &pano)
{
	vector<Mat> imgs;
	imgs.push_back(left);
	imgs.push_back(right);
	

	Stitcher stitcher = Stitcher::createDefault(config["try_use_gpu"]);
	Stitcher::Status status = stitcher.stitch(imgs, pano);

	if (status != Stitcher::OK) {
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return EXIT_FAILURE;
	}
	
	//namedWindow( "Display video", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	//imshow("Display video", pano); waitKey(0);
/*	
		// To remove the black portion after stitching, and confine in a rectangular region//

		// vector with all non-black point positions
		std::vector<cv::Point> nonBlackList;
		nonBlackList.reserve(pano.rows*pano.cols);

		// add all non-black points to the vector
		// there are more efficient ways to iterate through the image
		for(int j=0; j<pano.rows; ++j)
				for(int i=0; i<pano.cols; ++i)
				{
						// if not black: add to the list
						if(pano.at<cv::Vec3b>(j,i) != cv::Vec3b(0,0,0))
						{
								nonBlackList.push_back(cv::Point(i,j));
						}
				}

		// create bounding rect around those points
		cv::Rect bb = cv::boundingRect(nonBlackList);

		// display result
		namedWindow( "Result", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
		cv::imshow("Result", pano(bb));
		waitKey(0);	
		destroyAllWindows();	
*/
	return EXIT_SUCCESS;
	
}

int Joiner::stitchs1(const Mat &left, const Mat &right, Mat &pano)
{

	Mat gray_limage;
	Mat gray_rimage;

	//Covert to Grayscale
	cvtColor( left, gray_limage, CV_RGB2GRAY );
	cvtColor( right, gray_rimage, CV_RGB2GRAY );
/*
	namedWindow( "Images", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	imshow( "Images", left );waitKey(0);
	imshow( "Images", right );waitKey(0);
	destroyAllWindows();
*/
	if ( !gray_limage.data || !gray_rimage.data )
	{
			std::cout << " --(!) Error reading images " << std::endl;
			return -1;
	}


		//--Step 1 : Detect the keypoints using SURF Detector

		int minHessian = 400;

		SurfFeatureDetector detector( minHessian );

		std::vector< KeyPoint > keypoints_object, keypoints_scene;

		detector.detect( gray_limage, keypoints_object );
		detector.detect( gray_rimage, keypoints_scene );

		//--Step 2 : Calculate Descriptors (feature vectors)
		SurfDescriptorExtractor extractor;

		Mat descriptors_object,descriptors_scene;

		extractor.compute( gray_limage, keypoints_object, descriptors_object );
		extractor.compute( gray_rimage, keypoints_scene, descriptors_scene );

		//--Step 3 : Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match( descriptors_object, descriptors_scene, matches );

		double max_dist = 0;
		double min_dist = 100;

		//--Quick calculation of min-max distances between keypoints
		for(int i =0; i < descriptors_object.rows ; i++)
		{
				double dist = matches[i].distance;
				if( dist < min_dist ) min_dist = dist;
				if( dist > max_dist ) max_dist = dist;
		}

		//printf("-- Max dist : %f \n", max_dist );
		//printf("-- Min dist : %f \n", min_dist );

		//--Use only "good" matches (i.e. whose distance is less than 3 X min_dist )
		std::vector< DMatch > good_matches;

		for(int i =0 ; i < descriptors_object.rows ; i++)
		{
				if( matches[i].distance < 3*min_dist )
				{
						good_matches.push_back( matches[i] );
				}
		}
		std::vector< Point2f > obj;
		std::vector< Point2f > scene;

		for( unsigned int i = 0; i < good_matches.size(); i++)
		{
				//--Get the keypoints from the good matches
				obj.push_back( keypoints_object[good_matches[i].queryIdx].pt );
				scene.push_back( keypoints_scene[good_matches[i].trainIdx].pt );
		}

		//Find the Homography Matrix
		Mat H = findHomography( obj, scene, CV_RANSAC );

		// Use the homography Matrix to warp the images
		cv::Mat result;

		warpPerspective( left, result, H, cv::Size( left.cols+right.cols, left.rows) );
		cv::Mat half(result, cv::Rect(0, 0, right.cols, right.rows) );
		right.copyTo(half);
		result.copyTo(pano);

		// To remove the black portion after stitching, and confine in a rectangular region//

		// vector with all non-black point positions
		std::vector<cv::Point> nonBlackList;
		nonBlackList.reserve(result.rows*result.cols);

		// add all non-black points to the vector
		// there are more efficient ways to iterate through the image
		for(int j=0; j<result.rows; ++j)
				for(int i=0; i<result.cols; ++i)
				{
						// if not black: add to the list
						if(result.at<cv::Vec3b>(j,i) != cv::Vec3b(0,0,0))
						{
								nonBlackList.push_back(cv::Point(i,j));
						}
				}
/*
		// create bounding rect around those points
		cv::Rect bb = cv::boundingRect(nonBlackList);

		// display result
		namedWindow( "Result", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
		cv::imshow("Result", result(bb));
		waitKey(0);	
		destroyAllWindows();
*/
	return EXIT_SUCCESS;
}

int Joiner::stitchs2(const Mat &left, const Mat &right, Mat &pano)
{

	
	if (!right.data || !left.data)
	{
		cout << "Error : Images are not loaded..!!" << endl;
		return -1;
	}

	///Convert to gray-scale.

	cv::Mat gray_rightImg;
	cv::Mat gray_leftImg;
	cvtColor(right, gray_rightImg, CV_RGB2GRAY);
	cvtColor(left, gray_leftImg, CV_RGB2GRAY);


	///Detect key-points/features.

	int minPoints = 600;
	SurfFeatureDetector detector(minPoints);
	std::vector< KeyPoint > keypoints_right, keypoints_left;
	detector.detect(gray_rightImg, keypoints_right);
	detector.detect(gray_leftImg, keypoints_left);

	///Extract features.

	SurfDescriptorExtractor extractor;
	cv::Mat descriptors_right, descriptors_left;
	extractor.compute(gray_rightImg, keypoints_right, descriptors_right);
	extractor.compute(gray_leftImg, keypoints_left, descriptors_left);

	///Match features.

	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_right, descriptors_left, matches);
	double max_dist = 0; double min_dist = 100;

	///Reject the outliers and estimate homography using RANSAC.

	for( int i = 0; i < descriptors_right.rows; i++ )
	{
		double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}

	std::vector< DMatch > good_matches;
	for( int i = 0; i < descriptors_right.rows; i++ )
	{
		if( matches[i].distance < 3*min_dist ) good_matches.push_back( matches[i]);
	}
	std::vector< Point2f > obj;
	std::vector< Point2f > scene;

	for( unsigned int i = 0; i < good_matches.size(); i++ )
	{
		obj.push_back( keypoints_right[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_left[ good_matches[i].trainIdx ].pt );
	}
	cv::Mat H = findHomography( obj, scene, CV_RANSAC );

	///Warp the image using estimated homography and blend the images.

	cv::Mat result;
	warpPerspective(right,result,H,cv::Size(right.cols+left.cols,right.rows));
	cv::Mat half(result,cv::Rect(0,0,left.cols,left.rows));
	left.copyTo(half);
	result.copyTo(pano);
	
	//namedWindow( "Result", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	//imshow( "Result", result );	
	//waitKey(0);	
	//destroyAllWindows();

	return EXIT_SUCCESS;
}

int Joiner::stitch1(const Mat &left, const Mat &right, Mat &pano)
{
/*
	cv::Mat originalImage;
	cv::Rect faceRect;
	cv::Mat croppedFaceImage;

	croppedFaceImage = originalImage(faceRect).clone();
*/
	// Assign left ROI
	Rect lRect;// = selectROI("Left", left, fromCenter, showCrosshair);

	// Select ROI
	Rect rRect;// =  selectROI("Right", right, fromCenter, showCrosshair);
	
	cv::Point lcorners[1][4];
	lcorners[0][0] = Point( config["left_video_top_right_x"], config["left_video_top_right_y"] );
	lcorners[0][1] = Point( config["left_video_top_left_x"], config["left_video_top_left_y"] );
	lcorners[0][2] = Point( config["left_video_bottom_left_x"], config["left_video_bottom_left_y"] );
	lcorners[0][3] = Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] );
	const Point* lcorner_list[1] = { lcorners[0] };

	cv::Point rcorners[1][4];
	rcorners[0][0] = Point( config["right_video_top_right_x"], config["right_video_top_right_y"] );
	rcorners[0][1] = Point( config["right_video_top_left_x"], config["right_video_top_left_y"] );
	rcorners[0][2] = Point( config["right_video_bottom_left_x"], config["right_video_bottom_left_y"] );
	rcorners[0][3] = Point( config["right_video_bottom_right_x"], config["right_video_bottom_right_y"] );
	const Point* rcorner_list[1] = { rcorners[0] };
	
	int num_points = 4;
	int num_polygons = 1;
	int line_type = 8;
/*
    //C++: Rect boundingRect(InputArray points)
	//C++: Mat::Mat(int rows, int cols, int type, const Scalar& s)
	int lheight=abs(std::max(, ));
	int lwidth
	int rheight
	int rwidth
*/	
	cv::Mat lmask(bufVideoInfo.height,bufVideoInfo.width,CV_8UC3, cv::Scalar(0,0,0));
	cv::Mat rmask(bufVideoInfo.height,bufVideoInfo.width,CV_8UC3, cv::Scalar(0,0,0));
	
	cv::fillPoly( lmask, lcorner_list, &num_points, num_polygons, cv::Scalar( 255, 255, 255 ),  line_type);
	cv::fillPoly( rmask, rcorner_list, &num_points, num_polygons, cv::Scalar( 255, 255, 255 ),  line_type);
	
	///cv::Mat result;
	///cv::bitwise_and(source_image, mask, result);

	vector<Mat> imgs;
	vector<Mat> masks;


	if (config["divide_images"]) {
/*		
		Rect rect(0, 0, left.cols / 2, left.rows);
		imgs.push_back(left(rect).clone());
		masks.push_back(left(rect & lRect).clone());
		imgs.push_back(right(rect).clone());
		masks.push_back(right(rect & rRect).clone());

		rect.x = left.cols / 3;
		imgs.push_back(left(rect).clone());
		masks.push_back(left(rect & lRect).clone());
		imgs.push_back(right(rect).clone());
		masks.push_back(right(rect & rRect).clone());

		rect.x = left.cols / 2;
		imgs.push_back(left(rect).clone());
		masks.push_back(left(rect & lRect).clone());
		imgs.push_back(right(rect).clone());
		masks.push_back(right(rect & rRect).clone());
*/
	} else {
		imgs.push_back(left);
		masks.push_back(left(lRect).clone());
		imgs.push_back(right);
		masks.push_back(right(rRect).clone());
	}
	
	//Status Stitcher::stitch(InputArray images, OutputArray pano)
	//Status Stitcher::stitch(InputArray images, const std::vector<std::vector<Rect>>& rois, OutputArray pano)

	//Stitcher stitcher = Stitcher::createDefault(config["try_use_gpu"]);
	//Stitcher::Status status = stitcher->stitch(masks, pano);
/*
	if (status != Stitcher::OK) {
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return EXIT_FAILURE;
	}
*/
	return EXIT_SUCCESS;

}

int Joiner::stitchsg(const Mat &left, const Mat &right, Mat &pano)
{
	
	/**************** Imagen izquierda ****************/
	imshow("Display video", left); waitKey(0);

	///Primero rotar la imagen de la izquierda sacando los puntos iniciales y los puntos finales de tres vertices
	//int oside1 = abs(config["left_video_bottom_right_y"] - config["left_video_bottom_left_y"]);
	//int aside1 = abs(config["left_video_bottom_right_x"] - config["left_video_bottom_left_x"]);
	
	//int oside2 = abs(config["left_video_top_right_y"] - config["left_video_bottom_right_y"]);
	//int aside2 = abs(config["left_video_top_right_x"] - config["left_video_bottom_right_x"]);
	
	Point left_bottom_left_point, left_top_right_point, left_bottom_right_point;
	left_bottom_left_point = Point( config["left_video_bottom_left_x"], config["left_video_bottom_left_y"] );
	left_top_right_point = Point( config["left_video_top_right_x"], config["left_video_top_right_y"] );
	left_bottom_right_point = Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] );
	
	double bottom_left_hypotenuse = cv::norm(cv::Mat(left_bottom_right_point),cv::Mat(left_bottom_left_point));
	double top_left_hypotenuse = cv::norm(cv::Mat(left_bottom_right_point),cv::Mat(left_top_right_point));
	
	//double tanx1 = (double) oside1/aside1;
	//double left_rot_angle = (double) atan (tanx1) * 180.0 / J_PI;
	//cout << "Left horizontal tanx: " << tanx1 << " and angle: " << left_rot_angle <<endl;
	
	//double tanx2 = (double) oside2/aside2;
	//double left_vert_angle = (double) atan (tanx2) * 180.0 / J_PI;
	//cout << "Left vertical tanx: " << tanx2 << " and angle: " << left_vert_angle <<endl;
	
	Point left_bottom_left_rot_point, left_top_right_rot_point;
	left_bottom_left_rot_point.x = bottom_left_hypotenuse * cos(J_PI)+config["left_video_bottom_right_x"];
	left_bottom_left_rot_point.y = bottom_left_hypotenuse * sin(J_PI)+config["left_video_bottom_right_y"];
	//cout << "Bottom left hypotenuse: " << bottom_left_hypotenuse << " rot x: " << left_bottom_left_rot_point.x << " rot y: " << left_bottom_left_rot_point.y << endl;
	
	left_top_right_rot_point.x = config["left_video_bottom_right_x"];
	left_top_right_rot_point.y = config["left_video_top_right_y"]-top_left_hypotenuse * sin(J_PI);
	//cout << "Top left hypotenuse: " << top_left_hypotenuse << " rot x: " << left_top_right_rot_point.x << " rot y: " << left_top_right_rot_point.y << endl;
	
	std::vector<cv::Point2f> vect_lsrc_points;
	vect_lsrc_points.push_back(left_bottom_left_point);
	vect_lsrc_points.push_back(left_bottom_right_point);
	vect_lsrc_points.push_back(left_top_right_point);
	cv::Mat mat_lsrc_points(vect_lsrc_points);
	
	std::vector<cv::Point2f> vect_ldst_points;
	vect_ldst_points.push_back(left_bottom_left_rot_point);
	vect_ldst_points.push_back(left_bottom_right_point);
	vect_ldst_points.push_back(left_top_right_rot_point);
	cv::Mat mat_ldst_points(vect_ldst_points);
	
	///Ahora se alinea la horizontal y toda la imagen con ello
	Mat ltransform_mat = getAffineTransform(mat_lsrc_points, mat_ldst_points);
	Mat rot_left;
	warpAffine(left,rot_left,ltransform_mat,Size(left.cols,left.rows));
	imshow("Display video", rot_left); waitKey(0);
	cout << "Left matrix transformation size : " << rot_left.cols << " x " << rot_left.rows << endl; 


	
	/**************** Imagen derecha ****************/
	imshow("Display video", right); waitKey(0);
	//oside1 = abs(config["right_video_bottom_right_y"] - config["right_video_bottom_left_y"]);
	//aside1 = abs(config["right_video_bottom_right_x"] - config["right_video_bottom_left_x"]);

	//oside2 = abs(config["right_video_bottom_left_y"] - config["right_video_top_left_y"]);
	//aside2 = abs(config["right_video_top_left_x"] - config["right_video_bottom_left_x"]);

	Point right_bottom_left_point, right_top_left_point, right_bottom_right_point;
	right_bottom_left_point = Point( config["right_video_bottom_left_x"], config["right_video_bottom_left_y"] );
	right_top_left_point = Point( config["right_video_top_right_x"], config["right_video_top_right_y"] );
	right_bottom_right_point = Point( config["right_video_bottom_right_x"], config["right_video_bottom_right_y"] );

	double bottom_right_hypotenuse = cv::norm(cv::Mat(right_bottom_right_point),cv::Mat(right_bottom_left_point));
	double top_right_hypotenuse = cv::norm(cv::Mat(right_bottom_left_point),cv::Mat(right_top_left_point));

	//tanx1 = (double) oside1/aside1;
	//double right_rot_angle = (double) atan (tanx1) * 180.0 / J_PI;
	//cout << "Right horizontal tanx: " << tanx1 << " and angle: " << right_rot_angle <<endl;
	
	//tanx2 = (double) oside2/aside2;
	//double right_vert_angle = (double) atan (tanx2) * 180.0 / J_PI;
	//cout << "Right vertical tanx: " << tanx2 << " and angle: " << right_vert_angle <<endl;

	Point right_bottom_right_rot_point, right_top_left_rot_point;
	right_bottom_right_rot_point.x = bottom_right_hypotenuse * cos(0.0)+config["right_video_bottom_left_x"];
	right_bottom_right_rot_point.y = bottom_right_hypotenuse * sin(0.0)+config["right_video_bottom_left_y"];
	
	right_top_left_rot_point.x = config["right_video_bottom_left_x"];
	right_top_left_rot_point.y = config["right_video_top_left_y"]-top_right_hypotenuse * sin(0);
	//cout << "Top right hypotenuse: " << top_right_hypotenuse << " rot x: " << right_top_left_rot_point.x << " rot y: " << right_top_left_rot_point.y << endl;

	std::vector<cv::Point2f> vect_rsrc_points;
	vect_rsrc_points.push_back(right_bottom_left_point);
	vect_rsrc_points.push_back(right_bottom_right_point);
	vect_rsrc_points.push_back(right_top_left_point);
	cv::Mat mat_rsrc_points(vect_rsrc_points);
	
	std::vector<cv::Point2f> vect_rdst_points;
	vect_rdst_points.push_back(right_bottom_left_point);
	vect_rdst_points.push_back(right_bottom_right_rot_point);
	vect_rdst_points.push_back(right_top_left_rot_point);
	cv::Mat mat_rdst_points(vect_rdst_points);

	///Ahora se alinea la horizontal y toda la imagen con ello
	Mat rtransform_mat = getAffineTransform(mat_rsrc_points, mat_rdst_points);
	Mat rot_right;
	warpAffine(right,rot_right,rtransform_mat,Size(right.cols,right.rows));
	imshow("Display video", rot_right); waitKey(0);
	cout << "Right matrix transformation size : " << rot_right.cols << " x " << rot_right.rows << endl; 

	exit(0);
	return EXIT_SUCCESS;
}

void Joiner::Find_Transform_Matrixs(void)
{
	JGpu gp;
	if(config["try_use_gpu"])
		gp.Set();
	

	/**************** Imagen izquierda: rotacion y shear ****************/

	
	Point left_bottom_left_point, left_top_right_point, left_bottom_right_point, left_top_left_point;
	left_bottom_left_point = Point( config["left_video_bottom_left_x"], config["left_video_bottom_left_y"] );
	left_top_right_point = Point( config["left_video_top_right_x"], config["left_video_top_right_y"] );
	left_bottom_right_point = Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] );
	left_top_left_point = Point( config["left_video_top_left_x"], config["left_video_top_left_y"] );
	
	double bottom_left_hypotenuse = cv::norm(cv::Mat(left_bottom_right_point),cv::Mat(left_bottom_left_point));
	double side_left_hypotenuse = cv::norm(cv::Mat(left_bottom_right_point),cv::Mat(left_top_right_point));
		
	Point left_bottom_left_rot_point, left_top_right_rot_point, left_top_left_rot_point, left_bottom_right_rot_point;
	left_bottom_left_rot_point.x = bottom_left_hypotenuse * cos(J_PI)+config["left_video_bottom_right_x"];
	left_bottom_left_rot_point.y = bottom_left_hypotenuse * sin(J_PI)+config["left_video_bottom_right_y"];
	//cout << "Bottom left hypotenuse: " << bottom_left_hypotenuse << " rot x: " << left_bottom_left_rot_point.x << " rot y: " << left_bottom_left_rot_point.y << endl;
	
	left_top_right_rot_point.x = config["left_video_bottom_right_x"];
	left_top_right_rot_point.y = config["left_video_top_right_y"]-side_left_hypotenuse * sin(J_PI);
	//cout << "Top left hypotenuse: " << top_left_hypotenuse << " rot x: " << left_top_right_rot_point.x << " rot y: " << left_top_right_rot_point.y << endl;
	left_top_left_rot_point.x = left_top_left_point.x;
	if(config["enable_top_corners_correction"])
		left_top_left_rot_point.x= left_top_left_rot_point.x+abs(left_bottom_right_point.x-left_top_right_point.x)+config["left_top_corner_correction"];
	left_top_left_rot_point.y = left_top_right_rot_point.y;
	
	left_bottom_right_rot_point.x = left_bottom_right_point.x;
	left_bottom_right_rot_point.y = left_bottom_right_point.y;
	
	std::vector<cv::Point2f> vect_lsrc_points;
	vect_lsrc_points.push_back(left_bottom_left_point);
	vect_lsrc_points.push_back(left_bottom_right_point);
	vect_lsrc_points.push_back(left_top_right_point);
	vect_lsrc_points.push_back(left_top_left_point);
	cv::Mat mat_lsrc_points(vect_lsrc_points);
	
	std::vector<cv::Point2f> vect_ldst_points;
	vect_ldst_points.push_back(left_bottom_left_rot_point);
	vect_ldst_points.push_back(left_bottom_right_rot_point);
	vect_ldst_points.push_back(left_top_right_rot_point);
	vect_ldst_points.push_back(left_top_left_rot_point);
	cv::Mat mat_ldst_points(vect_ldst_points);
	
	///Ahora se alinea la linea horizontal y la linea vertical del campo con una transformacion de perspectiva
	ltransform_mat = getPerspectiveTransform(mat_lsrc_points, mat_ldst_points);

	
	/**************** Imagen derecha: rotacion y shear ****************/


	Point right_bottom_left_point, right_bottom_right_point, right_top_right_point,right_top_left_point;
	right_bottom_left_point = Point( config["right_video_bottom_left_x"], config["right_video_bottom_left_y"] );
	right_bottom_right_point = Point( config["right_video_bottom_right_x"], config["right_video_bottom_right_y"] );
	right_top_right_point = Point( config["right_video_top_right_x"], config["right_video_top_right_y"] );
	right_top_left_point = Point( config["right_video_top_left_x"], config["right_video_top_left_y"] );	

	double bottom_right_hypotenuse = cv::norm(cv::Mat(right_bottom_right_point),cv::Mat(right_bottom_left_point));
	double side_right_hypotenuse = cv::norm(cv::Mat(right_bottom_left_point),cv::Mat(right_top_left_point));

	Point right_bottom_right_rot_point, right_top_left_rot_point, right_top_right_rot_point, right_bottom_left_rot_point;
	right_bottom_right_rot_point.x = bottom_right_hypotenuse * cos(0.0)+config["right_video_bottom_left_x"];
	right_bottom_right_rot_point.y = bottom_right_hypotenuse * sin(0.0)+config["right_video_bottom_left_y"];
	
	right_top_left_rot_point.x = config["right_video_bottom_left_x"];
	right_top_left_rot_point.y = left_top_right_rot_point.y-side_right_hypotenuse * sin(0.0);
	
	right_top_right_rot_point.x = right_top_right_point.x;
	if(config["enable_top_corners_correction"])
		right_top_right_rot_point.x = right_top_right_rot_point.x - abs(right_bottom_left_point.x-right_top_left_point.x)+config["right_top_corner_correction"];
	right_top_right_rot_point.y = right_top_left_rot_point.y;

	right_bottom_left_rot_point.x = config["right_video_bottom_left_x"];
	right_bottom_left_rot_point.y = config["right_video_bottom_left_y"];

	std::vector<cv::Point2f> vect_rsrc_points;
	vect_rsrc_points.push_back(right_bottom_left_point);
	vect_rsrc_points.push_back(right_bottom_right_point);
	vect_rsrc_points.push_back(right_top_left_point);	
	vect_rsrc_points.push_back(right_top_right_point);
	cv::Mat mat_rsrc_points(vect_rsrc_points);
	
	std::vector<cv::Point2f> vect_rdst_points;
	vect_rdst_points.push_back(right_bottom_left_rot_point);
	vect_rdst_points.push_back(right_bottom_right_rot_point);
	vect_rdst_points.push_back(right_top_left_rot_point);
	vect_rdst_points.push_back(right_top_right_rot_point);
	cv::Mat mat_rdst_points(vect_rdst_points);

	///Ahora se alinea la linea horizontal y la linea vertical del campo con una transformacion de perspectiva
	rtransform_mat = getPerspectiveTransform(mat_rsrc_points, mat_rdst_points);
	
	/**************** Encontrar matriz de transformacion para calzar el ancho de la cancha****************/
	double top_horizontal_line = (left_top_right_rot_point.y+right_top_left_rot_point.y)/2.0; cout << "Linea horizontal arriba: " << top_horizontal_line << endl;
	double bottom_horizontal_line = (left_bottom_right_rot_point.y+right_bottom_left_rot_point.y)/2.0; cout << "Linea horizontal abajo: " << bottom_horizontal_line << endl;
	
	/**************** Imagen izquierda: estiramiento vertical****************/
	std::vector<cv::Point2f> vect_lssrc_points;
	vect_lssrc_points.push_back(left_bottom_left_rot_point);
	vect_lssrc_points.push_back(left_bottom_right_rot_point);
	vect_lssrc_points.push_back(left_top_right_rot_point);
	vect_lssrc_points.push_back(left_top_left_rot_point);
	cv::Mat mat_lssrc_points(vect_lssrc_points);
	
	Point left_bottom_left_stretch_point, left_bottom_right_stretch_point, left_top_left_stretch_point, left_top_right_stretch_point;
	left_bottom_left_stretch_point.x = left_bottom_left_rot_point.x;
	left_bottom_left_stretch_point.y = bottom_horizontal_line;
		
	left_bottom_right_stretch_point.x = left_bottom_right_rot_point.x;
	left_bottom_right_stretch_point.y = bottom_horizontal_line;

	left_top_right_stretch_point.x = left_top_right_rot_point.x;
	left_top_right_stretch_point.y = top_horizontal_line;
	
	left_top_left_stretch_point.x = left_top_left_rot_point.x;
	left_top_left_stretch_point.y = top_horizontal_line;
	
	std::vector<cv::Point2f> vect_lsdst_points;
	vect_lsdst_points.push_back(left_bottom_left_stretch_point);
	vect_lsdst_points.push_back(left_bottom_right_stretch_point);
	vect_lsdst_points.push_back(left_top_right_stretch_point);
	vect_lsdst_points.push_back(left_top_left_stretch_point);
	cv::Mat mat_lsdst_points(vect_ldst_points);
	
	//cout << "Left stretch mat" << endl;
	sltransform_mat = getPerspectiveTransform(vect_lssrc_points, vect_lsdst_points);
/*
	try{
		sltransform_mat = getAffineTransform(vect_lssrc_points, vect_lsdst_points);
	}
	catch (const cv::Exception& e) {		
		cout << "sltransform cols: " << sltransform_mat.cols << " rows: " << sltransform_mat.rows << endl;
		cout << "left vect dest size: " << vect_ldst_points.size() << endl;
		cout << "left vect src size: " << vect_lsrc_points.size() << endl;
		cerr << e.what() << endl;
		exit(1);
	}
*/	
	/**************** Imagen derecha: estiramiento vertical****************/
	std::vector<cv::Point2f> vect_rssrc_points;
	vect_rssrc_points.push_back(right_bottom_left_rot_point);
	vect_rssrc_points.push_back(right_bottom_right_rot_point);
	vect_rssrc_points.push_back(right_top_left_rot_point);
	vect_rssrc_points.push_back(right_top_right_rot_point);
	cv::Mat mat_rssrc_points(vect_rssrc_points);
		
	Point right_bottom_left_stretch_point, right_bottom_right_stretch_point, right_top_left_stretch_point, right_top_right_stretch_point;
	right_bottom_left_stretch_point.x = right_bottom_left_rot_point.x;
	right_bottom_left_stretch_point.y = bottom_horizontal_line;
	
	right_bottom_right_stretch_point.x = right_bottom_right_rot_point.x;
	right_bottom_right_stretch_point.y = bottom_horizontal_line;

	right_top_right_stretch_point.x = right_top_right_rot_point.x;
	right_top_right_stretch_point.y = top_horizontal_line;
	
	right_top_left_stretch_point.x = right_top_left_rot_point.x;
	right_top_left_stretch_point.y = top_horizontal_line;
	
	std::vector<cv::Point2f> vect_rsdst_points;
	vect_rsdst_points.push_back(right_bottom_left_stretch_point);
	vect_rsdst_points.push_back(right_bottom_right_stretch_point);
	vect_rsdst_points.push_back(right_top_left_stretch_point);
	vect_rsdst_points.push_back(right_top_right_stretch_point);	
	cv::Mat mat_rsdst_points(vect_rdst_points);
	
	//cout << "Right stretch mat" << endl;
	srtransform_mat = getPerspectiveTransform(vect_rssrc_points, vect_rsdst_points);
/*
	try{
		srtransform_mat = getAffineTransform(vect_rssrc_points, vect_rsdst_points);	
	}
	catch (const cv::Exception& e) {
		cout << "srtransform cols: " << srtransform_mat.cols << " rows: " << srtransform_mat.rows << endl;
		cout << "right vect dest size: " << vect_rsdst_points.size() << endl;
		cout << "right vect src size: " << vect_rssrc_points.size() << endl;
		cerr << e.what() << endl;
		exit(1);
	}	
*/
}

int Joiner::stitchsp2(const Mat &left, const Mat &right, Mat &pano)
{

	JGpu gp;
	if(config["try_use_gpu"])
		gp.Set();
	
	//namedWindow( "Display video", CV_WINDOW_NORMAL | CV_GUI_NORMAL);	
	//imshow("Display video", left); waitKey(0);	
	//imshow("Display video", right); waitKey(0);

	/**************** Imagen izquierda ****************/
	Point left_bottom_left_point, left_top_right_point, left_bottom_right_point, left_top_left_point;
	left_bottom_left_point = Point( config["left_video_bottom_left_x"], config["left_video_bottom_left_y"] );
	left_top_right_point = Point( config["left_video_top_right_x"], config["left_video_top_right_y"] );
	left_bottom_right_point = Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] );
	left_top_left_point = Point( config["left_video_top_left_x"], config["left_video_top_left_y"] );	
	Mat rot_left, stretch_left;	
	
	if(config["try_use_gpu"])
		gp.gpuwarpPerspective(left,rot_left,ltransform_mat,Size(left.cols,left.rows));
	else
		warpPerspective(left,rot_left,ltransform_mat,Size(left.cols,left.rows));	

	//imshow("Display video", rot_left); waitKey(0);	

	if(config["try_use_gpu"])
		gp.gpuwarpPerspective(rot_left,stretch_left,sltransform_mat,Size(left.cols,left.rows));
		//gp.gpuwarpAffine(rot_left,stretch_left,sltransform_mat,Size(left.cols,left.rows));		
	else
		warpPerspective(rot_left,stretch_left,sltransform_mat,Size(left.cols,left.rows));	
		//warpAffine(rot_left,stretch_left,sltransform_mat,Size(left.cols,left.rows));	
	
	//imshow("Display video", stretch_left); waitKey(0);
	
	/**************** Imagen derecha ****************/
	Point right_bottom_left_point, right_bottom_right_point, right_top_right_point,right_top_left_point;
	right_bottom_left_point = Point( config["right_video_bottom_left_x"], config["right_video_bottom_left_y"] );
	right_bottom_right_point = Point( config["right_video_bottom_right_x"], config["right_video_bottom_right_y"] );
	right_top_right_point = Point( config["right_video_top_right_x"], config["right_video_top_right_y"] );
	right_top_left_point = Point( config["right_video_top_left_x"], config["right_video_top_left_y"] );		
	Mat rot_right,stretch_right;
	
	if(config["try_use_gpu"])
		gp.gpuwarpPerspective(right,rot_right,rtransform_mat,Size(right.cols,right.rows));	
	else	
		warpPerspective(right,rot_right,rtransform_mat,Size(right.cols,right.rows));	

	//imshow("Display video", rot_right); waitKey(0);

	if(config["try_use_gpu"])
		gp.gpuwarpPerspective(rot_right,stretch_right,srtransform_mat,Size(right.cols,right.rows));	
		//gp.gpuwarpAffine(rot_right,stretch_right,srtransform_mat,Size(right.cols,right.rows));	
	else	
		warpPerspective(rot_right,stretch_right,srtransform_mat,Size(right.cols,right.rows));			
		//warpAffine(rot_right,stretch_right,srtransform_mat,Size(right.cols,right.rows));			

	//imshow("Display video", stretch_right); waitKey(0);

	///Se juntan ambos lados para tener una imagen panoramica
	Mat left_cut_frame(left.rows,left_bottom_right_point.x,left.type());
	Rect lRect(Point(0,0), Point(left_bottom_right_point.x,left.rows));
	left_cut_frame = stretch_left(lRect);
	//left_cut_frame = rot_left(lRect);
	
	//imshow("Display video", left_cut_frame); waitKey(0);

	Mat right_cut_frame(right.rows,abs(right.cols - right_bottom_left_point.x),right.type());
	Rect rRect(Point(right_bottom_left_point.x,left.rows),Point(right.cols,0));	
	right_cut_frame = stretch_right(rRect);
	//right_cut_frame = rot_right(rRect);
	
	//imshow("Display video", right_cut_frame); waitKey(0);

	Mat join_left_right_frame(right_cut_frame.rows,(left_cut_frame.cols+right_cut_frame.cols),right.type());
	
	///Antes de unir realizar una ecualizacion de color
	if(config["enable_s_color_correction"] || config["enable_v_color_correction"]){
		Mat equ_left_cut_frame(left_cut_frame.rows,left_cut_frame.cols,left_cut_frame.type());
		Mat equ_right_cut_frame(right_cut_frame.rows,right_cut_frame.cols,right_cut_frame.type());
		equ_left_cut_frame = equalizeColor(left_cut_frame);
		equ_right_cut_frame = equalizeColor(right_cut_frame);
	
		equ_left_cut_frame.copyTo(join_left_right_frame(Rect(0, 0, left_cut_frame.cols, left_cut_frame.rows)));
		equ_right_cut_frame.copyTo(join_left_right_frame(Rect(left_cut_frame.cols, 0, right_cut_frame.cols, right_cut_frame.rows)));
	}
	else{
		left_cut_frame.copyTo(join_left_right_frame(Rect(0, 0, left_cut_frame.cols, left_cut_frame.rows)));
		right_cut_frame.copyTo(join_left_right_frame(Rect(left_cut_frame.cols, 0, right_cut_frame.cols, right_cut_frame.rows)));		
	}

	
	if(config["enable_color_balance"]){
		Mat cb_join_left_right_frame;
		SimplestCB(join_left_right_frame,cb_join_left_right_frame,1);
		//imshow("Display video", cb_join_left_right_frame); waitKey(0);
		pano = cb_join_left_right_frame;
	}
	else
		pano = join_left_right_frame;

	//imshow("Display video", pano); waitKey(0);
	//exit(0);
	return EXIT_SUCCESS;	
}

int Joiner::stitchsp1(const Mat &left, const Mat &right, Mat &pano)
{
	
	/**************** Imagen izquierda ****************/
	///Primero se obtienen los puntos de referencia antes y despues de la transformacion
	//imshow("Display video", left); waitKey(0);

	JGpu gp;
	if(config["try_use_gpu"])
		gp.Set();
	

	///Primero rotar la imagen de la izquierda sacando los puntos iniciales y los puntos finales de tres vertices

	
	Point left_bottom_left_point, left_top_right_point, left_bottom_right_point, left_top_left_point;
	left_bottom_left_point = Point( config["left_video_bottom_left_x"], config["left_video_bottom_left_y"] );
	left_top_right_point = Point( config["left_video_top_right_x"], config["left_video_top_right_y"] );
	left_bottom_right_point = Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] );
	left_top_left_point = Point( config["left_video_top_left_x"], config["left_video_top_left_y"] );
	
	double bottom_left_hypotenuse = cv::norm(cv::Mat(left_bottom_right_point),cv::Mat(left_bottom_left_point));
	double top_left_hypotenuse = cv::norm(cv::Mat(left_bottom_right_point),cv::Mat(left_top_right_point));
	

	
	Point left_bottom_left_rot_point, left_top_right_rot_point, left_top_left_rot_point;
	left_bottom_left_rot_point.x = bottom_left_hypotenuse * cos(J_PI)+config["left_video_bottom_right_x"];
	left_bottom_left_rot_point.y = bottom_left_hypotenuse * sin(J_PI)+config["left_video_bottom_right_y"];
	//cout << "Bottom left hypotenuse: " << bottom_left_hypotenuse << " rot x: " << left_bottom_left_rot_point.x << " rot y: " << left_bottom_left_rot_point.y << endl;
	
	left_top_right_rot_point.x = config["left_video_bottom_right_x"];
	left_top_right_rot_point.y = config["left_video_top_right_y"]-top_left_hypotenuse * sin(J_PI);
	//cout << "Top left hypotenuse: " << top_left_hypotenuse << " rot x: " << left_top_right_rot_point.x << " rot y: " << left_top_right_rot_point.y << endl;
	left_top_left_rot_point.x = left_top_left_point.x;
	left_top_left_rot_point.y = left_top_right_rot_point.y;
	
	std::vector<cv::Point2f> vect_lsrc_points;
	vect_lsrc_points.push_back(left_bottom_left_point);
	vect_lsrc_points.push_back(left_bottom_right_point);
	vect_lsrc_points.push_back(left_top_right_point);
	vect_lsrc_points.push_back(left_top_left_point);
	cv::Mat mat_lsrc_points(vect_lsrc_points);
	
	std::vector<cv::Point2f> vect_ldst_points;
	vect_ldst_points.push_back(left_bottom_left_rot_point);
	vect_ldst_points.push_back(left_bottom_right_point);
	vect_ldst_points.push_back(left_top_right_rot_point);
	vect_ldst_points.push_back(left_top_left_rot_point);
	cv::Mat mat_ldst_points(vect_ldst_points);
	
	///Ahora se alinea la linea horizontal y la linea vertical del campo con una transformacion de perspectiva
	ltransform_mat = getPerspectiveTransform(mat_lsrc_points, mat_ldst_points);
	Mat rot_left;	
	
	if(config["try_use_gpu"])
		gp.gpuwarpPerspective(left,rot_left,ltransform_mat,Size(left.cols,left.rows));
	else
		warpPerspective(left,rot_left,ltransform_mat,Size(left.cols,left.rows));
	//imshow("Display video", rot_left); waitKey(0);
	//cout << "Left matrix transformation size : " << rot_left.cols << " x " << rot_left.rows << endl; 


	
	/**************** Imagen derecha ****************/
	///Primero se obtienen los puntos de referencia antes y despues de la transformacion
	//imshow("Display video", right); waitKey(0);


	Point right_bottom_left_point, right_bottom_right_point, right_top_right_point,right_top_left_point;
	right_bottom_left_point = Point( config["right_video_bottom_left_x"], config["right_video_bottom_left_y"] );
	right_bottom_right_point = Point( config["right_video_bottom_right_x"], config["right_video_bottom_right_y"] );
	right_top_right_point = Point( config["right_video_top_right_x"], config["right_video_top_right_y"] );
	right_top_left_point = Point( config["right_video_top_left_x"], config["right_video_top_left_y"] );	

	double bottom_right_hypotenuse = cv::norm(cv::Mat(right_bottom_right_point),cv::Mat(right_bottom_left_point));
	//double top_right_hypotenuse = cv::norm(cv::Mat(right_bottom_left_point),cv::Mat(right_top_left_point));



	Point right_bottom_right_rot_point, right_top_left_rot_point, right_top_right_rot_point;
	right_bottom_right_rot_point.x = bottom_right_hypotenuse * cos(0.0)+config["right_video_bottom_left_x"];
	right_bottom_right_rot_point.y = bottom_right_hypotenuse * sin(0.0)+config["right_video_bottom_left_y"];
	
	right_top_left_rot_point.x = config["right_video_bottom_left_x"];
	//right_top_left_rot_point.y = config["right_video_top_left_y"]-top_right_hypotenuse * sin(0);
	right_top_left_rot_point.y = left_top_right_rot_point.y;
	//cout << "Top right hypotenuse: " << top_right_hypotenuse << " rot x: " << right_top_left_rot_point.x << " rot y: " << right_top_left_rot_point.y << endl;
	right_top_right_rot_point.x = right_top_right_point.x;
	right_top_right_rot_point.y = right_top_left_rot_point.y;

	std::vector<cv::Point2f> vect_rsrc_points;
	vect_rsrc_points.push_back(right_bottom_left_point);
	vect_rsrc_points.push_back(right_bottom_right_point);
	vect_rsrc_points.push_back(right_top_left_point);	
	vect_rsrc_points.push_back(right_top_right_point);
	cv::Mat mat_rsrc_points(vect_rsrc_points);
	
	std::vector<cv::Point2f> vect_rdst_points;
	vect_rdst_points.push_back(right_bottom_left_point);
	vect_rdst_points.push_back(right_bottom_right_rot_point);
	vect_rdst_points.push_back(right_top_left_rot_point);
	vect_rdst_points.push_back(right_top_right_rot_point);
	cv::Mat mat_rdst_points(vect_rdst_points);

	///Ahora se alinea la linea horizontal y la linea vertical del campo con una transformacion de perspectiva
	rtransform_mat = getPerspectiveTransform(mat_rsrc_points, mat_rdst_points);
	Mat rot_right;
	if(config["try_use_gpu"])
		gp.gpuwarpPerspective(right,rot_right,rtransform_mat,Size(right.cols,right.rows));	
	else	
		warpPerspective(right,rot_right,rtransform_mat,Size(right.cols,right.rows));
	//imshow("Display video", rot_right); waitKey(0);
	//cout << "Right matrix transformation size : " << rot_right.cols << " x " << rot_right.rows << endl; 
	
	///Se juntan ambos lados para tener una imagen panoramica
	Mat left_cut_frame(left.rows,left_bottom_right_point.x,left.type());
	Rect lRect(Point(0,0), Point(left_bottom_right_point.x,left.rows));
	left_cut_frame = rot_left(lRect);
	//imshow("Display video", left_cut_frame); waitKey(0);

	Mat right_cut_frame(right.rows,abs(right.cols - right_bottom_left_point.x),right.type());
	//cout << "right cut frame: " << right_cut_frame.cols << " x " << right_cut_frame.rows << endl;
	Rect rRect(Point(right_bottom_left_point.x,left.rows),Point(right.cols,0));
	//cout << "right rect: " << rRect.width << " x " << rRect.height << endl;
	right_cut_frame = rot_right(rRect);
	//imshow("Display video", right_cut_frame); waitKey(0);

	Mat join_left_right_frame(right_cut_frame.rows,(left_cut_frame.cols+right_cut_frame.cols),right.type());
	
	///Antes de unir realizar una ecualizacion de color
	if(config["enable_hsv_color_correction"]){
		Mat equ_left_cut_frame(left_cut_frame.rows,left_cut_frame.cols,left_cut_frame.type());
		Mat equ_right_cut_frame(right_cut_frame.rows,right_cut_frame.cols,right_cut_frame.type());
		equ_left_cut_frame = equalizeColor(left_cut_frame);
		equ_right_cut_frame = equalizeColor(right_cut_frame);
	
		equ_left_cut_frame.copyTo(join_left_right_frame(Rect(0, 0, left_cut_frame.cols, left_cut_frame.rows)));
		equ_right_cut_frame.copyTo(join_left_right_frame(Rect(left_cut_frame.cols, 0, right_cut_frame.cols, right_cut_frame.rows)));
	}
	else{
		left_cut_frame.copyTo(join_left_right_frame(Rect(0, 0, left_cut_frame.cols, left_cut_frame.rows)));
		right_cut_frame.copyTo(join_left_right_frame(Rect(left_cut_frame.cols, 0, right_cut_frame.cols, right_cut_frame.rows)));		
	}

	//imshow("Display video", join_left_right_frame); waitKey(0);
	
	pano = join_left_right_frame;

	//exit(0);
	return EXIT_SUCCESS;
}

int Joiner::stitchsgrs(const Mat &left, const Mat &right, Mat &pano)
{
	
	/**************** Imagen izquierda ****************/
	imshow("Display video", left); waitKey(0);

	///Primero rotar la imagen de la izquierda para que linea horizontal este realmente en la horizontal
	int oside1 = abs(config["left_video_bottom_right_y"] - config["left_video_bottom_left_y"]);
	int aside1 = abs(config["left_video_bottom_right_x"] - config["left_video_bottom_left_x"]);
	
	int oside2 = abs(config["left_video_top_right_y"] - config["left_video_bottom_right_y"]);
	int aside2 = abs(config["left_video_top_right_x"] - config["left_video_bottom_right_x"]);
	
	double bottom_left_hypotenuse = cv::norm(cv::Mat(Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] )),cv::Mat(Point( config["left_video_bottom_left_x"], config["left_video_bottom_left_y"] )));
	double top_left_hypotenuse = cv::norm(cv::Mat(Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] )),cv::Mat(Point( config["left_video_top_right_x"], config["left_video_top_right_y"] )));
	
	double tanx1 = (double) oside1/aside1;
	double left_rot_angle = (double) atan (tanx1) * 180.0 / J_PI;
	//cout << "Left horizontal tanx: " << tanx1 << " and angle: " << left_rot_angle <<endl;
	
	double tanx2 = (double) oside2/aside2;
	//double left_vert_angle = (double) atan (tanx2) * 180.0 / J_PI;
	//cout << "Left vertical tanx: " << tanx2 << " and angle: " << left_vert_angle <<endl;
	
	Point left_bottom_left_rot_point, left_top_right_rot_point;
	left_bottom_left_rot_point.x = bottom_left_hypotenuse * cos(J_PI)+config["left_video_bottom_right_x"];
	left_bottom_left_rot_point.y = bottom_left_hypotenuse * sin(J_PI)+config["left_video_bottom_right_y"];
	//cout << "Bottom left hypotenuse: " << bottom_left_hypotenuse << " rot x: " << left_bottom_left_rot_point.x << " rot y: " << left_bottom_left_rot_point.y << endl;
	
	left_top_right_rot_point.x = config["left_video_top_right_x"]-top_left_hypotenuse * sin(tanx1);
	left_top_right_rot_point.y = config["left_video_bottom_right_y"]-top_left_hypotenuse * sin(tanx2-tanx1*2);
	//cout << "Top left hypotenuse: " << top_left_hypotenuse << " rot x: " << left_top_right_rot_point.x << " rot y: " << left_top_right_rot_point.y << endl;
	
	Mat rot_mat = getRotationMatrix2D(Point( config["left_video_bottom_right_x"], config["left_video_bottom_right_y"] ), left_rot_angle, 1.0);
	Mat rot_left;
	warpAffine(left,rot_left,rot_mat,Size(left.cols,left.rows));
	//circle( rot_left, left_bottom_left_rot_point, 6, Scalar(0,0,255), 2, 8, 0 );
	//circle( rot_left, left_top_right_rot_point, 6, Scalar(0,0,255), 2, 8, 0 );
	imshow("Display video", rot_left); waitKey(0);
	cout << "Left matrix transformation size : " << rot_left.cols << " x " << rot_left.rows << endl; 
	
	///Ahora se alinea la horizontal y toda la imagen con ello
	double shear_factor = 0.7;
	
	Mat shear_left = shear(rot_left,shear_factor,0);
	imshow("Display video", shear_left); waitKey(0);
	cout << "Left shear matrix size : " << shear_left.cols << " x " << shear_left.rows << endl; 
	

/*
	double semi_shear_angle = abs(atan(tanx2)-atan(tanx1));
	
	cv::Mat t(2,3,CV_64F, cvScalar(0.0));

    t.at<double>(0,0) = 1;    
    t.at<double>(1,1) = 1; 
    t.at<double>(0,1) = -tan(0.17);
    
    cv::Mat dest;
	cv::Size size(left.cols,left.rows);
	warpAffine(rot_left, dest, t, size, INTER_LINEAR, BORDER_CONSTANT);

	imshow("Display video", dest); waitKey(0);
*/
	
	/**************** Imagen derecha ****************/
	imshow("Display video", right); waitKey(0);

	///Ahora rotar la imagen de la derecha
	oside1 = abs(config["right_video_bottom_right_y"] - config["right_video_bottom_left_y"]);
	aside1 = abs(config["right_video_bottom_right_x"] - config["right_video_bottom_left_x"]);
	tanx1 = (double) oside1/aside1;
	double right_rot_angle = (double) -1.0 * atan (tanx1) * 180.0 / J_PI;
	//cout << "Left horizontal tanx: " << tanx << " and angle: " << left_rot_angle <<endl;

	rot_mat = getRotationMatrix2D(Point( config["right_video_bottom_left_x"], config["right_video_bottom_left_y"] ), right_rot_angle, 1.0);
	Mat rot_right;
	warpAffine(right,rot_right,rot_mat,Size(right.cols,right.rows));
	imshow("Display video", rot_right); waitKey(0);
	cout << "Right matrix transformation size : " << rot_right.cols << " x " << rot_right.rows << endl;

	///Ahora se alinea la horizontal y toda la imagen con ello


	Mat shear_right = shear(rot_right,shear_factor,0);
	imshow("Display video", shear_right); waitKey(0);
	cout << "Right shear matrix size : " << shear_right.cols << " x " << shear_right.rows << endl; 

	
	exit(0);
	return EXIT_SUCCESS;
}

cv::Mat Joiner::shear(const cv::Mat & input, float Bx, float By)
{
    if (Bx*By == 1)
    {
        throw("Shearing: Bx*By==1 is forbidden");
    }


    if (input.type() != CV_8UC3) return cv::Mat();

    // shearing:
    // x'=x+y·Bx
    // y'=y+x*By

    // shear the extreme positions to find out new image size:
    std::vector<cv::Point2f> extremePoints;
    extremePoints.push_back(cv::Point2f(0, 0));
    extremePoints.push_back(cv::Point2f(input.cols, 0));
    extremePoints.push_back(cv::Point2f(input.cols, input.rows));
    extremePoints.push_back(cv::Point2f(0, input.rows));

    for (unsigned int i = 0; i < extremePoints.size(); ++i)
    {
        cv::Point2f & pt = extremePoints[i];
        pt = cv::Point2f(pt.x + pt.y*Bx, pt.y + pt.x*By);
    }

    cv::Rect offsets = cv::boundingRect(extremePoints);

    cv::Point2f offset = -offsets.tl();
    cv::Size resultSize = offsets.size();

    cv::Mat shearedImage = cv::Mat::zeros(resultSize, input.type()); // every pixel here is implicitely shifted by "offset"

    // perform the shearing by back-transformation
    for (int j = 0; j < shearedImage.rows; ++j)
    {

        for (int i = 0; i < shearedImage.cols; ++i)
        {
            cv::Point2f pp(i, j);

            pp = pp - offset; // go back to original coordinate system

            // go back to original pixel:
            // x'=x+y·Bx
            // y'=y+x*By
            //   y = y'-x*By
            //     x = x' -(y'-x*By)*Bx 
            //     x = +x*By*Bx - y'*Bx +x'
            //     x*(1-By*Bx) = -y'*Bx +x'
            //     x = (-y'*Bx +x')/(1-By*Bx)

            cv::Point2f p;
            p.x = (-pp.y*Bx + pp.x) / (1 - By*Bx);
            p.y = pp.y - p.x*By;

            if ((p.x >= 0 && p.x < input.cols) && (p.y >= 0 && p.y < input.rows))
            {
                // TODO: interpolate, if wanted (p is floating point precision and can be placed between two pixels)!
                shearedImage.at<cv::Vec3b>(j, i) = input.at<cv::Vec3b>(p);
            }
        }
    }

    return shearedImage;
}

Mat Joiner::equalizeColor(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat hsv;
		
		JGpu gp;
		if(config["try_use_gpu"])
			gp.Set();

		if(config["try_use_gpu"])
			gp.gpucvtColor(inputImage,hsv,CV_BGR2HSV);
		else
			cvtColor(inputImage,hsv,CV_BGR2HSV);

        vector<Mat> channels;
        split(hsv,channels);

		if(config["enable_s_color_correction"])
			equalizeHist(channels[1], channels[1]); ///Esto como que normaliza el contraste
        if(config["enable_v_color_correction"])
			equalizeHist(channels[2], channels[2]); ///Esto aumenta el contraste aun mas

        Mat result;
        merge(channels,hsv);

		if(config["try_use_gpu"])
			gp.gpucvtColor(hsv,result,CV_HSV2BGR);
		else
			cvtColor(hsv,result,CV_HSV2BGR);

        return result;
    }
    return Mat();
}


/*
Mat Joiner::equalizeColor(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
		Mat temp;
		inputImage.copyTo(temp);

        vector<Mat> channels;
        split(temp,channels);

        equalizeHist(channels[0], channels[0]);
        equalizeHist(channels[1], channels[1]);
        equalizeHist(channels[2], channels[2]);

        merge(channels,temp);        

        return temp;
    }
    return Mat();
}
*/

void Joiner::SimplestCB(Mat& in, Mat& out, float percent) {
    assert(in.channels() == 3);
    assert(percent > 0 && percent < 100);

    float half_percent = percent / 200.0f;

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

Joiner::~Joiner()
{
	
}
