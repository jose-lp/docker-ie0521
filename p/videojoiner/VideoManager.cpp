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


void VideoManager::StartVideoSaver(string output_video)
{

		//Create a VideoWriter
		cout << "Saving video. " << endl;
		current_video = output_video;
		
		ovideo.open(output_video, CV_FOURCC('X','V','I','D'), 30, Size(bufVideoInfo.width,bufVideoInfo.height), true);

	
}

void VideoManager::StartVideoSaver(string output_video, int width, int height) //H264 y H265 max frame size 8192×4320 PIM1 a 4095x4095
{

		//Create a VideoWriter
		cout << "Saving video with dimensions of width: " << width << " height: " << height << endl;
		current_video = output_video;
		
		ovideo.open(output_video, CV_FOURCC('X','V','I','D'), 30, Size(width,height), true); //PIM1,HEVC,XVID,H264,DIVX,U263,I263
		
		if(!ovideo.isOpened())
			cout << "Error opening output video" << endl;
		else
			cout << "Output video opened" << endl;

	
}

void VideoManager::SimpleFrameSaver(Mat frame)
{
		
		if (!ovideo.isOpened()) {
			//throw;
			cout << "Output video file not found." << endl;
			std::runtime_error ( "Failed to open output file" );

			exit(1);
		}
		
		//imshow("Display video",frame); waitKey(0);
		ovideo.write(frame);
		frame_number++;
	
}

cv::Mat VideoManager::GetFrame(int fnumber)
{	

	if((fnumber >= 0) & (fnumber <= bufVideoInfo.frames-1)){
		frame_number=fnumber;
		ivideo.set(CV_CAP_PROP_POS_FRAMES, frame_number);	
		Mat frame;
		ivideo >> frame;
		return frame;
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
			return frame;
		}
		
		else{
			std::cout << "Frame out of boundary." << endl;
			Mat empty;
			return empty;
		}
}

void VideoManager::SetFrameNumber(int fnumber)
{
	//cout << "Setting frame number to " << fnumber << endl;
	frame_number = fnumber;
	ivideo.set(CV_CAP_PROP_POS_FRAMES, fnumber);	
}

void VideoManager::ReloadFrameFromVideo(int fnumber)
{
	
	ivideo.set(CV_CAP_PROP_POS_FRAMES, fnumber);	

}


VideoManager::~VideoManager()
{
	ivideo.release();
	ovideo.release();
	
}
