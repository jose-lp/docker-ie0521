/** @file YamlParser.h
 *  @brief Routines for parsing data as yaml
 *
 *  @details Routines for parsing data as yaml
 *  @bug Por determinar
 */
#ifndef YAML_PARSER_HPP
#define YAML_PARSER_HPP

//includes
#include <iostream>
#include <fstream>
#include <string>
#include "yaml-cpp/yaml.h"
#include "Structs.h"
#include "Defs.h"
#include <cstdlib>

using namespace std;


class YamlParser
{

private:
	///Data members
	//YAML::Node track_node; //**< Basenode for the tracking yaml file */
	YAML::Emitter yout;
	std::string tracking_file;
	bool write_enable;

	///Methods
	/**
	 * @brief Write to the tracking file a tracking point
	 * @details Write the tracking point directly to the YAML buffer
	 * @param TrackPoint tkpoint: A manual annotation tracking point
	 * */	
	void WriteTrackPoint(TrackPoint tkpoint);
	void WriteFullTrackPoint(TrackPoint tkpoint);	

public:
	///Data members
	std::map<std::string,int> anotator_parms;	
	
	///Methods
	/**Simple contructor*/
	YamlParser();
	

	/**
	 * @brief Store header info on the tracking file.
	 * @details Store header info made of program parameters and video info as a
	 *          header to the yaml tracking file.
	 * @param string input_file: Path of the input video file.
	 * @param MediaInfo bufVideoInfo: Video media information.
	 * */
	void StoreHeaderAnnotationInfo(std::string input_file, MediaInfo bufVideoInfo);
	
	/**
	 * @brief Initialize the result writer.
	 * @details Initialize the result writer, must be called before inserting
	 *          nodes into the parser.
	 * @param string tracking_file: Path of the output tracking yaml file.
	 * */	
	void InitResultWriter(std::string track_file);

	/**
	 * @brief Write to the tracking file a vector of tracking points
	 * @details Write the tracking point directly to the YAML buffer from
	 *          a vector of tracking points structs.
	 * @param vector< TrackPoint > vpoints: A manual annotation vector of tracking points
	 * */	
	void WriteVectorPoint(vector< TrackPoint > vpoints);

	/**
	 * @brief Load an existing tracking file with data.
	 * @details Load an existing tracking file with data.
	 * @param std::string track_file: Tracking file to load.
	 * @param std::string input_file: Input video file for the corresponding track file.
	 * @return Return a vector of TrackPoint objects
	 * */
	vector< TrackPoint > LoadTrackingFile(std::string track_file, std::string input_file);
	
	/**
	 * @brief Close the result writer.
	 * @param Close the root node and dumps it to the output tracking yaml file.
	 * */
	void CloseResultWriter(void);

	/**
	 * @brief Load the anotator configuracion.
	 * @param Input yaml configuration file.
	 * */	
	void LoadAnotatorConfig(std::string config_file);
	
	/**Simple distructor*/
	~YamlParser();
};

#endif
