#include "YamlParser.h"

using namespace std;


YamlParser::YamlParser()
{
	hue_min = 60; 
	hue_max = 180; 
	porcentaje_espurias = 6;
	tamano_kernel = 9;
	umbral_hue = 93;
	graph_window = 2;
	mode = 1;
	write_enable = NO;
	start_frame = 0;
	stop_frame = 324000;
}

void YamlParser::LoadConfig(std::string config_file)
{
	YAML::Node config = YAML::LoadFile(config_file);
	if (config.IsNull()){
		cout << "Error loading config file: " << config_file << endl;
		exit(1);
	}
	segparms["mode"] = config["segmentator"]["mode"].as<int>();
	
	segparms["hue_min"] = config["segmentator"]["pris"]["hue_min"].as<int>();
	segparms["hue_max"] = config["segmentator"]["pris"]["hue_max"].as<int>();
	segparms["porcentaje_espurias"] = config["segmentator"]["pris"]["porcentaje_espurias"].as<int>();
	segparms["area_max_threshold"] = config["segmentator"]["pris"]["area_max_threshold"].as<int>();
	segparms["tamano_kernel"] = config["segmentator"]["pris"]["tamano_kernel"].as<int>();
	segparms["umbral_hue"] = config["segmentator"]["pris"]["umbral_hue"].as<int>();
	segparms["hmorph_axis"] = config["segmentator"]["pris"]["hmorph_axis"].as<int>();
	segparms["vmorph_axis"] = config["segmentator"]["pris"]["vmorph_axis"].as<int>();
	segparms["enable_morph_line_remover"] = config["segmentator"]["pris"]["enable_morph_line_remover"].as<int>();
	segparms["enable_hough_line_remover"] = config["segmentator"]["pris"]["enable_hough_line_remover"].as<int>();
	segparms["line_kernel"] = config["segmentator"]["pris"]["line_kernel"].as<int>();
	segparms["minLineLength"] = config["segmentator"]["pris"]["minLineLength"].as<int>();
	segparms["maxLineGap"] = config["segmentator"]["pris"]["maxLineGap"].as<int>();
	segparms["shape_contour_remover"] = config["segmentator"]["pris"]["shape_contour_remover"].as<int>();	
	segparms["lower_boundary"] = config["segmentator"]["pris"]["lower_boundary"].as<int>();
	segparms["upper_boundary"] = config["segmentator"]["pris"]["upper_boundary"].as<int>();
	segparms["enable_boundary_blob_remover"] = config["segmentator"]["pris"]["enable_boundary_blob_remover"].as<int>();
	segparms["enable_morph_operations"] = config["segmentator"]["pris"]["enable_morph_operations"].as<int>();
	segparms["morph_erode_kernel"] = config["segmentator"]["pris"]["morph_erode_kernel"].as<int>();
	segparms["morph_dilate_kernel"] = config["segmentator"]["pris"]["morph_dilate_kernel"].as<int>();
	segparms["enable_manual_field_selection"] = config["segmentator"]["pris"]["enable_manual_field_selection"].as<int>();
	segparms["enable_white_remover"] = config["segmentator"]["pris"]["enable_white_remover"].as<int>();
	segparms["top_left_point_x"] = config["segmentator"]["pris"]["top_left_point_x"].as<int>();
	segparms["top_left_point_y"] = config["segmentator"]["pris"]["top_left_point_y"].as<int>();
	segparms["top_right_point_x"] = config["segmentator"]["pris"]["top_right_point_x"].as<int>();
	segparms["top_right_point_y"] = config["segmentator"]["pris"]["top_right_point_y"].as<int>();
	segparms["bottom_left_point_x"] = config["segmentator"]["pris"]["bottom_left_point_x"].as<int>();
	segparms["bottom_left_point_y"] = config["segmentator"]["pris"]["bottom_left_point_y"].as<int>();
	segparms["bottom_right_point_x"] = config["segmentator"]["pris"]["bottom_right_point_x"].as<int>();
	segparms["bottom_right_point_y"] = config["segmentator"]["pris"]["bottom_right_point_y"].as<int>();
	
	segparms["history"] = config["segmentator"]["mog"]["history"].as<int>();
	segparms["learningRate"] = config["segmentator"]["mog"]["learningRate"].as<int>();
	segparms["varThreshold"] = config["segmentator"]["mog"]["varThreshold"].as<int>();
	segparms["bShadowDetection"] = config["segmentator"]["mog"]["bShadowDetection"].as<int>();
	segparms["porcentaje_espuriasmog"] = config["segmentator"]["mog"]["porcentaje_espurias"].as<int>();
	segparms["history"] = config["segmentator"]["mog"]["history"].as<int>();
	segparms["gaussian_kernel"] = config["segmentator"]["mog"]["gaussian_kernel"].as<int>();
	
	weights["position"] = config["mpgt"]["position_weight"].as<double>();
	weights["histogram"] = config["mpgt"]["histogram_weight"].as<double>();
	weights["contour"] = config["mpgt"]["contour_weight"].as<double>();
	weights["area"] = config["mpgt"]["area_weight"].as<double>();
	weights["speed"] = config["mpgt"]["speed_weight"].as<double>();
	weights["direction"] = config["mpgt"]["direction_weight"].as<double>();
	weights["depth"] = config["mpgt"]["depth_weight"].as<double>();
	weights["max_speed"] = config["mpgt"]["max_speed"].as<double>();
	weights["max_merge_distance"] = config["mpgt"]["max_merge_distance"].as<double>();
	
	mpgtparms["min_blob_area_threshold"]=config["mpgt"]["min_blob_area_threshold"].as<int>();
	mpgtparms["merged_blob_area_threshold"]=config["mpgt"]["merged_blob_area_threshold"].as<int>();
	mpgtparms["blob_memoir_lenght"]=config["mpgt"]["blob_memoir_lenght"].as<int>();
	mpgtparms["memoir_diff_threshold"]=config["mpgt"]["memoir_diff_threshold"].as<int>();
	mpgtparms["enable_oclusion_solver"]=config["mpgt"]["enable_oclusion_solver"].as<int>();
	mpgtparms["enable_abduction_finder"]=config["mpgt"]["enable_abduction_finder"].as<int>();
	mpgtparms["enable_clustering"]=config["mpgt"]["enable_clustering"].as<int>();
	mpgtparms["hue_clustering_only"]=config["mpgt"]["hue_clustering_only"].as<int>();
	mpgtparms["how_many_colors"]=config["mpgt"]["how_many_colors"].as<int>();
	mpgtparms["kmean_colors_cuantization"]=config["mpgt"]["kmean_colors_cuantization"].as<int>();
	mpgtparms["area_as_cluster_feature"]=config["mpgt"]["area_as_cluster_feature"].as<int>();
	mpgtparms["clustering_mode"]=config["mpgt"]["clustering_mode"].as<int>();
	mpgtparms["n_clusters"]=config["mpgt"]["n_clusters"].as<int>();
	mpgtparms["match_method"]=config["mpgt"]["match_method"].as<int>();
	mpgtparms["match_threshold"]=config["mpgt"]["match_threshold"].as<int>();

	drawparms["enable_boundary_blob_remover"]=config["drawer"]["enable_boundary_blob_remover"].as<int>();
	drawparms["lower_boundary"]=config["drawer"]["lower_boundary"].as<int>();
	drawparms["upper_boundary"]=config["drawer"]["upper_boundary"].as<int>();
	drawparms["draw_ids"]=config["drawer"]["draw_ids"].as<int>();
	drawparms["draw_contours"]=config["drawer"]["draw_contours"].as<int>();
	drawparms["draw_bounding_box"]=config["drawer"]["draw_bounding_box"].as<int>();
	drawparms["draw_blob_ids_also"]=config["drawer"]["draw_blob_ids_also"].as<int>();
	drawparms["draw_cluster_ids_also"]=config["drawer"]["draw_cluster_ids_also"].as<int>();
	drawparms["draw_trayectories"]=config["drawer"]["draw_trayectories"].as<int>();
	drawparms["font_scale"]=config["drawer"]["font_scale"].as<int>();
	drawparms["bounding_box_thickness"]=config["drawer"]["bounding_box_thickness"].as<int>();
	drawparms["trayectories_radius"]=config["drawer"]["trayectories_radius"].as<int>();
	drawparms["enable_color_balance"]=config["drawer"]["enable_color_balance"].as<int>();
	drawparms["color_balance_percent"]=config["drawer"]["color_balance_percent"].as<int>();
	
	start_frame = config["mpgt"]["start_frame"].as<int>();
	stop_frame = config["mpgt"]["stop_frame"].as<int>();
	init_tkp_threshold = config["mpgt"]["init_tkp_threshold"].as<int>();
	graph_window = config["mpgt"]["graph_window"].as<int>();

}

