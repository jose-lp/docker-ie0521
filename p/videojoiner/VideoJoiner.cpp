/**
 *  Herramienta para unir videos correspondientes a camaras que captan
 *  un campo de futbol generando un video en modo panoramico
 * 
 **/

//includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <queue>
#include <opencv2/opencv.hpp>
#include <getopt.h>
#include "Defs.h"
#include "VideoManager.h"
#include "Structs.h"
#include "YamlParser.h"
#include "Joiner.h"

//namespaces
using namespace std;
using namespace cv;

//global vars
std::string input_left_file = "input_left_video.avi";
std::string input_right_file = "input_right_video.avi";
std::string output_file = "output.avi";
std::string config_file = "config.yaml";
bool save_ovideo = false;
int ilabel = 0;
Point mpt, spt;
std::string corners[4]= {"top left","top right","bottom left","bottom right"};
YamlParser yp;
VideoManager vlh, vrh, vout; //Video player object-file handler

//function prototypes
void PrintHelp(void);
void ProcessArgs(int argc, char** argv);
void mouseEventSource(int event, int x, int y, int flags, void* param);
int SelectFullVideoCorners(Mat input);
int SelectSingleVideoCorners(void);
void CreateRectFromPoints(void);

//functions
///************************Funcion principal
int main(int argc, char *argv[])
{

	/// Init program: read args & video properties
	ProcessArgs(argc, argv);
    string input_left_video = input_left_file;
    string input_right_video = input_right_file;

	vlh.SimpleVideoLoader(input_left_video);
	vrh.SimpleVideoLoader(input_right_video);

	yp.LoadJoinerConfig(config_file); //cout << "Safe key en main: " << yp.joiner_parms["safe_key"] << endl;
	if(save_ovideo){
		if(yp.joiner_parms["enable_output_video_crop"])
			vout.StartVideoSaver(output_file,
				(yp.joiner_parms["video_top_right_x"]-yp.joiner_parms["video_top_left_x"]),
				(yp.joiner_parms["video_bottom_right_y"]-yp.joiner_parms["video_top_right_y"]));		
		else
			vout.StartVideoSaver(output_file,
				(vlh.bufVideoInfo.width+vrh.bufVideoInfo.width),
				vlh.bufVideoInfo.height);
		   
	}

	double ldWidth = vlh.bufVideoInfo.width; //get the width of frames of the video
	double ldHeight = vlh.bufVideoInfo.height; //get the height of frames of the video
	double lfps = vlh.bufVideoInfo.fps; //get the frames per seconds of the video
	double ltframes = vlh.bufVideoInfo.frames; //get the total frames 

	double rdWidth = vrh.bufVideoInfo.width; //get the width of frames of the video
	double rdHeight = vrh.bufVideoInfo.height; //get the height of frames of the video
	double rfps = vrh.bufVideoInfo.fps; //get the frames per seconds of the video
	double rtframes = vrh.bufVideoInfo.frames; //get the total frames 


	cout << "Frame per seconds for left video : " << lfps << endl;
	cout << "Total frames in left video : " << ltframes << endl;
	cout << "Left frame size = " << ldWidth << "x" << ldHeight << endl;

	cout << "Frame per seconds for right video : " << rfps << endl;
	cout << "Total frames in right video : " << rtframes << endl;
	cout << "Right frame size = " << rdWidth << "x" << rdHeight << endl;
   
	if(lfps != rfps){
		cout << "Videos FPS are different, unable to join them" << endl;
		return 1;
	}
 
	Joiner jnr(yp.joiner_parms,vlh.bufVideoInfo);
	std::string::size_type sz;     // alias of size_t
	jnr.SetJoiner(yp.joiner_parms["enable_frame_drop"],std::stof(yp.joiner_sparms["frame_rate"],&sz));

	///Single video point selection mode
	if(yp.joiner_parms["point_selection_mode"] == SINGLE_VIDEO_PMODE)
		return SelectSingleVideoCorners();

	else{
		
		///Primero sincronizar los videos
		if(yp.joiner_parms["sync_mode"]==TIMECODE_MODE){
			cout << "Using timecodes for syncronization" << endl;
			//jnr.synch(vlh.ivideo,ltc,vrh.ivideo,rtc);
			jnr.synch(&vlh,&vrh,&yp);
			
		}
		
		else if(yp.joiner_parms["sync_mode"]==MANUAL_MODE){
			cout << "Using manual frame numbers for syncronization" << endl;
			jnr.synch_manual(vlh.ivideo, yp.joiner_parms["left_start_frame"], vrh.ivideo, yp.joiner_parms["right_start_frame"]);
		}

		else if(yp.joiner_parms["sync_mode"]==GRAPHIC_MODE){
			cout << "Using graphic selection for syncronization" << endl;
			jnr.synch_graph2(vlh.ivideo, vrh.ivideo, yp.joiner_parms["start_frame"]);
		}
		else {
			cout << "No syncronization mode selected" << endl;
		}		

	}
	
	///Ahora ir pegando frame por frame segun la cantidad establecida en la config
	for(int fn = yp.joiner_parms["start_frame"]; fn <= yp.joiner_parms["stop_frame"]; fn++){
		
		///Primero la correccion de color (Este es con el metodo de Marcos)
		if(yp.joiner_parms["enable_color_correction"]){
	
			/// Extraer una pareja de cuadros de los videos
			Mat lframe, rframe;
			vlh.ivideo >> lframe;
			vrh.ivideo >> rframe;
			if(lframe.empty() || rframe.empty()){
				cout << "End of video file or unable to read frame from video, this is the end..." << endl;
				return 0;
			}
				

			/// Asigna el video de referencia para la corrección de color
			Mat *ref_frame, *target_frame;
			//std::string ccvr = yp.joiner_sparms["color_correction_video_reference"];
			if (yp.joiner_sparms["color_correction_video_reference"].compare("left")) {
				ref_frame = &lframe;
				target_frame = &rframe;
			} else {
				ref_frame = &rframe;
				target_frame = &lframe;
			}

			/// Aplica la corrección de color
			jnr.color_match(ref_frame, target_frame);			
		}
		
		if(yp.joiner_parms["stitch_mode"] == STITCH_MJ || 
		   yp.joiner_parms["stitch_mode"] == STITCH_FULL || 
		   yp.joiner_parms["stitch_mode"] == STITCH_SURF1 ||
		   yp.joiner_parms["stitch_mode"] == STITCH_SURF2 ||
		   yp.joiner_parms["stitch_mode"] == STITCH_GEORS ||
		   yp.joiner_parms["stitch_mode"] == STITCH_GEOP ||
		   yp.joiner_parms["stitch_mode"] == STITCH_GEO){
			
			int lfn = jnr.config["left_video_start_frame"]+fn;
			int rfn = jnr.config["right_video_start_frame"]+fn;
			
			vlh.ReloadFrameFromVideo(lfn);
			vrh.ReloadFrameFromVideo(rfn);
			
			if(yp.joiner_parms["sync_mode"]!=TIMECODE_MODE){
				vlh.SetFrameNumber(lfn);
				vrh.SetFrameNumber(rfn);
			}
			
			Mat lframe, rframe, pano;
			namedWindow( "Display video", CV_WINDOW_NORMAL | CV_GUI_NORMAL); ///OJO!! Esto se debe comentar para que corra en Tara
			lframe = vlh.GetFrame(lfn); //imshow("Display video", lframe); waitKey(0);
			rframe = vrh.GetFrame(rfn); //imshow("Display video", rframe); waitKey(0);

			if(yp.joiner_parms["stitch_mode"] == STITCH_FULL){
				cout << "Stitching full mode for frame " << fn << " ..."<< endl;
				jnr.stitchf(lframe,rframe,pano);
			}

			else if(yp.joiner_parms["stitch_mode"] == STITCH_SURF1){
				cout << "Stitching surf mode 1 for frame " << fn << " ..."<< endl;
				jnr.stitchs1(lframe,rframe,pano);
			}

			else if(yp.joiner_parms["stitch_mode"] == STITCH_SURF2){
				cout << "Stitching surf mode 2 for frame " << fn << " ..."<< endl;
				jnr.stitchs2(lframe,rframe,pano);
			}

			else if(yp.joiner_parms["stitch_mode"] == STITCH_GEO){
				cout << "Stitching geometric mode for frame " << fn << " ..."<< endl;
				jnr.stitchsg(lframe,rframe,pano);
			}
			else if(yp.joiner_parms["stitch_mode"] == STITCH_GEORS){
				cout << "Stitching geometric rotation and shear for frame " << fn << " ..."<< endl;
				jnr.stitchsgrs(lframe,rframe,pano);
			}
			else if(yp.joiner_parms["stitch_mode"] == STITCH_GEOP){
				cout << "Stitching geometric perspective mode for frame " << fn << " ..."<< endl;
				//jnr.stitchsp2(lframe,rframe,pano);

				if(fn == yp.joiner_parms["start_frame"]) //Encontrar la matriz de transformacion solo una  vez
					jnr.Find_Transform_Matrixs();
				
				jnr.stitchsp2(lframe,rframe,pano);

			}
			else
				cout << "No stitching..." << endl;

			//jnr.stitch1(lframe,rframe,pano);
			
			///Full video point selection mode
			if(yp.joiner_parms["point_selection_mode"] == FULL_VIDEO_PMODE)
				return SelectFullVideoCorners(pano);
				
			if(yp.joiner_parms["enable_output_video_crop"]){

				Point	top_left_point;
				Point	top_right_point;
				Point	bottom_left_point;
				Point	bottom_right_point;

				///Set ROI points
				top_left_point.x = yp.joiner_parms["video_top_left_x"];
				top_left_point.y = yp.joiner_parms["video_top_left_y"];
				top_right_point.x = yp.joiner_parms["video_top_right_x"];
				top_right_point.y = yp.joiner_parms["video_top_right_y"];
				bottom_left_point.x = yp.joiner_parms["video_bottom_left_x"];
				bottom_left_point.y = yp.joiner_parms["video_bottom_left_y"];
				bottom_right_point.x = yp.joiner_parms["video_bottom_right_x"];
				bottom_right_point.y = yp.joiner_parms["video_bottom_right_y"];

				vector<Point> points;
				points.push_back(top_right_point);
				points.push_back(top_left_point);
				points.push_back(bottom_left_point);
				points.push_back(bottom_right_point);
				
				///Find the corresponding ROI based on the points
				convexHull(points, points);                         //to assure correct point order
				Mat roi(pano.rows, pano.cols, CV_8U, Scalar(0));  //black image
				fillConvexPoly(roi, points, Scalar(255));           //draw ROI in white
				//imshow("Display video",roi); waitKey(0);

				///Filter original image according to ROI
				Mat crop_pano((bottom_left_point.y-top_left_point.y),(top_right_point.y-top_left_point.y),pano.type());   
				pano.copyTo(crop_pano, roi);
				//imshow("Display video",crop_pano); waitKey(0);
				
				Rect rRect(top_left_point, bottom_right_point);
				Mat real_crop_pano = crop_pano(rRect);
				//imshow("Display video",real_crop_pano); waitKey(0);
				
				///Save output
				if(save_ovideo){
					//cout << "Showing crop frame..." << endl;
					//imshow("Display video",crop_pano); waitKey(0);
					vout.SimpleFrameSaver(real_crop_pano);
				}
			}
			
			if(save_ovideo and not yp.joiner_parms["enable_output_video_crop"]){
				//cout << "Show full frame..." << endl;
				//imshow("Display video",pano); waitKey(0);
				vout.SimpleFrameSaver(pano);
			}
		}
		else{
			cout << "Not supported stitch mode" << endl;
		}
		
	}
	cout << "-= Finishing joiner =-" << endl;
}


