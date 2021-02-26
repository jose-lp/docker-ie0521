/**
 *  Herramienta para realizar la anotacion manual de jugadores. Sirve
 *  para el proceso de validacion de algoritmos de rastreo de jugadores
 *  de futbol.
 * 
 **/

//includes
#include <iostream>
#include <iomanip>
#include <ctime>
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
#include <gtk/gtk.h>

//namespaces
using namespace std;
using namespace cv;

//global vars
std::string input_file = "input_file.dv";
std::string output_file = "trackingdata_file.yaml";
std::string config_file = "config.yaml";
std::string resize_output_file = "resize_output.yaml";
bool load = false;
bool ocluido = false;
bool resize_data = false;
int ilabel = 0;
bool enable_gtkw = false;
bool tag_oclusions = false;
int font_size=1;
int new_size = -1;
string addlabel = "0";
Point pt;
TrackPoint trackpt;
//pt.x=0.0;pt.y=0.0;
vector< TrackPoint > centros_escogidos;
Mat frame;
VideoManager fh; //Video player object-file handler
double dHeight;

//function prototypes
void PrintHelp(void);
void ProcessArgs(int argc, char** argv);
void mouseEventSource(int event, int x, int y, int flags, void* param);
void DialogWindow(int *argc,char *argv[]);
void set_label(GtkButton* button, gpointer entry);
void kill_window(GtkButton* button, gpointer window);
std::string Get_Timestamp(void);

