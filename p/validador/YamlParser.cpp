#include "YamlParser.h"

using namespace std;


YamlParser::YamlParser()
{
	write_enable = NO;
	constants["miss"] = 1.0;
	constants["false"] = 1.0;
	constants["mismatch"] = 1.0;
	constants["report_level"] = 0;
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

	return loaded_tracking_points;
}

void YamlParser::LoadMeasurerConfig(std::string config_file)
{
	YAML::Node config = YAML::LoadFile(config_file);
	if (config.IsNull()){
		cout << "Error loading config file: " << config_file << endl;
		exit(1);
	}
	
	constants["start_frame"] = config["validator"]["start_frame"].as<int>();
	constants["stop_frame"] = config["validator"]["stop_frame"].as<int>();
	constants["miss"] = config["mota"]["miss_constant"].as<double>();
	constants["false"] = config["mota"]["false_positive_constant"].as<double>();
	constants["mismatch"] = config["mota"]["mismatches_constant"].as<double>();
	constants["report_level"] = config["validator"]["report_level"].as<int>();
	constants["motp_error"] = config["motp"]["precision_error"].as<double>();
	constants["mismatch_error"] = config["mota"]["mismatch_error"].as<double>();
	constants["detection_error"] = config["mota"]["detection_error"].as<double>();

}


YamlParser::~YamlParser()
{
	
}
