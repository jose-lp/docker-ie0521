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
#include "MultiPartiteGraph.h"
#include "Structs.h"
#include "Defs.h"
#include <cstdlib>

using namespace std;

/** @class YamlParser
 *  @brief Class for the manipulation of yaml data
 *  @details Class definition for the input and output of yaml format data, 
 *  includes the configuration parameters and the results from the tracker.
 */
class YamlParser
{

private:
	///Data members
	//YAML::Node track_node; //**< Basenode for the tracking yaml file */
	YAML::Emitter yout; //**< Emitter pipe handler */
	std::string tracking_file; //**< Tracking file name */
	bool write_enable; //**< Enable the writing of results */

	///Methods
	

public:
	///Data members
	int hue_min; //**< Hue min segmentation parameter */
	int hue_max; //**< Hue max segmentation parameter */
	int porcentaje_espurias; //**< Spurious percent segmentation parameter */
	int tamano_kernel; //**< Kernel size segmentation parameter of the LogVariance filter */
	int umbral_hue; //**< Hue threshold segmentation parameter */
	int graph_window; //**< Multipartite graph window parameter */
	int mode; //**< Segmentation mode */
	int start_frame; //**<  Start frame */
	int stop_frame; //**< Stop frame */
	int init_tkp_threshold; //**< Considered threshold for binding a init track point to a blob */
	int max_merge_distance; //**< Not used */
	
	std::map<std::string,double> weights; //**< Features weights */
	std::map<std::string,int> segparms; //**< Segmentation parameters */
	std::map<std::string,int> mpgtparms; //**< Tracking parameters */
	std::map<std::string,int> drawparms; //**< Drawing parameters */
	
	
	///Methods
	/**Simple contructor*/
	YamlParser();
	
	/**
	 * @brief Load configuration parameters from a yaml file.
	 * @param string config_file: Path of the configuration file.
	 * */
	void LoadConfig(std::string config_file);


	/**
	 * @brief Load an existing tracking file with data.
	 * @details Load an existing tracking file with data.
	 * @param std::string track_file: Tracking file to load.
	 * @param std::string input_file: Input video file for the corresponding track file.
	 * @return Return a vector of TrackPoint objects
	 * */
	vector< TrackPoint > LoadTrackingFile(std::string track_file, std::string input_file);

	/**
	 * @brief Store header info on the tracking file.
	 * @details Store header info made of program parameters and video info as a
	 *          header to the yaml tracking file.
	 * @param string input_file: Path of the input video file.
	 * @param MediaInfo bufVideoInfo: Video media information.
	 * */
	void StoreHeaderInfo(std::string input_file, MediaInfo bufVideoInfo);
	
	/**
	 * @brief Initialize the result writer.
	 * @details Initialize the result writer, must be called before inserting
	 *          nodes into the parser.
	 * @param string tracking_file: Path of the output tracking yaml file.
	 * */	
	void InitResultWriter(std::string track_file);
	
	/**
	 * @brief Close the result writer.
	 * @param Close the root node and dumps it to the output tracking yaml file.
	 * */
	void CloseResultWriter(void);

	/**
	 * @brief 
	 * @param Close the root node and dumps it to the output tracking yaml file.
	 * */
	void StoreFrameResult(Graph graph);
	
	/**Simple distructor*/
	~YamlParser();
};

#endif
