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
	YAML::Emitter yout;

	///Methods


public:
	///Data members
	std::map<std::string,int> joiner_parms;	
	std::map<std::string,std::string> joiner_sparms;
	bool write_enable;

	
	///Methods
	/**Simple contructor*/
	YamlParser();

	/**
	 * @brief Store joiner configuration file.
	 * @details Store the joiner configuration parameters if save option is selected
	 * @param string output_file: Path of the output config file.
	 * @param std::map<std::string,int> parms: Joiner parameters to save.
	 * */
	void StoreJoinerConfig(std::string output_file, std::map<std::string,int> parms);	

	/**
	 * @brief Load the video joiner configuracion.
	 * @param Input yaml configuration file.
	 * */	
	void LoadJoinerConfig(std::string config_file);
	
	/**Simple distructor*/
	~YamlParser();
};

#endif
