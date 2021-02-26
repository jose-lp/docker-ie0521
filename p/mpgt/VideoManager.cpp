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
		std::runtime_error ( "Failed to open input file" );

		exit(1);
	}

	//Fill input video file information on struct MediaInfo
	bufVideoInfo.fps = ivideo.get(CV_CAP_PROP_FPS);
	bufVideoInfo.frames =  ivideo.get(CV_CAP_PROP_FRAME_COUNT);
	bufVideoInfo.width = ivideo.get(CV_CAP_PROP_FRAME_WIDTH);
	bufVideoInfo.height = ivideo.get(CV_CAP_PROP_FRAME_HEIGHT);

}


void VideoManager::StartVideoSaver(string output_video)
{
		
		ovideo.open(output_video, CV_FOURCC('X','V','I','D'), 30, Size(bufVideoInfo.width,bufVideoInfo.height), true);
		if (!ovideo.isOpened()) {
			//throw;
			cout << "Output video file not found." << endl;
			std::runtime_error ( "Failed to open output file" );

			exit(1);
		}
	
}

void VideoManager::SimpleFrameSaver(Mat frame)
{
		
		if (!ovideo.isOpened()) {
			//throw;
			cout << "Output video file not found." << endl;
			std::runtime_error ( "Failed to open output file" );

			exit(1);
		}
		
		ovideo << frame;
	
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

void VideoManager::SetFrame(int fnumber)
{
	frame_number = fnumber;
}

void VideoManager::ReloadFrameFromVideo(int fnumber)
{
	
	ivideo.set(CV_CAP_PROP_POS_FRAMES, fnumber);	
	Mat frame;
	ivideo >> frame;
	mat_deque[fnumber]=frame;

}

VideoManager::~VideoManager()
{
	ivideo.release();
	mat_deque.shrink_to_fit();
}