void YamlParser::StoreHeaderInfo(std::string input_file, MediaInfo bufVideoInfo)
{
	if(write_enable){
		yout << YAML::Comment("---Tracking file results---") << YAML::Newline;
		yout << YAML::BeginMap;
		yout << YAML::Key << "Configuration";
				
		//yout << YAML::Newline;
		yout << YAML::Comment("Video information");
		yout << YAML::BeginMap;
		yout << YAML::Key << "Video file"; yout << YAML::Value << input_file << YAML::Newline;
		yout << YAML::Key << "Frame per seconds"; yout << YAML::Value << bufVideoInfo.fps;
		yout << YAML::Key << "Total frames in video"; yout << YAML::Value << bufVideoInfo.frames;
		yout << YAML::Key << "Width"; yout << YAML::Value << bufVideoInfo.width;
		yout << YAML::Key << "Height"; yout << YAML::Value << bufVideoInfo.height << YAML::Newline;
				
		//yout << YAML::Newline;
		yout << YAML::Comment("Segmentation parameters");		
		yout << YAML::Key << "Mode"; yout << YAML::Value << segparms["mode"];
		yout << YAML::Key << "Hue Min"; yout << YAML::Value << hue_min;
		yout << YAML::Key << "Hue Max"; yout << YAML::Value << hue_max;
		yout << YAML::Key << "Hue Threshold"; yout << YAML::Value << umbral_hue;
		yout << YAML::Key << "Kernel Size"; yout << YAML::Value << tamano_kernel;
		yout << YAML::Key << "Sporius"; yout << YAML::Value << porcentaje_espurias;
		yout << YAML::Key << "Enable line remover"; yout << segparms["enable_line_remover"];
		yout << YAML::Key << "Enable green blob remover"; yout << segparms["enable_green_blob_remover"];
		yout << YAML::Key << "Line kernel"; yout << YAML::Value << segparms["line_kernel"];
		yout << YAML::Key << "minLineLenght"; yout << YAML::Value << segparms["minLineLength"];
		yout << YAML::Key << "maxLineGap"; yout << YAML::Value << segparms["maxLineGap"];
		yout << YAML::Key << "Line contour remover"; yout << YAML::Value << segparms["line_contour_remover"];
		yout << YAML::Key << "history"; yout << YAML::Value << segparms["history"];
		yout << YAML::Key << "varThreshold"; yout << YAML::Value << segparms["varThreshold"];
		yout << YAML::Key << "bShadowDetection"; yout << YAML::Value << segparms["bShadowDetection"];
		yout << YAML::Key << "learningRate"; yout << YAML::Value << segparms["learningRate"] << YAML::Newline;
		
		//yout << YAML::Newline;
		yout << YAML::Comment("Multipartite graph tracker parameters");
		//yout << YAML::Newline;
		yout << YAML::Key << "Position Weight"; yout << YAML::Value << weights["position"];
		yout << YAML::Key << "Histogram Weight"; yout << YAML::Value << weights["histogram"];
		yout << YAML::Key << "Contour Weight"; yout << YAML::Value << weights["contour"];
		yout << YAML::Key << "Area Weight"; yout << YAML::Value << weights["area"];
		yout << YAML::Key << "Window Size"; yout << YAML::Value << graph_window << YAML::Newline;
		yout << YAML::EndMap;
		//yout << YAML::Newline;		
		
		yout << YAML::EndMap;
	}
}

