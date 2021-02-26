/**
 * This is a sequential version of the
 * Multipartite Graph Tracker MPGT
 **/

//includes
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <queue>
#include <opencv2/opencv.hpp>
#include <getopt.h>
#include "PrisSerial.h"
#include "Defs.h"
#include "VideoManager.h"
#include "Structs.h"
#include "PlayerBlob.h"
#include "Segmentator.h"
#include "MultiPartiteGraph.h"
#include "YamlParser.h"
#include "Timer.h"
#include "stdafx.h"
#include "dataanalysis.h"
#include "Dominant.h"

//namespaces
using namespace std;
using namespace cv;
using namespace alglib;

//global vars
std::string input_file = "input_file.dv";
std::string output_file = "video_file.avi";
std::string track_file = "tracking_file.yaml";
std::string config_file = "config.yaml";
std::string init_file = "init.yaml";
vector< cv::Point > centers;
vector< TrackPoint > init_tpoints;
bool create_video = false;
bool enable_tracking = false;
bool draw_centers = false;
bool draw_ids = false;
bool load_init_data = false;
double init_prom_area = 0;
double init_prom_ratio = 0;

//function prototypes
void PrintHelp(void);
void ProcessArgs(int argc, char** argv);

//functions
int main(int argc, char *argv[])
{	

	/// Init program: read args & video properties
	ProcessArgs(argc, argv);
	cout << "Loading video file ..." << endl;
	
	VideoManager fh; //Video player object-file handler
	fh.SimpleVideoLoader(input_file);
	int isize = fh.bufVideoInfo.frames;
	
	double dWidth = fh.bufVideoInfo.width; //get the width of frames of the video
	double dHeight = fh.bufVideoInfo.height; //get the height of frames of the video
	double fps = fh.bufVideoInfo.fps; //get the frames per seconds of the video
	double tframes = fh.bufVideoInfo.frames; //get the total frames 

	cout << "Frame per seconds : " << fps << endl;
	cout << "Total frames in video : " << tframes << endl;
	cout << "Frame Size = " << dWidth << "x" << dHeight << endl;	

	cv::Mat iframe, oframe;

	cout << "Starting segmentation and tracking..." << endl;
	Timer tm,seg_cronometer,tracking_cronometer,writing_cronometer;
	
	tm.Start("MPGT segmentation and tracking");
	seg_cronometer.CrnStart("Segmentation");
	tracking_cronometer.CrnStart("Tracking");
	if(create_video)
		writing_cronometer.CrnStart("Wrinting");
	seg_cronometer.CrnPause();
	tracking_cronometer.CrnPause();
	if(create_video)
		writing_cronometer.CrnPause();
	
	YamlParser yp;
	yp.LoadConfig(config_file);
	if(yp.graph_window<2){
		cout << "Graph window must be greater than 1." << endl;
		return 1;
	}
	yp.segparms["video_height"]=dHeight;
	yp.segparms["video_width"]=dWidth;
	yp.InitResultWriter(track_file);
	yp.StoreHeaderInfo(input_file, fh.bufVideoInfo);
	if(load_init_data)
		init_tpoints = yp.LoadTrackingFile(init_file, "non");
	
	///Segmentator object and operation mode;
	Segmentator sg(yp.segparms);
	
	if(create_video)
		fh.StartVideoSaver(output_file);
	///Run on each frame on a batch mode equal to the graph window until stop frame % window size to avoid seg faults
	Graph bpg_previos_graph;
	OclusionMemoir OMemory; ///Oclusion event/node memory
	AbductionMemoir AMemory; ///Abduction event/node memory
	OMemory.InitMemoir(yp.mpgtparms,yp.weights);
	AMemory.InitMemoir(yp.mpgtparms,yp.weights);
	int stop_frame_wsize = (int)yp.stop_frame - ((int)yp.stop_frame % yp.graph_window);
	for(int step=yp.start_frame;step<isize && step<= stop_frame_wsize;step=step+yp.graph_window){

		MultiPartiteGraph mpgo(yp.graph_window);
		mpgo.initial_frame=step; 
		mpgo.last_frame=step+yp.graph_window-1;
		cout << "Working on block from " << mpgo.initial_frame << " to " << mpgo.last_frame << endl;
		cout << "Performing segmentation ..." << endl;

		///**************First step: Segmentation**************
		for(int i=0; i<yp.graph_window;i++) {
		
			seg_cronometer.CrnResume();
			int frame_number = i+step;
			D(cout << "Extracting blobs from frame " << frame_number << endl;)
			iframe = fh.GetFrame(frame_number);
			if(iframe.empty())
				break;

			//cv::normalize( iframe, iframe, 0, 1, cv::NORM_MINMAX, CV_32FC3, cv::Mat() );		
			vector< vector< cv::Point > > contornos_jugadores = sg.Segment(iframe);
		
			///Adquirir los descriptores para cada blob y guardarlos en los nodos correspondientes
			for(unsigned int j=0;j<contornos_jugadores.size();j++){
				double max_area = 0.0, min_area = 9999999.9;
				PlayerBlob blob;
				blob.FillModel(frame_number,j,contornos_jugadores[j],iframe);
				if(load_init_data)
					blob.AssignInitTag(init_tpoints,yp.init_tkp_threshold);
				
				///Quitar los blobs que estan fuera de los limites superiores e inferiores
				if(yp.segparms["enable_boundary_blob_remover"]){		
					double high_limit = (double)dHeight*(double)yp.segparms["lower_boundary"]/100.0;
					double low_limit = (double)dHeight*(double)yp.segparms["upper_boundary"]/100.0;
					//cout << "Blob with id " << blob.blob_id << " on frame " << frame_number << " at [ " <<  blob.centroid.x << " , " << blob.centroid.y 
					// << " ] inspected" << " lower limit = " <<low_limit << " higher limit " << high_limit << endl;			
					if((blob.centroid.y <= low_limit) &&
					   (blob.centroid.y >= high_limit) ){						
						mpgo.InsertNode(frame_number,blob);
					}					
				}

				else{	
					mpgo.InsertNode(frame_number,blob);
					mpgo.min_area = (min_area < blob.area) ? min_area : blob.area;
					mpgo.max_area = (max_area > blob.area) ? max_area : blob.area;
					mpgo.prom_area += blob.area; mpgo.prom_ratio += blob.ratio;	
				}
				//cout << "Mostrando info de blob " << blob.blob_id << " en el frame " << blob.blob_frame_number << endl;
				//imshow("Contorno de jugador",blob.contourRegion);
			}
		     seg_cronometer.CrnPause();  
		}//for loop within a multipartite graph
		
		///Calculate the average area and ratio for the clasification of blobs
		//mpgo.prom_area /= mpgo.total_blobs; mpgo.prom_ratio /= mpgo.total_blobs;
		if(step <= yp.start_frame + yp.graph_window){
			init_prom_area = mpgo.prom_area/mpgo.total_blobs;
			init_prom_ratio = mpgo.prom_ratio/mpgo.total_blobs;
		}
		
			mpgo.prom_area = init_prom_area;
			mpgo.prom_ratio = init_prom_ratio;
		
		
		
		///**************Second step a: MPGT node clustering **************
		if(yp.mpgtparms["enable_clustering"]){
			
			cout << "Performing clustering ..." << endl;
			///Sweep the MPGT to extract the most predominant colors of each blob
			///to be use as features for the clustering
			real_2d_array xy; 
			std::vector<double> cluster_features;
			int how_many_colors = yp.mpgtparms["how_many_colors"];
				
			for(int gi=0; gi<yp.graph_window;gi++) { 
				int igraph = gi+step;
				Graph actual_graph = mpgo.GetGraph(igraph);
					
				for(unsigned int ni=0;ni<actual_graph.size();ni++){ //ni node id
					Node blobnode = mpgo.GetNode(igraph,ni);
					std::vector<double> node_pcolors;
					if(yp.mpgtparms["hue_clustering_only"]){
						//node_pcolors = GetProminentHueRangeColor(blobnode.contourRegion, how_many_colors,yp.mpgtparms["kmean_colors_cuantization"]);
						//node_pcolors = find_dominant_hcolors2(blobnode.contourRegion, how_many_colors);
						node_pcolors = find_dominant_hcolors3(blobnode.contourRegion, how_many_colors);
					}
					else
						node_pcolors = GetProminentHueSaturationRangeColor(blobnode.contourRegion, how_many_colors);
						
					cluster_features.insert(cluster_features.end(),node_pcolors.begin(),node_pcolors.end());
					if(yp.mpgtparms["area_as_cluster_feature"])
						//cluster_features.insert(cluster_features.end(),(double)(128.0 * blobnode.area/(mpgo.max_area - mpgo.min_area)));
						cluster_features.insert(cluster_features.end(),(double)(blobnode.area));
/*
					imshow("blob", blobnode.contourRegion); waitKey(0);
					Mat dcolor(blobnode.contourRegion.rows,blobnode.contourRegion.cols,CV_8UC3,cv::Scalar(node_pcolors[0],node_pcolors[1],100));
					std::string name = "blob"; imshowHSV(name, dcolor); waitKey(0);
*/					
/*					
					cout << "Dominant colors for blob " << blobnode.blob_id << endl;	
					for(unsigned int ncolors=0;ncolors<node_pcolors.size();ncolors++)
						cout << node_pcolors[ncolors] << " ";
					cout << endl;
*/					
				}
			}
			int irows,icols;
			
			if(yp.mpgtparms["hue_clustering_only"]){
				irows = cluster_features.size()/how_many_colors;
				if(yp.mpgtparms["area_as_cluster_feature"])
					irows += 1;
				icols = how_many_colors;
			}
			else{
				irows = cluster_features.size()/(how_many_colors*2);
				if(yp.mpgtparms["area_as_cluster_feature"])
					irows += 1;
				icols = how_many_colors*2;				
			}
			xy.setcontent(irows,icols, cluster_features.data());
			//xy = "[[1,1],[1,2],[4,1],[2,3],[4,1.5]]";
/*			
			cout << "Cluster dataset of " << xy.rows() << " elements and " << xy.cols() << " features" << endl;
			for(int i=0;i<xy.rows();i++) {
				for(int j=0;j<xy.cols();j++) cout << xy[i][j] << "  ";
					cout << endl;
			}
*/			
			if(yp.mpgtparms["clustering_mode"] == KMEANS || 
			   yp.mpgtparms["clustering_mode"] == KMEANSPP){
				cout << "CLUSTERING MODE " << yp.mpgtparms["clustering_mode"] << endl;
				clusterizerstate s;
				kmeansreport krep;
				

				clusterizercreate(s);
				clusterizersetpoints(s, xy, EUCLIDEAN_D); ///state, data, distance
				clusterizersetkmeanslimits(s, 5, 0); ///state,restarts, max iterations(0 unlimited iterations)
				
				if(yp.mpgtparms["clustering_mode"] == KMEANS)
					clusterizersetkmeansinit(s,KM);
				else if(yp.mpgtparms["clustering_mode"] == KMEANSPP)
					clusterizersetkmeansinit(s,KMPP);
				
				clusterizerrunkmeans(s, yp.mpgtparms["n_clusters"], krep); ///state, clusters, output report
				
				real_2d_array  c = krep.c; //array[K,NFeatures], rows of the array store centers
				integer_1d_array cidx = krep.cidx; //array[NPoints], which contains cluster indexes								

				//cout << "CLUSTERS assigment: " << cidx.tostring().c_str() << " with " << krep.npoints << " and " << krep.nfeatures << " features" << endl;
				//cout << "Number of clusters: " << krep.k.tostring().c_str() << " with " << krep.c << " centers " << endl;y
/*
				for(int i=0;i<c.rows();i++) {
					for(int j=0;j<c.cols();j++) 
						cout << c[i][j] << "  ";
					cout << endl;
				}
*/
				
				///Asignar el cluster a cada nodo
				int graphidx = 0;
				for(int gi=0; gi<yp.graph_window;gi++) { 
					int igraph = gi+step;
					Graph actual_graph = mpgo.GetGraph(igraph);
					int graph_size = actual_graph.size();
					//cout << " Asignando clusters en el grafo " << igraph << " de tamano " << graph_size << endl;
					
					for(int ni=0;ni<graph_size;ni++){
						mpgo.SetNodePlayerCluster(igraph,ni,cidx[graphidx+ni]);
						//cout << "Asignando en el cuadro " << igraph << " al nodo " << ni << " el cluster " << cidx[graphidx+ni] << endl;
					}
					graphidx += graph_size;
				}
				
			}
			else if(yp.mpgtparms["clustering_mode"] == AHCS || 
					yp.mpgtparms["clustering_mode"] == AHCC || 
					yp.mpgtparms["clustering_mode"] == AHCW ){
				cout << "CLUSTERING MODE " << yp.mpgtparms["clustering_mode"] << endl;
				clusterizerstate s;
				ahcreport rep;
				
				//real_2d_array xy = "[[1],[3],[10],[16],[20]]"; //Cambiar esto por los features
				
				integer_1d_array cidx;
				integer_1d_array cz;

				clusterizercreate(s);
				try {
					clusterizersetpoints(s, xy, EUCLIDEAN_D);
				}
				catch(alglib::ap_error e)
				{
					printf("error setpoints msg: %s\n", e.msg.c_str());
				}
				if(yp.mpgtparms["clustering_mode"] == AHCS)
					clusterizersetahcalgo(s, SINGLE_LINK);
				else if(yp.mpgtparms["clustering_mode"] == AHCC)
					clusterizersetahcalgo(s, COMPLETE_LINK);
				else if(yp.mpgtparms["clustering_mode"] == AHCW)
					clusterizersetahcalgo(s, WARD);										
				
				try {
					clusterizerrunahc(s, rep);
				}
				catch(alglib::ap_error e)
				{
					printf("error runhc msg: %s\n", e.msg.c_str());
				}	
				//clusterizergetkclusters(rep, yp.mpgtparms["n_clusters"], cidx, cz); //En cidx esta el cluster asignado
				try {
					clusterizergetkclusters(rep, yp.mpgtparms["n_clusters"], cidx, cz); //En cidx esta el cluster asignado
				}
				catch(alglib::ap_error e)
				{
					printf("error getkclusters msg: %s\n", e.msg.c_str());
				}
//				printf("%s\n", cidx.tostring().c_str());
				
				///Asignar el cluster a cada nodo
				int graphidx = 0;
				for(int gi=0; gi<yp.graph_window;gi++) { 
					int igraph = gi+step;
					Graph actual_graph = mpgo.GetGraph(igraph);
					int graph_size = actual_graph.size();
					
					for(int ni=0;ni<graph_size;ni++){
						mpgo.SetNodePlayerCluster(igraph,ni,cidx[graphidx+ni]);
						//cout << "Asignando en el cuadro " << igraph << " al nodo " << ni << " el cluster " << cidx[graphidx+ni] << endl;
					}
					graphidx += graph_size;
				}
			
			}

		}
			
		///**************Second step b: Tracking**************
		if(enable_tracking){
			cout << "Tracking players ..." << endl;
			tracking_cronometer.CrnResume();  
			if(step != yp.start_frame){///To exclude first frame of sequence
				mpgo.SetOclusionMemories(OMemory.vector_memoir);
				mpgo.SetAbductionMemories(AMemory.vector_memoir);
				mpgo.SolveExtendedBorderBPG(yp.weights,yp.mpgtparms,step,bpg_previos_graph);//Con esto se propaga los ids anteriores, es el ultimo grafo del mpg anterior
				bpg_previos_graph.clear();			
				OMemory.vector_memoir.clear();
				AMemory.vector_memoir.clear();
			}
			//if(not yp.segparms["enable_extended_finder"])
			mpgo.MVMPAssignFromFirstGraph(yp.weights,step);
			mpgo.MVExtendedSimpleSolver(yp.weights,yp.mpgtparms,step);	///Tracking function	
		
			int last_frame_number_from_window = step+yp.graph_window-1;
			//cout << "bpg first graph = " << last_frame_number_from_window << endl;
			bpg_previos_graph = mpgo.GetGraph(last_frame_number_from_window);
			OMemory.vector_memoir = mpgo.GetOclusionMemories();
			AMemory.vector_memoir = mpgo.GetAbductionMemories();
			tracking_cronometer.CrnPause();
		
			for(int gi=0; gi<yp.graph_window;gi++) { 
				int frame_number = gi+step;
				yp.StoreFrameResult(mpgo.GetGraph(frame_number));
			}
		}
	
		///**************Third step: Draw results**************
		if(create_video){
		writing_cronometer.CrnResume();  
		
		///Dibujar los contornos  y los ids para cada frame del multigrafo		
		cout << "Writing video for the current block ..." << endl;
		for(int gi=0; gi<yp.graph_window;gi++) { 
		
			int frame_number = gi+step;
			D(cout << "Drawing results for frame " << frame_number << endl;)
			iframe = fh.GetFrame(frame_number);
			if(iframe.empty())
				break;
			putText(iframe, std::to_string(frame_number), Point(0,dHeight), FONT_HERSHEY_PLAIN, (double)yp.drawparms["font_scale"], CV_RGB(255,0,0), 2.0);			

			vector< vector< cv::Point > > contornos_jugadores;
			vector< Node > vectornode;
			
			Graph solved_graph = mpgo.GetGraph(frame_number);

			///Extract the contours from the graph nodes
			for(unsigned int ni=0;ni<solved_graph.size();ni++){ //ni node id
				Node blobnode = mpgo.GetNode(frame_number,ni);
				if(yp.drawparms["enable_boundary_blob_remover"]){
					double high_limit = (double)dHeight*(double)yp.drawparms["lower_boundary"]/100.0;
					double low_limit = (double)dHeight*(double)yp.drawparms["upper_boundary"]/100.0;
					
					if((blobnode.centroid.y <= low_limit) &&
					   (blobnode.centroid.y >= high_limit) ){						
							contornos_jugadores.push_back(blobnode.contorno);
							vectornode.push_back(blobnode);										   
					}
				}
				else{
					contornos_jugadores.push_back(blobnode.contorno);
					vectornode.push_back(blobnode);				
				}
				if((draw_centers==true) || (yp.drawparms["draw_trayectories"]))
					centers.push_back(blobnode.centroid);
			}//for loop to sweep all nodes within a graph
						
			cv::Mat contornos_graf = DrawContour( contornos_jugadores, iframe.rows, iframe.cols );						
			contornos_graf = ConvertColor( contornos_graf, PRIS_GRY2RGB );				
			//imshow("Contornos y centros", contornos_graf); waitKey(0);			
			cv::normalize( iframe, iframe, 0, 1, cv::NORM_MINMAX, CV_32FC3, cv::Mat() );			
			cv::Mat oframe;
			if(yp.drawparms["draw_contours"]){
				oframe = Add( iframe, contornos_graf );
				oframe = Normalize( oframe, PRIS_8UC1 );
				if(oframe.empty()){
					cout << "Frame de salida vacio" << endl;
					break;
				}
				//imshow("Contornos y centros en entrada", oframe); waitKey(0);			
			}			
			else{
				oframe = iframe.clone();
				oframe = Normalize( oframe, PRIS_8UC1 );
			}
			if(yp.drawparms["draw_bounding_box"]){
				vector<cv::Rect> boundRect( contornos_jugadores.size() );
				for( unsigned int i = 0; i < contornos_jugadores.size(); i++ ){
					boundRect[i] = boundingRect( Mat(contornos_jugadores[i]) );
					rectangle( oframe, boundRect[i], Scalar(200,200,200), yp.drawparms["bounding_box_thickness"], 8, 0 );
				}
			}			

			if((draw_centers==true) || (yp.drawparms["draw_trayectories"])){
				D(cout << "Drawing trayectories..." << endl;)
				for(unsigned int j=0;j<centers.size();j++){
					circle( oframe, centers[j], yp.drawparms["trayectories_radius"], Scalar(255,255,255), -1, 8, 0 );					
				}
			}
				
			///Dibujar los blob ids/labels
			if(draw_ids==true || yp.drawparms["draw_ids"]==true){
				for(unsigned int vn = 0; vn <	vectornode.size(); vn++){
					if(vectornode[vn].merged_centroids.size() <= 1){ ///Si es un blob de un jugador
						std::string sid;
						sid = std::to_string(vectornode[vn].player_tag);
					
						if(yp.drawparms["draw_blob_ids_also"])
							sid = sid + "/" + std::to_string(vectornode[vn].blob_id);
						if(yp.drawparms["draw_cluster_ids_also"])
							sid = sid + "[" + std::to_string(vectornode[vn].cluster) + "]";
						
						putText(oframe, sid, vectornode[vn].centroid, FONT_HERSHEY_PLAIN, (double)yp.drawparms["font_scale"], CV_RGB(0,255,0), 2.0);
					}
					else{///Esto es para dibujar los centroides y tags de los jugadores dentro de un merged blob
						for (map< int,Point2f >::iterator mtcit=vectornode[vn].merged_centroids.begin(); 
							 mtcit!=vectornode[vn].merged_centroids.end(); ++mtcit){
						
							std::string single_sid;
							single_sid = std::to_string(mtcit->first);
						
							putText(oframe, single_sid, mtcit->second, FONT_HERSHEY_PLAIN, (double)yp.drawparms["font_scale"], CV_RGB(0,255,0), 2.0);
						}
						
						std::string sid;
						
						if(yp.drawparms["draw_blob_ids_also"])
							sid = std::to_string(vectornode[vn].blob_id);
						if(yp.drawparms["draw_cluster_ids_also"])
							sid = sid + "[" + std::to_string(vectornode[vn].cluster) + "]";
						
						putText(oframe, sid, vectornode[vn].centroid, FONT_HERSHEY_PLAIN, (double)yp.drawparms["font_scale"], CV_RGB(0,255,0), 2.0);

					}
				}
			}

			//imshow("Video de salida",oframe); waitKey();
			if(create_video){
				
				if(yp.drawparms["enable_color_balance"]){
					Mat cb_oframe;
					SimplestCB(oframe, cb_oframe,yp.drawparms["color_balance_percent"]);
					fh.SimpleFrameSaver(cb_oframe);			
				}
				else
					fh.SimpleFrameSaver(oframe);			
			}
			
		}
		writing_cronometer.CrnPause();  
		
	  }//for loop to sweep all frames in a batch mode
	}	
	tm.Stop();
	seg_cronometer.CrnStop();
	tracking_cronometer.CrnStop();
	if(create_video)
		writing_cronometer.CrnStop();
	cout << "FPS = " << (yp.stop_frame-yp.start_frame)/tm.accum << endl;

	yp.CloseResultWriter();
	cout << "Finish..."<<endl;

	return 0;

}

