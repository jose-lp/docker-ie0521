/** @file Measurer.h
 *  @brief Routines for validation of tracking algorithms
 *
 *  @details Routines for parsing data as yaml
 *  @bug Por determinar
 */
 
#ifndef MEASUERER_HPP
#define MEASUERER_HPP

//includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Structs.h"
#include "Defs.h"
#include <cstdlib>
#include <algorithm>
#include <math.h>
#include <iomanip> 
#include <omp.h>

using namespace std;


class Measurer
{
	
private:
	///Data members
	unsigned int misses;
	double false_positives;
	double false_negatives;
	double true_positives;
	double true_negatives;	
	double missmatches;
	double gt_objects;
	double tk_objects;
	double motp_error;
	double motp_matches;
	double precision;
	double recall;
	double accuracy;
	double f1measure;
	double motp;
	double mota;
	double start_frame;
	double stop_frame;
	double mota_mean;
	double motp_mean;
	double mota_variance;
	double motp_variance;
	///Methods

public:
	///Data members
	double cm,cf,cs;
	double report_level;
	double missmatch_error;
	double detection_error;
	
	///Methods
	/**Simple contructor*/
	Measurer();
	Measurer(std::map<std::string,double> constants);
	
	double MeasureMota(vector< TrackPoint > anotados, vector< TrackPoint > rastreados);
	double MeasureMota(map<int, vector< TrackPoint >> manotados, map<int, vector< TrackPoint >> mrastreados);
	double MeasureMota_old(vector< TrackPoint > anotados, vector< TrackPoint > rastreados);
	double MeasureMota(vector< TrackPoint > anotados, vector< TrackPoint > rastreados, int tid);
	double MeasureMotp(vector< TrackPoint > anotados, vector< TrackPoint > rastreados);
	double CalculateMean(vector< double > data);
	double CalculateVariance(vector< double > data);
	//double MeasureMotp(map<int, vector< TrackPoint >> manotados, map<int, vector< TrackPoint >> mrastreados);
	
	void SaveResults(std::string report_file, std::string input_file, std::string annotation_file);
		
	/**Simple distructor*/
	~Measurer();
};

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 2)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

#endif
