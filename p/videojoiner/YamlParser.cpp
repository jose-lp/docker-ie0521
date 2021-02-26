#include "YamlParser.h"

using namespace std;


YamlParser::YamlParser()
{
		write_enable = false;
}


void YamlParser::LoadJoinerConfig(std::string config_file)
{
	YAML::Node config = YAML::LoadFile(config_file);
	if (config.IsNull()){
		cout << "Error loading config file: " << config_file << endl;
		exit(1);
	}

	joiner_parms["esc_key"] = config["keys"]["esc_key"].as<int>();
	joiner_parms["quit_key"] = config["keys"]["quit_key"].as<int>();
	joiner_parms["enter_key"] = config["keys"]["enter_key"].as<int>();
	joiner_parms["safe_key"] = config["keys"]["safe_key"].as<int>();
	joiner_parms["next_key"] = config["keys"]["next_key"].as<int>();
	joiner_parms["left_key"] = config["keys"]["left_key"].as<int>();
	joiner_parms["right_key"] = config["keys"]["right_key"].as<int>();
	joiner_parms["up_key"] = config["keys"]["up_key"].as<int>();
	joiner_parms["down_key"] = config["keys"]["down_key"].as<int>();
	
	joiner_parms["left_video_start_frame"] = config["joiner"]["left_video_start_frame"].as<int>();
	joiner_parms["right_video_start_frame"] = config["joiner"]["right_video_start_frame"].as<int>();
	joiner_parms["point_selection_mode"] = config["joiner"]["point_selection_mode"].as<int>();

	joiner_parms["left_video_top_left_x"] = config["joiner"]["left_video_top_left_x"].as<int>();
	joiner_parms["left_video_top_right_x"] = config["joiner"]["left_video_top_right_x"].as<int>();
	joiner_parms["left_video_bottom_left_x"] = config["joiner"]["left_video_bottom_left_x"].as<int>();
	joiner_parms["left_video_bottom_right_x"] = config["joiner"]["left_video_bottom_right_x"].as<int>();

	joiner_parms["left_video_top_left_y"] = config["joiner"]["left_video_top_left_y"].as<int>();
	joiner_parms["left_video_top_right_y"] = config["joiner"]["left_video_top_right_y"].as<int>();
	joiner_parms["left_video_bottom_left_y"] = config["joiner"]["left_video_bottom_left_y"].as<int>();
	joiner_parms["left_video_bottom_right_y"] = config["joiner"]["left_video_bottom_right_y"].as<int>();

	joiner_parms["right_video_top_left_x"] = config["joiner"]["right_video_top_left_x"].as<int>();
	joiner_parms["right_video_top_right_x"] = config["joiner"]["right_video_top_right_x"].as<int>();
	joiner_parms["right_video_bottom_left_x"] = config["joiner"]["right_video_bottom_left_x"].as<int>();
	joiner_parms["right_video_bottom_right_x"] = config["joiner"]["right_video_bottom_right_x"].as<int>();

    joiner_parms["right_video_top_left_y"] = config["joiner"]["right_video_top_left_y"].as<int>();
	joiner_parms["right_video_top_right_y"] = config["joiner"]["right_video_top_right_y"].as<int>();
	joiner_parms["right_video_bottom_left_y"] = config["joiner"]["right_video_bottom_left_y"].as<int>();
	joiner_parms["right_video_bottom_right_y"] = config["joiner"]["right_video_bottom_right_y"].as<int>();

	joiner_parms["video_top_left_x"] = config["joiner"]["video_top_left_x"].as<int>();
	joiner_parms["video_top_right_x"] = config["joiner"]["video_top_right_x"].as<int>();
	joiner_parms["video_bottom_left_x"] = config["joiner"]["video_bottom_left_x"].as<int>();
	joiner_parms["video_bottom_right_x"] = config["joiner"]["video_bottom_right_x"].as<int>();
    joiner_parms["video_top_left_y"] = config["joiner"]["video_top_left_y"].as<int>();
	joiner_parms["video_top_right_y"] = config["joiner"]["video_top_right_y"].as<int>();
	joiner_parms["video_bottom_left_y"] = config["joiner"]["video_bottom_left_y"].as<int>();
	joiner_parms["video_bottom_right_y"] = config["joiner"]["video_bottom_right_y"].as<int>();
	joiner_parms["enable_output_video_crop"] = config["joiner"]["enable_output_video_crop"].as<int>();

	joiner_parms["sync_mode"] = config["joiner"]["sync_mode"].as<int>();
	joiner_sparms["left_timecode"] = config["joiner"]["left_timecode"].as<std::string>();
	joiner_sparms["right_timecode"] = config["joiner"]["right_timecode"].as<std::string>();
	joiner_parms["enable_drop_frame"] = config["joiner"]["enable_drop_frame"].as<int>();
	joiner_sparms["frame_rate"] = config["joiner"]["frame_rate"].as<std::string>();
	joiner_parms["start_frame"] = config["joiner"]["start_frame"].as<int>();
	joiner_parms["stop_frame"] = config["joiner"]["stop_frame"].as<int>();
	joiner_parms["frame2skip"] = config["joiner"]["frame2skip"].as<int>();
	joiner_parms["enable_color_balance"] = config["joiner"]["enable_color_balance"].as<int>();
	joiner_parms["enable_s_color_correction"] = config["joiner"]["enable_s_color_correction"].as<int>();
	joiner_parms["enable_v_color_correction"] = config["joiner"]["enable_v_color_correction"].as<int>();
	joiner_sparms["color_correction_video_reference"] = config["joiner"]["color_correction_video_reference"].as<std::string>();
	joiner_parms["enable_color_region_selection"] = config["joiner"]["enable_color_region_selection"].as<int>();
	joiner_parms["stitch_mode"] = config["joiner"]["stitch_mode"].as<int>();
	joiner_parms["divide_images"] = config["joiner"]["divide_images"].as<int>();
	joiner_parms["try_use_gpu"] = config["joiner"]["try_use_gpu"].as<int>();
	joiner_parms["enable_top_corners_correction"] = config["joiner"]["enable_top_corners_correction"].as<int>();
	joiner_parms["left_top_corner_correction"] = config["joiner"]["left_top_corner_correction"].as<int>();
	joiner_parms["right_top_corner_correction"] = config["joiner"]["right_top_corner_correction"].as<int>();
}