void YamlParser::InitResultWriter(std::string track_file)
{
	std::ifstream stream(track_file);
	if (stream.good())
		cout << "Tracking file: " << track_file << " exist, overwriting!" << endl;
	
	tracking_file=track_file;
	//track_node = YAML::LoadFile(tracking_file); 
	write_enable = YES;
}

void YamlParser::CloseResultWriter(void)
{
	write_enable = NO;
	cout << "Writing results to yaml tracking file." << endl;
	std::ofstream fout(tracking_file); 
	//fout << yout; // dump into the output file	
	fout << yout.c_str();
}

void YamlParser::StoreFrameResult(Graph graph)
{
	if(write_enable && (graph.size()>0)){
		int fg_number=graph[0].blob_frame_number;
		D(cout << "Writing results for frame " << fg_number << endl;)
			
		yout << YAML::BeginMap;
		yout << YAML::Key << "Frame";
		yout << YAML::Value << fg_number;
		yout << YAML::Key << "Blobs";
		
		yout << YAML::BeginSeq;
/*		
		for (unsigned int ib=0;ib<graph.size();ib++){
			yout << YAML::BeginMap;
			yout << YAML::Key << "Player id";
			yout << YAML::Value << graph[ib].player_tag;
			yout << YAML::Key << "x";
			yout << YAML::Value << (int)graph[ib].centroid.x;
			yout << YAML::Key << "y";
			yout << YAML::Value << (int)graph[ib].centroid.y;
			yout << YAML::EndMap;	
		}
*/
		for (unsigned int ib=0;ib<graph.size();ib++){
			for (map< int,Point2f >::iterator mtcit=graph[ib].merged_centroids.begin(); 
							 mtcit!=graph[ib].merged_centroids.end(); ++mtcit){
			
				yout << YAML::BeginMap;
				yout << YAML::Key << "Player id";
				yout << YAML::Value << mtcit->first;
				yout << YAML::Key << "x";
				yout << YAML::Value << (int)mtcit->second.x;
				yout << YAML::Key << "y";
				yout << YAML::Value << (int)mtcit->second.y;
				yout << YAML::EndMap;	
				
			}
		}

		yout << YAML::EndSeq;
		
		yout << YAML::EndMap; //Este es del frame
		
	}
}