///************************Funcion que imprime ayuda
void PrintHelp(void)
{
    std::cout <<
			"\n Arguments: \n"
            "-i <ifile>:     Input video file\n"
            "-o <ofile>:    Output video file\n"
            "-t <tfile>:     Track file to store tracking information\n"
			"-c <cfile>:    Configuration file\n"
			"-l <lfile>:    Initialization data file\n"			
            "-C:           Draw blob trayectories \n"  
            "-I:               Draw blob ids \n"             
            "--help:              Show help\n";
                
    exit(1);
}

///************************Funcion que procesa argumentos
void ProcessArgs(int argc, char** argv)
{
	
	//if (argc < 4) 
    //    PrintHelp();        
	
    const char* const short_opts = "i:o:t:c:l:CIh";
    const option long_opts[] = {
            {"input", required_argument, nullptr, 'i'},
            {"output", optional_argument, nullptr, 'o'},
            {"track", optional_argument, nullptr, 't'},
            {"config", optional_argument, nullptr, 'c'},
            {"load", optional_argument, nullptr, 'l'},
		    {"centers", no_argument, nullptr, 'C'},           
		    {"ids", no_argument, nullptr, 'I'},
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
            std::cout << "Input video file is: " << input_file << std::endl;
            break;


        case 'o':
            output_file = std::string(optarg);
            std::cout << "Output video file set to: " << output_file << std::endl;
            create_video=true;
            break;

        case 't':
            track_file = std::string(optarg);
            std::cout << "Tracking file set to: " << track_file << std::endl;
            enable_tracking=true;
            break;

        case 'c':
            config_file = std::string(optarg);
            std::cout << "Configuration file set to: " << config_file << std::endl;
            break;
      
        case 'l':
            init_file = std::string(optarg);
            std::cout << "Initialization file set to: " << init_file << std::endl;
            load_init_data = true;
            break;

        case 'C':
            draw_centers = true;
            std::cout << "Drawing segmented trayectories "  << std::endl;
            break;

        case 'I':
            draw_ids = true;
            std::cout << "Drawing blob ids "  << std::endl;
            break;

        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            PrintHelp();
            break;
        }

    }

}