//functions
///************************Funcion principal
int main(int argc, char *argv[])
{

	/// Init program: read args & video properties
	ProcessArgs(argc, argv);
    string input_video = input_file;

	//Mat frame;
	//VideoManager fh; //Video player object-file handler
	fh.SimpleVideoLoader(input_video);

   double dWidth = fh.bufVideoInfo.width; //get the width of frames of the video
   dHeight = fh.bufVideoInfo.height; //get the height of frames of the video
   double fps = fh.bufVideoInfo.fps; //get the frames per seconds of the video
   double tframes = fh.bufVideoInfo.frames; //get the total frames 

   cout << "Frame per seconds : " << fps << endl;
   cout << "Total frames in video : " << tframes << endl;
   cout << "Frame Size = " << dWidth << "x" << dHeight << endl;
   
   YamlParser yp;
   yp.LoadAnotatorConfig(config_file);
   enable_gtkw = yp.anotator_parms["enable_gtk_window"];
   tag_oclusions = yp.anotator_parms["tag_oclusions"];
   font_size = yp.anotator_parms["font_size"];


   /// Load previous manual annotation from tracking file
   if(load){
	   cout << "Loading stored data from track file." << endl;
      centros_escogidos = yp.LoadTrackingFile(output_file,input_file);
   }
   
   /// Resize (keeping the aspect ratio) the loaded data
   if(load and resize_data){
	   
	    double ratio = ((float)new_size)/((float)dWidth);
	   
	    yp.InitResultWriter(resize_output_file);
	    MediaInfo new_info = fh.bufVideoInfo;
	    new_info.width = new_size;
	    yp.StoreHeaderAnnotationInfo(input_file,new_info);
	    
	    vector< TrackPoint > resize_centros_escogidos;
	    vector<TrackPoint>::iterator myTrkPtVectorIterator;
	    
		for(myTrkPtVectorIterator = centros_escogidos.begin(); 
			myTrkPtVectorIterator != centros_escogidos.end();
			myTrkPtVectorIterator++){	    
				TrackPoint resize_tkpt;
				resize_tkpt.frame_number = myTrkPtVectorIterator->frame_number;
				resize_tkpt.label = myTrkPtVectorIterator->label;
				resize_tkpt.state = myTrkPtVectorIterator->state;
				resize_tkpt.timestamp = myTrkPtVectorIterator->timestamp;

				///Do the resize
				resize_tkpt.point.x = ratio * (double)myTrkPtVectorIterator->point.x;
				resize_tkpt.point.y = ratio * (double)myTrkPtVectorIterator->point.y;

				resize_centros_escogidos.push_back(resize_tkpt);
		}

		cout << "Saving resized manual tracking to new output file and closing program." << endl;
		yp.WriteVectorPoint(resize_centros_escogidos);
		yp.CloseResultWriter();
		
		return 0;
   }

   yp.InitResultWriter(output_file);
   yp.StoreHeaderAnnotationInfo(input_file,fh.bufVideoInfo);

   int step = yp.anotator_parms["default_step_size"];
   Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

	/// Read first frame of video
    frame=fh.GetFrame(0);
	int current_frame = fh.frame_number;		
	cout << "Current frame : " << current_frame <<  "\r";

	/// Create display Window
	namedWindow( "Display video", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
    
    /// Set the callback function for any mouse event on source frame
    setMouseCallback("Display video", mouseEventSource, &frame);
       
    
    int run=TRUE;
    while(run)
    {
		
		current_frame = fh.frame_number;		
		cout << "Current frame : " << current_frame <<  "\n";

		for (unsigned int ic = 0; ic < centros_escogidos.size(); ic++){
			
			if(centros_escogidos[ic].frame_number==current_frame){
				//int icframe_number = centros_escogidos[ic].frame_number;
				int iclabel = centros_escogidos[ic].label;
				Point icpoint = centros_escogidos[ic].point;
		
				putText(frame, std::to_string(iclabel), icpoint, FONT_HERSHEY_PLAIN, (double)yp.anotator_parms["font_size"], CV_RGB(0,255,0), 2.0);
				circle( frame, icpoint, 3, 255, -1, 8, 0 );				
			}
		}
		putText(frame, std::to_string(current_frame), Point(0,dHeight), FONT_HERSHEY_PLAIN, (double)yp.anotator_parms["font_size"], CV_RGB(255,0,0), 2.0);		
		
        imshow("Display video", frame);
        
		int key = waitKey(0);
		cout << "Key pressed: " << key << endl;
		//TrackPoint trackpt;
		vector<TrackPoint>::iterator myTrkPtVectorIterator;
		


			if(key == yp.anotator_parms["next_key"]){
				if(current_frame+1<=tframes){
					cout << "Going forward one frame. " << endl;
					frame=fh.GetNextFrame();
				}
			}
				
			else if(key == yp.anotator_parms["previous_key"]){
				if(current_frame-1>=0){
					frame=fh.GetFrame(current_frame-1);
					cout << "Going backward one frame. " << endl;
				}
			}

			else if(key == yp.anotator_parms["next_step_key"]){ 
				if(current_frame+step<=tframes){
					frame=fh.GetFrame(current_frame+step);
					cout << "Going  " << step << " frames forward." << endl;
				}
			}

			else if(key == yp.anotator_parms["previous_step_key"]){ 
				if(current_frame-step>=0){
					frame=fh.GetFrame(current_frame-step);
					cout << "Going  " << step << " frames back." << endl;
				}
			}

			else if(key == yp.anotator_parms["increase_step"]){ 
				step += yp.anotator_parms["default_step_size"];
				cout << "Increasing step size to  " << step << endl;
			}

			else if(key == yp.anotator_parms["decrease_step"]){
				if(step>=yp.anotator_parms["default_step_size"]){
					step -= yp.anotator_parms["default_step_size"];
					cout << "Decreasing step size to  " << step << endl;
				}
			}

			else if(key == yp.anotator_parms["increase1sec_step"]){ 
				step += fps;
				cout << "Increasing step size 1 second" << endl;
			}

			else if(key == yp.anotator_parms["decrease1sec_step"]){
				if(step>=fps){
					step -= fps;
					cout << "Decreasing step size 1 second " << endl;
				}
			}

			else if(key == yp.anotator_parms["delete_key"]){
				/// Borrar frame del deque y volver a cargar del video
				cout << "Deleting existing labels on current frame number = " << current_frame << endl;
				fh.ReloadFrameFromVideo(current_frame);
				frame=fh.GetFrame(current_frame);
				
				/// Borrar todos los jugadores anotados en el frame actual/seleccionado
				if(centros_escogidos.size()>0){
					for(myTrkPtVectorIterator = centros_escogidos.begin(); 
						myTrkPtVectorIterator != centros_escogidos.end();
						myTrkPtVectorIterator++){//arreglar seg fault cuando no queda nada que borrar
					
						if(myTrkPtVectorIterator->frame_number==current_frame){
							//centros_escogidos.erase(centros_escogidos.begin() + i );
							cout << "Deleting label " << myTrkPtVectorIterator->label << " in frame " << myTrkPtVectorIterator->frame_number << endl;
							centros_escogidos.erase(myTrkPtVectorIterator--);
						}
					}
/*				
				for(unsigned int i=0;i<centros_escogidos.size();i++){ //iterando con el viejo estilo												
							cout << "Label left: " << centros_escogidos[i].label << " in frame " << centros_escogidos[i].frame_number << endl;
				}
*/				}
			}

			else if(key == yp.anotator_parms["add_key"] and (pt.x not_eq 0) and (pt.y not_eq 0)){				
				if(not yp.anotator_parms["enable_gtk_window"]){
					cout << "Choose a label to add(on this terminal window): " << endl;

					//Label must be a int, check type and cast to string				
					std::cin >> ilabel;

					while(std::cin.fail()) {
						std::cout << "Error, label must be a integer, try again." << std::endl;
						std::cin.clear();
						std::cin.ignore(256,'\n');
						std::cin >> ilabel;
					}
				
					if(yp.anotator_parms["tag_oclusions"]==YES){
						cout << "The player is occluded (y/n): (on this terminal window): " << endl;
						char answer='n';
						while(!(std::cin >> answer) || !std::isalpha(answer)) {
							std::cout << "Error, answer must be y/n, try again." << std::endl;
							std::cin.clear();
							std::cin.ignore(256,'\n');
						}
						if (answer == 'y')
							trackpt.state=1;
						else
							trackpt.state=0;
					}
					else
						trackpt.state=0;
						
					//Change label to string, require to draw it on frame
					addlabel = std::to_string(ilabel);
					putText(frame, addlabel, pt, FONT_HERSHEY_PLAIN, (double)yp.anotator_parms["font_size"], CV_RGB(0,255,0), 2.0);
					circle( frame, pt, 3, 255, -1, 8, 0 );
					trackpt.point=pt; trackpt.frame_number=current_frame; 
					trackpt.label=ilabel;trackpt.timestamp=Get_Timestamp();
					centros_escogidos.push_back(trackpt);
					cout << "Return/switch to image window!" << endl;
					pt.x=0.0;pt.y=0.0;
				}
				else{
					ilabel=0;
					DialogWindow(&argc,argv);
					if (ocluido and (yp.anotator_parms["tag_oclusions"]==YES))
						trackpt.state=1;
					else
						trackpt.state=0;	
					if(ilabel != 0){	
						putText(frame, addlabel, pt, FONT_HERSHEY_PLAIN, (double)yp.anotator_parms["font_size"], CV_RGB(0,255,0), 2.0);
						circle( frame, pt, 3, 255, -1, 8, 0 );
						trackpt.point=pt; trackpt.frame_number=current_frame; 
						trackpt.label=ilabel; trackpt.timestamp=Get_Timestamp();
						centros_escogidos.push_back(trackpt);					
					}
					pt.x=0.0;pt.y=0.0;
				}
			}

			else if(key == yp.anotator_parms["safe_key"]){ 
				cout << "Saving manual tracking to output file and closing program." << endl;
				yp.WriteVectorPoint(centros_escogidos);
				yp.CloseResultWriter();
				run = FALSE;
			}
				
			else if(key == yp.anotator_parms["quit_key"]){
				cout << "Closing program " << endl;
				run = FALSE;
			}
			
			else if(key == yp.anotator_parms["delete_single_key"]){
				
				if(centros_escogidos.size()>0){
					int last_position = centros_escogidos.size()-1;
					if(centros_escogidos[last_position].frame_number == current_frame){
						cout << "Deleting last label if is in the current frame number = " << current_frame << endl;
						centros_escogidos.pop_back();
						frame=fh.GetFrame(current_frame);						
					}
				
				}
			}			

			else if(key == yp.anotator_parms["where_key"]){
				
					
				cout << "Current frame number: " << current_frame << endl;
				
			}

			//else
			//	cout << "Key pressed: " << key << endl;
		        



    }
	
	
	//ivideo.release();
	destroyAllWindows();
	cout << "Exit"<< endl;
	return 0;

}


///************************Funcion que imprime ayuda
void PrintHelp(void)
{
    std::cout <<
			"\n Arguments: \n"
            "-i <ifile>:	Input video file\n"
            "-c <cfile>:	Configuration file.\n"
            "-t <tfile>:	Tracking file to store or load tracking information with YAML format. \n"
            "-l:			Load tracking file data.\n"
            "-r: <size>:	Resize anotations keeping the ratio.\n"
            "-o: <ofile>:	Output file if resize option is selected.\n"
            "--help:              Show help\n";
            
    std::cout <<
			"\n Controls: \n"
            "right arrow to forward 1 frame.\n"
            "left arrow to backward 1 frame.\n"
            "up arrow to forward a step of frames.\n"
            "down arrow to backward a step of frames.\n"
            "+ to increase step size.\n"
            "- to decrease step size.\n"
            "c to increase step size in 1 second.\n"
            "z to decrease step size. in 1 second.\n"
            "d to delete labels on a frame.\n"
            "a to add a label.\n"
            "s to save all trackings to file. \n"            
            "q to quit program without saving data. \n";
    
    exit(1);
}

///************************Funcion que procesa argumentos
void ProcessArgs(int argc, char** argv)
{
	
	if (argc < 3) 
        PrintHelp();        
	
    const char* const short_opts = "li:t:c:r:o:h";
    const option long_opts[] = {
		    {"load", no_argument, nullptr, 'l'},
            {"input", required_argument, nullptr, 'i'},
            {"track", required_argument, nullptr, 't'},
            {"config", optional_argument, nullptr, 'c'},
            {"resize", optional_argument, nullptr, 'r'},
            {"output", optional_argument, nullptr, 'o'},
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
        case 'i':
            input_file = std::string(optarg);
            std::cout << "Input file is: " << input_file << std::endl;
            break;

        case 't':
            output_file = std::string(optarg);
            std::cout << "Tracking file set to: " << output_file << std::endl;
            break;

        case 'c':
            config_file = std::string(optarg);
            std::cout << "Configuration file set to: " << config_file << std::endl;
            break;

        case 'l':
			{
				ifstream ifile(output_file);
				if(ifile){
					std::cout << "Load tracking file data as previous information\n";
					load = true;
				}
				else{
					std::cout << "Error:Cant load the data from the tracking file"<<std::endl;
					std::cout << "Invalid document data or invalid file (must provide the file with tracking option)" << std::endl;
					exit(1);
				}
			}
            break;
        case 'r':
			resize_data = true;
            new_size = std::stoi(optarg);
            std::cout << "Anotation new size set to: " << new_size << std::endl;
            break;
        case 'o':
            resize_output_file = std::string(optarg);
            std::cout << "Resize output file set to: " << resize_output_file << std::endl;
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

	 if ( (flags == SHIFT_MOUSE_PRESS) && (event == EVENT_LBUTTONDOWN) ) ///Si CTRL+LBUTTON retrase un frame
	 {
		  cout << "Left mouse button is clicked while pressing CTRL key - position (" << x << ", " << y << ")" << endl;

		  int current_frame = fh.frame_number;
		  frame=fh.GetFrame(current_frame-1);
		  current_frame = fh.frame_number;
		  cout << "Going backward one frame to " << current_frame << endl;

			for (unsigned int ic = 0; ic < centros_escogidos.size(); ic++){
			
				if(centros_escogidos[ic].frame_number==current_frame){
					//int icframe_number = centros_escogidos[ic].frame_number;
					int iclabel = centros_escogidos[ic].label;
					Point icpoint = centros_escogidos[ic].point;
		
					putText(frame, std::to_string(iclabel), icpoint, FONT_HERSHEY_PLAIN, (double)font_size, CV_RGB(0,255,0), 2.0);
					circle( frame, icpoint, 3, 255, -1, 8, 0 );				
				}
			}

		  putText(frame, std::to_string(current_frame), Point(0,dHeight), FONT_HERSHEY_PLAIN, (double)font_size, CV_RGB(255,0,0), 2.0);		
		  imshow("Display video", frame);	
		   

	 }
                    
     else if  ( (flags != SHIFT_MOUSE_PRESS) && (event == EVENT_LBUTTONDOWN) ) ///Si LBUTTON capture posicion de mouse
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		  pt.x=x; pt.y=y;
          //centros_escogidos.push_back(pt);
     }

	 else if ( (flags == SHIFT_MOUSE_PRESS) && (event == EVENT_RBUTTONDOWN) ) ///Si CTRL+RBUTTON adelante un frame
	 {
		  cout << "Right mouse button is clicked while pressing CTRL key - position (" << x << ", " << y << ")" << endl;

		  frame=fh.GetNextFrame();
		  int current_frame = fh.frame_number;
		  cout << "Going forward one frame to " << current_frame << endl;

			for (unsigned int ic = 0; ic < centros_escogidos.size(); ic++){
			
				if(centros_escogidos[ic].frame_number==current_frame){
					//int icframe_number = centros_escogidos[ic].frame_number;
					int iclabel = centros_escogidos[ic].label;
					Point icpoint = centros_escogidos[ic].point;
		
					putText(frame, std::to_string(iclabel), icpoint, FONT_HERSHEY_PLAIN, (double)font_size, CV_RGB(0,255,0), 2.0);
					circle( frame, icpoint, 3, 255, -1, 8, 0 );				
				}
			}

		  putText(frame, std::to_string(current_frame), Point(0,dHeight), FONT_HERSHEY_PLAIN, (double)font_size, CV_RGB(255,0,0), 2.0);		
		  imshow("Display video", frame);
		  

	 }

     else if  ( (flags != SHIFT_MOUSE_PRESS) && (event == EVENT_RBUTTONDOWN) )
     {
          cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }

     else if  ( event == EVENT_MBUTTONDOWN )
     {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          pt.x=x; pt.y=y; ilabel=0;
          if(enable_gtkw){
			int *argc; char *argv[0];
			DialogWindow(argc,argv);

			if (ocluido and (tag_oclusions==YES))
				trackpt.state=1;
			else
				trackpt.state=0;	
			if(ilabel != 0){
				addlabel = std::to_string(ilabel);	
				putText(frame, addlabel, pt, FONT_HERSHEY_PLAIN, (double)font_size, CV_RGB(0,255,0), 2.0);
				circle( frame, pt, 3, 255, -1, 8, 0 );
				trackpt.point=pt; trackpt.frame_number=fh.frame_number; 
				trackpt.label=ilabel; trackpt.timestamp=Get_Timestamp();
				centros_escogidos.push_back(trackpt);					
			}
			pt.x=0.0;pt.y=0.0;
			
		  int current_frame = fh.frame_number;

			for (unsigned int ic = 0; ic < centros_escogidos.size(); ic++){
			
				if(centros_escogidos[ic].frame_number==current_frame){
					//int icframe_number = centros_escogidos[ic].frame_number;
					int iclabel = centros_escogidos[ic].label;
					Point icpoint = centros_escogidos[ic].point;
		
					putText(frame, std::to_string(iclabel), icpoint, FONT_HERSHEY_PLAIN, (double)font_size, CV_RGB(0,255,0), 2.0);
					circle( frame, icpoint, 3, 255, -1, 8, 0 );				
				}
			}

		  imshow("Display video", frame);			
						
		  }
     }
/*
     else if ( event == EVENT_MOUSEMOVE )
     {
          cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

     }		
*/

	
	//cout << "event = " << event << "with flags = " << flags << endl;


}

///************************Callbacks y funcion para ventana gtk
/* "quit" button "released" callback function. */
void kill_window(GtkButton* button, gpointer window)
{
    /* make sure we realy quit. */
    cout << "Closing popup label window" << endl;
    gtk_widget_destroy(GTK_WIDGET(window));
    gtk_main_quit();
}

void set_label(GtkButton* button, gpointer entry)
{
    const char *clabel = gtk_entry_get_text(GTK_ENTRY(entry));
	ilabel = atoi(clabel);
	addlabel = clabel;
	cout << "Player label = " << ilabel << endl;

}

void toggle_button_callback (GtkWidget *button, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button))) {
        ocluido = true; cout << "Ocluded" << endl;
    }
    else{ 
		ocluido = false; cout << "Not ocluded" << endl;
	}
}