///************************Funcion que imprime ayuda
void PrintHelp(void)
{
    std::cout <<
			"\n Arguments: \n"
            "-l <lfile>:     Input left video file\n"
            "-r <rfile>:     Input right video file\n"            
            "-c <cfile>:    Configuration file.\n"
            "-o <ofile>:	Output video file\n"
            "--help:              Show help\n";
            

    
    exit(1);
}

///************************Funcion que procesa argumentos
void ProcessArgs(int argc, char** argv)
{
	
	if (argc < 3) 
        PrintHelp();        
	
    const char* const short_opts = "r:l:o:c:h";
    const option long_opts[] = {
		    {"right", required_argument, nullptr, 'r'},
            {"left", required_argument, nullptr, 'l'},
            {"out", required_argument, nullptr, 'o'},
            {"config", required_argument, nullptr, 'c'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, 0}
    };

    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
        {
        case 'r':
            input_right_file = std::string(optarg);
            std::cout << "Input right file is: " << input_right_file << std::endl;
            break;

        case 'l':
            input_left_file = std::string(optarg);
            std::cout << "Input left file is: " << input_left_file << std::endl;
            break;

        case 'o':
            output_file = std::string(optarg);
            std::cout << "Output file set to: " << output_file << std::endl;
            save_ovideo=true;
            break;

        case 'c':
            config_file = std::string(optarg);
            std::cout << "Configuration file set to: " << config_file << std::endl;
            break;


        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            PrintHelp();
            break;
        }

    }

}