vector< TrackPoint > YamlParser::LoadTrackingFile(std::string track_file, std::string input_file)
{
	vector< TrackPoint > loaded_tracking_points;

	std::vector<YAML::Node> blocks = YAML::LoadAllFromFile(track_file);

	if (blocks[0].IsNull()){
		cout << "Error loading tracking file: " << track_file << endl;
		exit(1);
	}
	
	else{
		cout << "Init tracking file loading: " << track_file << endl;
	}
/*	
	const std::string conf_input_file = blocks[0]["Configuration"]["Video file"].as<std::string>();
	if (conf_input_file.compare(input_file) != 0){
		cout << "Warning: Video file on tracking file is different from input file" << endl;
		//exit(1);
	}
*/
/*
	for(std::vector<YAML::Node>::const_iterator block=blocks.begin(); block != blocks.end(); ++block) {
		//std::cout << "Next document of " << blocks.size() << endl;
		//parse(*doc);
		
	}	
*/

	for(unsigned int i = 0; i<blocks.size();i++){
				
		TrackPoint tkpt;
		YAML::Node data=blocks[i];
		
		tkpt.frame_number = data["Frame"].as<int>();
		
		YAML::Node blobs = data["Blobs"];
		for(unsigned int ib = 0; ib < blobs.size(); ib++){
			tkpt.label = blobs[ib]["Player id"].as<int>();
			tkpt.point.x = blobs[ib]["x"].as<int>();
			tkpt.point.y = blobs[ib]["y"].as<int>();
			loaded_tracking_points.push_back(tkpt);			
		}
	}

	return loaded_tracking_points;
}

YamlParser::~YamlParser()
{
	
}
