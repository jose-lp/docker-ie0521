#include "YamlParser.h"

using namespace std;


YamlParser::YamlParser()
{
	write_enable = NO;
}



void YamlParser::StoreHeaderAnnotationInfo(std::string input_file, MediaInfo bufVideoInfo)
{
	if(write_enable){
		yout << YAML::Comment("---Tracking file results---") << YAML::Newline;
		yout << YAML::BeginMap;
		yout << YAML::Key << "Configuration";

		yout << YAML::BeginMap;
		yout << YAML::Key << "Video file"; yout << YAML::Value << input_file << YAML::Newline;
		yout << YAML::Key << "Frame per seconds"; yout << YAML::Value << bufVideoInfo.fps;
		yout << YAML::Key << "Total frames in video"; yout << YAML::Value << bufVideoInfo.frames;
		yout << YAML::Key << "Width"; yout << YAML::Value << bufVideoInfo.width;
		yout << YAML::Key << "Height"; yout << YAML::Value << bufVideoInfo.height << YAML::Newline;
		yout << YAML::Key << "output_version"; yout << YAML::Value << anotator_parms["output_version"] << YAML::Newline;
		yout << YAML::EndMap;

		yout << YAML::EndMap;
	}
}

void YamlParser::InitResultWriter(std::string track_file)
{
	std::ifstream stream(track_file);
	if (stream.good())
		cout << "Tracking file: " << track_file << " exist, overwriting if save option is selected!" << endl;
	
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

void YamlParser::WriteVectorPoint(vector< TrackPoint > vpoints)
{
	
	std::sort(vpoints.begin(), vpoints.end()); //sort points by frame number
	
	int previous_fnumber = -1, current_fnumber = 0;
	
	if(anotator_parms["output_version"]==1){
		for(unsigned int i = 0; i < vpoints.size(); i++){
			current_fnumber = vpoints[i].frame_number;
			if(current_fnumber != previous_fnumber){
				if(i != 0){
					yout << YAML::EndSeq;
					yout << YAML::EndMap;
				}
				yout << YAML::BeginMap;
				yout << YAML::Key << "Frame";
				yout << YAML::Value << current_fnumber;
				yout << YAML::Key << "Blobs";
				yout << YAML::BeginSeq;						
			}
			WriteTrackPoint(vpoints[i]);
			previous_fnumber = current_fnumber;		
		}
		yout << YAML::EndSeq;
		yout << YAML::EndMap;	
	}
	else if(anotator_parms["output_version"]==2){
		yout << YAML::BeginSeq;
		
		for(unsigned int i = 0; i < vpoints.size(); i++)
			WriteFullTrackPoint(vpoints[i]);
		
		yout << YAML::EndSeq;		
	}
}

void YamlParser::WriteTrackPoint(TrackPoint tkpoint)
{
	if(write_enable){
		yout << YAML::BeginMap;
		yout << YAML::Key << "Player id";
		yout << YAML::Value << (int)tkpoint.label;
		yout << YAML::Key << "x";
		yout << YAML::Value << (int)tkpoint.point.x;
		yout << YAML::Key << "y";
		yout << YAML::Value << (int)tkpoint.point.y;
		yout << YAML::Key << "occluded";
		yout << YAML::Value << (int)tkpoint.state;
		yout << YAML::Key << "timestamp";
		yout << YAML::Value << (std::string)tkpoint.timestamp;
		yout << YAML::EndMap;		
	}
}

void YamlParser::WriteFullTrackPoint(TrackPoint tkpoint)
{
	if(write_enable){
		yout << YAML::BeginMap;
		yout << YAML::Key << "Frame";
		yout << YAML::Value << (int)tkpoint.frame_number;
		yout << YAML::Key << "Player id";
		yout << YAML::Value << (int)tkpoint.label;
		yout << YAML::Key << "x";
		yout << YAML::Value << (int)tkpoint.point.x;
		yout << YAML::Key << "y";
		yout << YAML::Value << (int)tkpoint.point.y;
		yout << YAML::Key << "occluded";
		yout << YAML::Value << (int)tkpoint.state;
		yout << YAML::EndMap;		
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

	///Sort trackpoints by frame number
	std::sort(loaded_tracking_points.begin(), loaded_tracking_points.end());

	return loaded_tracking_points;
}

void YamlParser::LoadAnotatorConfig(std::string config_file)
{
	YAML::Node config = YAML::LoadFile(config_file);
	if (config.IsNull()){
		cout << "Error loading config file: " << config_file << endl;
		exit(1);
	}
	
	anotator_parms["esc_key"] = config["keys"]["esc_key"].as<int>();
	anotator_parms["next_step_key"] = config["keys"]["next_step_key"].as<int>();
	anotator_parms["previous_step_key"] = config["keys"]["previous_step_key"].as<int>();
	anotator_parms["next_key"] = config["keys"]["next_key"].as<int>();
	anotator_parms["previous_key"] = config["keys"]["previous_key"].as<int>();
	anotator_parms["increase_step"] = config["keys"]["increase_step"].as<int>();
	anotator_parms["decrease_step"] = config["keys"]["decrease_step"].as<int>();
	anotator_parms["increase1sec_step"] = config["keys"]["increase1sec_step"].as<int>();
	anotator_parms["decrease1sec_step"] = config["keys"]["decrease1sec_step"].as<int>();
	anotator_parms["quit_key"] = config["keys"]["quit_key"].as<int>();
	anotator_parms["space_key"] = config["keys"]["space_key"].as<int>();
	anotator_parms["delete_key"] = config["keys"]["delete_key"].as<int>();
	anotator_parms["add_key"] = config["keys"]["add_key"].as<int>();
	anotator_parms["safe_key"] = config["keys"]["safe_key"].as<int>();
	anotator_parms["shift_del_key"] = config["keys"]["shift_del_key"].as<int>();
	anotator_parms["delete_single_key"] = config["keys"]["delete_single_key"].as<int>();
	anotator_parms["where_key"] = config["keys"]["where_key"].as<int>();
	
	anotator_parms["default_step_size"] = config["anotator"]["default_step_size"].as<int>();
	anotator_parms["enable_gtk_window"] = config["anotator"]["enable_gtk_window"].as<int>();
	anotator_parms["output_version"] = config["anotator"]["output_version"].as<int>();
	anotator_parms["tag_oclusions"] = config["anotator"]["tag_oclusions"].as<int>();
	anotator_parms["font_size"] = config["anotator"]["font_size"].as<int>();

}

YamlParser::~YamlParser()
{
	
}