///************************Funcion que lee los eventos del mouse
void mouseEventSource(int event, int x, int y, int flags, void* param) 
{                    
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		  mpt.x=x; mpt.y=y;
     }
     else if  ( event == EVENT_RBUTTONDOWN )
     {
          cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
/*
     else if ( event == EVENT_MOUSEMOVE )
     {
          cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

     }		
*/

}

///************************Funcion para seleccionar los puntos para recortar frames de salida
int SelectFullVideoCorners(Mat input)
{
	///Full video point selection mode
	cout << "-== Full video point selection mode ==-" << endl;
	cout << "Select corners on video..." << endl;
		
	/// Create display Window
	namedWindow( "Display video", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	Mat frame;
    
	/// Set the callback function for any mouse event on source frame
	setMouseCallback("Display video", mouseEventSource, &frame);

	/// Read frame of video
	frame=input;

	int corner = TOP_LEFT;
	while( corner < 4 )
	{
		cout << "Select the " << corners[corner] << " corner.." << endl;
		imshow("Display video", frame);
		int key = waitKey(0);

		if(key == yp.joiner_parms["enter_key"] and (mpt.x not_eq 0) and (mpt.y not_eq 0)){
			spt.x=mpt.x; spt.y=mpt.y;
			if(corner==TOP_LEFT){
				cout << "top left captured" << endl;
				yp.joiner_parms["video_top_left_x"]=(int)spt.x;
				yp.joiner_parms["video_top_left_y"]=(int)spt.y;
			}

			else if(corner==TOP_RIGHT){
				cout << "top right captured" << endl;
				yp.joiner_parms["video_top_right_x"]=(int)spt.x;
				yp.joiner_parms["video_top_right_y"]=(int)spt.y;
			}
			else if(corner==BOTTOM_LEFT){
				cout << "bottom left captured" << endl;
				yp.joiner_parms["video_bottom_left_x"]=(int)spt.x;
				yp.joiner_parms["video_bottom_left_y"]=(int)spt.y;
			}				
			else if(corner==BOTTOM_RIGHT){
				cout << "bottom right captured" << endl;
				yp.joiner_parms["video_bottom_right_x"]=(int)spt.x;
				yp.joiner_parms["video_bottom_right_y"]=(int)spt.y;
			}
			else
				cout << "No more corners to select" << endl;

			mpt.x=0.0;mpt.y=0.0;
			corner++;
		}
			
		else if(key == yp.joiner_parms["safe_key"]){	
			cout << "Saving selected points" << endl;
			CreateRectFromPoints();
			yp.StoreJoinerConfig(config_file,yp.joiner_parms);
			return 0;
		}
			
		else if(key == yp.joiner_parms["quit_key"]){
			cout << "Closing program" << endl;
			return 0;
		}

		else{
			//cout << "Umm " << yp.joiner_parms["save_key"] << endl;
			cout << "Key pressed: " << key << endl;	
		}
	}
		
	destroyAllWindows();
	int run = TRUE;
	while (run) {
		string answer = "N";
		cout << "Save selected points as corners for videos? (y/N)" << endl;
		getline(cin, answer);

		if (answer.length() == 1) {
			if(answer[0] == 'n')
				run = FALSE;
			else if(answer[0] == 'y'){
				CreateRectFromPoints();
				yp.StoreJoinerConfig(config_file,yp.joiner_parms);
				run = FALSE;
			}
			else{
				cout << "Invalid character " << answer << ", please try again" << endl;		
				run = TRUE;
			}
		}
	}	
	cout << "No more corners to select, start next time with the selection mode off" << endl;		
	return 0;
}

///************************Funcion para seleccionar los puntos de las esquinas en cada video
int SelectSingleVideoCorners(void)
{
	cout << "-== Single video point selection mode ==-" << endl;
	cout << ".:: Working on LEFT video ::." << endl;
	cout << "Select corners on left video..." << endl;
		
	/// Create display Window
	namedWindow( "Display video", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	Mat frame;
    
	/// Set the callback function for any mouse event on source frame
	setMouseCallback("Display video", mouseEventSource, &frame);

	/// Read frame of left video
	frame=vlh.GetFrame(yp.joiner_parms["start_frame"]);

	int corner = TOP_LEFT;
	while( corner < 4 )
	{
		cout << "Select the " << corners[corner] << " corner.." << endl;
		imshow("Display video", frame);
		int key = waitKey(0);

		if(key == yp.joiner_parms["enter_key"] and (mpt.x not_eq 0) and (mpt.y not_eq 0)){
			spt.x=mpt.x; spt.y=mpt.y;
			if(corner==TOP_LEFT){
				cout << "top left captured" << endl;
				yp.joiner_parms["left_video_top_left_x"]=(int)spt.x;
				yp.joiner_parms["left_video_top_left_y"]=(int)spt.y;
			}

			else if(corner==TOP_RIGHT){
				cout << "top right captured" << endl;
				yp.joiner_parms["left_video_top_right_x"]=(int)spt.x;
				yp.joiner_parms["left_video_top_right_y"]=(int)spt.y;
			}
			else if(corner==BOTTOM_LEFT){
				cout << "bottom left captured" << endl;
				yp.joiner_parms["left_video_bottom_left_x"]=(int)spt.x;
				yp.joiner_parms["left_video_bottom_left_y"]=(int)spt.y;
			}				
			else if(corner==BOTTOM_RIGHT){
				cout << "bottom right captured" << endl;
				yp.joiner_parms["left_video_bottom_right_x"]=(int)spt.x;
				yp.joiner_parms["left_video_bottom_right_y"]=(int)spt.y;
			}
			else
				cout << "No more corners to select" << endl;

			mpt.x=0.0;mpt.y=0.0;
			corner++;
		}
			
		else if(key == yp.joiner_parms["safe_key"]){
			cout << "Saving selected points" << endl;
			yp.StoreJoinerConfig(config_file,yp.joiner_parms);
			return 0;
		}
			
		else if(key == yp.joiner_parms["quit_key"]){
			cout << "Closing program" << endl;
			return 0;
		}

		else{
			//cout << "Umm " << yp.joiner_parms["save_key"] << endl;
			cout << "Key pressed: " << key << endl;	
		}
	}

	cout << ".:: Working on RIGHT video ::." << endl;	
	cout << "Select corners on right video" << endl;
	/// Read frame of right video
	frame=vrh.GetFrame(yp.joiner_parms["start_frame"]);

	corner = TOP_LEFT;
	while( corner < 4 )
	{
		cout << "Select the " << corners[corner] << " corner.. " << endl;
		imshow("Display video", frame);
		int key = waitKey(0);

		if(key == yp.joiner_parms["enter_key"] and (mpt.x not_eq 0) and (mpt.y not_eq 0)){
			spt.x=mpt.x; spt.y=mpt.y;
			if(corner==TOP_LEFT){
				cout << "Top right captured" << endl;
				yp.joiner_parms["right_video_top_left_x"]=(int)spt.x;
				yp.joiner_parms["right_video_top_left_y"]=(int)spt.y;
			}

			else if(corner==TOP_RIGHT){
				cout << "Top right captured" << endl;
				yp.joiner_parms["right_video_top_right_x"]=(int)spt.x;
				yp.joiner_parms["right_video_top_right_y"]=(int)spt.y;
			}
			else if(corner==BOTTOM_LEFT){
				cout << "Bottom left captured" << endl;
				yp.joiner_parms["right_video_bottom_left_x"]=(int)spt.x;
				yp.joiner_parms["right_video_bottom_left_y"]=(int)spt.y;
			}				
			else if(corner==BOTTOM_RIGHT){
				cout << "bottom right captured" << endl;
				yp.joiner_parms["right_video_bottom_right_x"]=(int)spt.x;
				yp.joiner_parms["right_video_bottom_right_y"]=(int)spt.y;
			}
			else
				cout << "No more corners to select" << endl;

			mpt.x=0.0;mpt.y=0.0;
			corner++;
		}
			
		else if(key == yp.joiner_parms["safe_key"]){
			cout << "Saving selected points" << endl;
			yp.StoreJoinerConfig(config_file,yp.joiner_parms);
			return 0;
		}
			
		else if(key == yp.joiner_parms["quit_key"]){
			cout << "Closing program" << endl;
			return 0;
		}

		else
			cout << "Key pressed: " << key << endl;	
			
	}

	destroyAllWindows();
	int run = TRUE;
	while (run) {
		string answer = "N";
		cout << "Save selected points as corners for videos? (y/N)" << endl;
		getline(cin, answer);

		if (answer.length() == 1) {
			if(answer[0] == 'n')
				run = FALSE;
			else if(answer[0] == 'y'){
				yp.StoreJoinerConfig(config_file,yp.joiner_parms);
				run = FALSE;
			}
			else{
				cout << "Invalid character " << answer << ", please try again" << endl;		
				run = TRUE;
			}
		}
		
	}	
	cout << "No more corners to select, start next time with the selection mode off" << endl;
	return 0;			
}

///************************Funcion para crear un rectangulo de los puntos dados
void CreateRectFromPoints(void)
{

	///Crear un rectangulo con las coordenadas mas externas
	int left_boundary, right_boundary, top_boundary, bottom_boundary;
	left_boundary = PMIN(yp.joiner_parms["video_top_left_x"],yp.joiner_parms["video_bottom_left_x"]);
	right_boundary = PMAX(yp.joiner_parms["video_top_right_x"],yp.joiner_parms["video_bottom_right_x"]);
	top_boundary = PMIN(yp.joiner_parms["video_top_left_y"],yp.joiner_parms["video_top_right_y"]);
	bottom_boundary = PMAX(yp.joiner_parms["video_bottom_left_y"],yp.joiner_parms["video_bottom_right_y"]);			
	
	yp.joiner_parms["video_top_left_x"]=left_boundary;
	yp.joiner_parms["video_bottom_left_x"]=left_boundary;	
	yp.joiner_parms["video_top_left_y"]=top_boundary;		
	yp.joiner_parms["video_top_right_x"]=right_boundary;
	yp.joiner_parms["video_top_right_y"]=top_boundary;
	yp.joiner_parms["video_bottom_left_y"]=bottom_boundary;
	yp.joiner_parms["video_bottom_right_x"]=right_boundary;
	yp.joiner_parms["video_bottom_right_y"]=bottom_boundary;
	
}