void DialogWindow(int *argc, char *argv[])
{
	//Widgets
	GtkWidget *window;
	GtkWidget *table;
	GtkWidget *label1;
	GtkWidget *check;
	GtkWidget *entry1;
	GtkWidget *okBtn;
		
	gtk_init(argc,&argv);
		
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "Football player labelling");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	table = gtk_table_new(3, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(window), table);

	label1 = gtk_label_new("Number");
	
	//gtk_table_attach(GTK_TABLE(table), label1, 0, 1, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach_defaults(GTK_TABLE(table), label1, 0, 1, 0, 1);

	entry1 = gtk_entry_new();
		
	//gtk_table_attach(GTK_TABLE(table), entry1, 1, 2, 0, 1, GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);	
	gtk_table_attach_defaults(GTK_TABLE(table), entry1, 1, 2, 0, 1);

	check = gtk_check_button_new_with_label ("Ocluido");
	gtk_table_attach_defaults(GTK_TABLE(table), check, 0, 1, 1, 2);
	ocluido = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
	
	okBtn = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okBtn, 70, 30);
	gtk_table_attach_defaults(GTK_TABLE(table), okBtn, 1, 2, 2, 3);

	gtk_widget_show_all(window);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(GTK_OBJECT(okBtn), "clicked",G_CALLBACK (set_label), entry1);
	g_signal_connect(GTK_OBJECT(okBtn), "released",G_CALLBACK (kill_window), window);
	g_signal_connect(GTK_OBJECT(check), "toggled",G_CALLBACK (toggle_button_callback), NULL);
			
	gtk_main();
}

///************************Funcion para generar un timestamp
std::string Get_Timestamp(void)
{
/*	
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time( &tm, "%Y-%m-%d_%H:%M:%S" );
	std::string timestamp( oss.str() );
*/	
	time_t rawtime;
    std::time(&rawtime);
    struct tm *tinfo = std::localtime(&rawtime);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d_%H:%M:%S", tinfo);
    return std::string(buffer);	
	
	//return timestamp;
}