void YamlParser::StoreJoinerConfig(std::string output_file, std::map<std::string,int> parms)
{
	//if(write_enable){
		std::ifstream stream(output_file);
		if (stream.good())
			cout << "Saving and overwriting configuration file " << output_file << endl;

			
		joiner_parms = parms;
		
		yout << YAML::Comment("--- # Configuration file") << YAML::Newline;

		yout << YAML::BeginMap;
		yout << YAML::Key << "keys";
			yout << YAML::BeginMap;
			yout << YAML::Key << "esc_key"; yout << YAML::Value << joiner_parms["esc_key"];
			yout << YAML::Key << "quit_key"; yout << YAML::Value << joiner_parms["quit_key"];
			yout << YAML::Key << "safe_key"; yout << YAML::Value << joiner_parms["safe_key"];
			yout << YAML::Key << "next_key"; yout << YAML::Value << joiner_parms["next_key"];
			yout << YAML::Key << "left_key"; yout << YAML::Value << joiner_parms["left_key"];
			yout << YAML::Key << "right_key"; yout << YAML::Value << joiner_parms["right_key"];
			yout << YAML::Key << "down_key"; yout << YAML::Value << joiner_parms["down_key"];
			yout << YAML::Key << "up_key"; yout << YAML::Value << joiner_parms["up_key"];
			yout << YAML::Key << "enter_key"; yout << YAML::Value << joiner_parms["enter_key"] << YAML::Newline;
			yout << YAML::EndMap;
		yout << YAML::EndMap;

		yout << YAML::BeginMap;
		yout << YAML::Key << "joiner";
			yout << YAML::BeginMap;

			yout << YAML::Key << "left_video_start_frame"; yout << YAML::Value << joiner_parms["left_video_start_frame"];
			yout << YAML::Key << "right_video_start_frame"; yout << YAML::Value << joiner_parms["right_video_start_frame"];
			yout << YAML::Key << "point_selection_mode"; yout << YAML::Value << joiner_parms["point_selection_mode"];

			yout << YAML::Key << "left_video_top_left_x"; yout << YAML::Value << joiner_parms["left_video_top_left_x"];
			yout << YAML::Key << "left_video_top_right_x"; yout << YAML::Value << joiner_parms["left_video_top_right_x"];
			yout << YAML::Key << "left_video_bottom_left_x"; yout << YAML::Value << joiner_parms["left_video_bottom_left_x"];
			yout << YAML::Key << "left_video_bottom_right_x"; yout << YAML::Value << joiner_parms["left_video_bottom_right_x"];

			yout << YAML::Key << "left_video_top_left_y"; yout << YAML::Value << joiner_parms["left_video_top_left_y"];
			yout << YAML::Key << "left_video_top_right_y"; yout << YAML::Value << joiner_parms["left_video_top_right_y"];
			yout << YAML::Key << "left_video_bottom_left_y"; yout << YAML::Value << joiner_parms["left_video_bottom_left_y"];
			yout << YAML::Key << "left_video_bottom_right_y"; yout << YAML::Value << joiner_parms["left_video_bottom_right_y"];

			yout << YAML::Key << "right_video_top_left_x"; yout << YAML::Value << joiner_parms["right_video_top_left_x"];
			yout << YAML::Key << "right_video_top_right_x"; yout << YAML::Value << joiner_parms["right_video_top_right_x"];
			yout << YAML::Key << "right_video_bottom_left_x"; yout << YAML::Value << joiner_parms["right_video_bottom_left_x"];
			yout << YAML::Key << "right_video_bottom_right_x"; yout << YAML::Value << joiner_parms["right_video_bottom_right_x"];			
			
			yout << YAML::Key << "right_video_top_left_y"; yout << YAML::Value << joiner_parms["right_video_top_left_y"];
			yout << YAML::Key << "right_video_top_right_y"; yout << YAML::Value << joiner_parms["right_video_top_right_y"];
			yout << YAML::Key << "right_video_bottom_left_y"; yout << YAML::Value << joiner_parms["right_video_bottom_left_y"];
			yout << YAML::Key << "right_video_bottom_right_y"; yout << YAML::Value << joiner_parms["right_video_bottom_right_y"];


			yout << YAML::Key << "video_top_left_x"; yout << YAML::Value << joiner_parms["video_top_left_x"];
			yout << YAML::Key << "video_top_right_x"; yout << YAML::Value << joiner_parms["video_top_right_x"];
			yout << YAML::Key << "video_bottom_left_x"; yout << YAML::Value << joiner_parms["video_bottom_left_x"];
			yout << YAML::Key << "video_bottom_right_x"; yout << YAML::Value << joiner_parms["video_bottom_right_x"];			
			yout << YAML::Key << "video_top_left_y"; yout << YAML::Value << joiner_parms["video_top_left_y"];
			yout << YAML::Key << "video_top_right_y"; yout << YAML::Value << joiner_parms["video_top_right_y"];
			yout << YAML::Key << "video_bottom_left_y"; yout << YAML::Value << joiner_parms["video_bottom_left_y"];
			yout << YAML::Key << "video_bottom_right_y"; yout << YAML::Value << joiner_parms["video_bottom_right_y"];
			yout << YAML::Key << "enable_output_video_crop"; yout << YAML::Value << joiner_parms["enable_output_video_crop"];
			
			yout << YAML::Key << "sync_mode"; yout << YAML::Value << joiner_parms["sync_mode"];			
			yout << YAML::Key << "left_timecode"; yout << YAML::Value << joiner_sparms["left_timecode"];			
			yout << YAML::Key << "right_timecode"; yout << YAML::Value << joiner_sparms["right_timecode"];			
			yout << YAML::Key << "enable_drop_frame"; yout << YAML::Value << joiner_parms["enable_drop_frame"];			
			yout << YAML::Key << "frame_rate"; yout << YAML::Value << joiner_sparms["frame_rate"];			
			yout << YAML::Key << "start_frame"; yout << YAML::Value << joiner_parms["start_frame"];			
			yout << YAML::Key << "stop_frame"; yout << YAML::Value << joiner_parms["stop_frame"];			
			yout << YAML::Key << "frame2skip"; yout << YAML::Value << joiner_parms["frame2skip"];

			yout << YAML::Key << "enable_color_balance"; yout << YAML::Value << joiner_parms["enable_color_balance"];
			yout << YAML::Key << "enable_s_color_correction"; yout << YAML::Value << joiner_parms["enable_s_color_correction"];
			yout << YAML::Key << "enable_v_color_correction"; yout << YAML::Value << joiner_parms["enable_v_color_correction"];
			yout << YAML::Key << "color_correction_video_reference"; yout << YAML::Value << joiner_sparms["color_correction_video_reference"];
			yout << YAML::Key << "enable_color_region_selection"; yout << YAML::Value << joiner_parms["enable_color_region_selection"];
			yout << YAML::Key << "stitch_mode"; yout << YAML::Value << joiner_parms["stitch_mode"];
			yout << YAML::Key << "divide_images"; yout << YAML::Value << joiner_parms["divide_images"];
			yout << YAML::Key << "try_use_gpu"; yout << YAML::Value << joiner_parms["try_use_gpu"];
			yout << YAML::Key << "enable_top_corners_correction"; yout << YAML::Value << joiner_parms["enable_top_corners_correction"];
			yout << YAML::Key << "left_top_corner_correction"; yout << YAML::Value << joiner_parms["left_top_corner_correction"];
			yout << YAML::Key << "right_top_corner_correction"; yout << YAML::Value << joiner_parms["right_top_corner_correction"];
			
			yout << YAML::EndMap;
		yout << YAML::EndMap;
		
		cout << "Writing config file..." << endl;
		std::ofstream fout(output_file); 
		fout << yout.c_str();
	//}
}

YamlParser::~YamlParser()
{
	
}
