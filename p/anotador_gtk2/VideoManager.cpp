#include "VideoManager.h"
#include "Defs.h"

using namespace cv;
using namespace std;


VideoManager::VideoManager()
{
	frame_number = 0;
}


void VideoManager::SimpleVideoLoader(string input_video)
{
	current_video = input_video;
	ivideo.open(input_video);
	if (!ivideo.isOpened()) {
		//throw;
		cout << "Input video file not found." << endl;
		std::runtime_error ( "Failed to open file input file" );

		exit(1);
	}

	//Fill input video file information on struct MediaInfo
	bufVideoInfo.fps = ivideo.get(CV_CAP_PROP_FPS);
	bufVideoInfo.frames =  ivideo.get(CV_CAP_PROP_FRAME_COUNT);
	bufVideoInfo.width = ivideo.get(CV_CAP_PROP_FRAME_WIDTH);
	bufVideoInfo.height = ivideo.get(CV_CAP_PROP_FRAME_HEIGHT);
/*
	//loop from start to stop frame
	for(double i=0; i<=bufVideoInfo.frames; i++) {
		Mat iframe(bufVideoInfo.height,bufVideoInfo.width,CV_8UC3);
		ivideo >> iframe;

		//Safe frame on buffer
		mat_deque.push_back(iframe);
	}
	
	ivideo.release();
*/
}


void VideoManager::SimpleVideoSaver(MatBuffer &sequence, string output_video)
{


		int number_of_mats = sequence.size();

		//Create a VideoWriter
		cout << "Saving video. " << endl;
		
		VideoWriter ovideo(output_video, CV_FOURCC('P','I','M','1'), 30, Size(bufVideoInfo.width,bufVideoInfo.height), true);
		//Loop sequence
		for(int i=0; i<number_of_mats; i++) {

			Mat outMat;
			outMat=sequence.front();
			ovideo << outMat;
			sequence.pop_front();			
		}

	
}

cv::Mat VideoManager::GetFrame(int fnumber)
{	

	if((fnumber >= 0) & (fnumber <= bufVideoInfo.frames-1)){
		frame_number=fnumber;
		ivideo.set(CV_CAP_PROP_POS_FRAMES, frame_number);	
		Mat frame;
		ivideo >> frame;
		return frame;
		//return mat_deque[fnumber];
	}
	else{
		std::cout << "Frame out of boundary." << endl;
		Mat empty;
		return empty;
	}
}

cv::Mat VideoManager::GetNextFrame(void)
{
		if((frame_number >= 0) & (frame_number < bufVideoInfo.frames)){
			cv::Mat frame;
			frame_number++;
			ivideo.set(CV_CAP_PROP_POS_FRAMES, frame_number);	
			ivideo >> frame;
			//frame = mat_deque[frame_number];			
			return frame;
		}
		
		else{
			std::cout << "Frame out of boundary." << endl;
			Mat empty;
			return empty;
		}
}

void VideoManager::SetFrame(int fnumber)
{
	frame_number = fnumber;
}

void VideoManager::ReloadFrameFromVideo(int fnumber)
{
	
	ivideo.set(CV_CAP_PROP_POS_FRAMES, fnumber);	
	SetFrame(fnumber);
	//Mat frame;
	//ivideo >> frame;
	//mat_deque[fnumber]=frame;
	//ivideo.release();
}

VideoManager::~VideoManager()
{
	ivideo.release();
	mat_deque.shrink_to_fit();
}
