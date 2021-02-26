/** @file Timer.h
 *  @brief Function prototypes for profiling/timing.
 *
 *  @details This contains the definition of the class Timer
 *  for measuring code blocks using system time. Just to be use as a
 *  way to have a general notion of the performance.
 *
 */
#ifndef MPGT_TIMER_HPP
#define MPGT_TIMER_HPP

#include <sys/time.h>
#include <ctime>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <chrono>

//typedef long long int64;
//typedef unsigned long long uint64;

using namespace std;

/** @class Timer
 *  @brief Measurement functions for intrument inside the code blocks
 *  @details This abstract class includes functions that returns the
 *  elapsed time between two events with a resolution of microseconds
 *  or seconds.
 */
class Timer
{
private:
	double start_time;	//**< Start time */	
	bool running;		//**< Running state */
	string tag;			//**< Tag for the evenet. */
	double GetTimeSeconds(); //**< Function that returns a seconds measure. */


public:
	double accum; 		//**<Difference of start and stop calls */
	double running_time; //**< Accumulator of time for running time */
	
	/** The constructor */
	Timer();


	/**
	* @brief Starts a measure event.
	* @details Starts a measure event in seconds without a string tag.
	*
	*/
	void Start();


	/**
	* @brief Starts a measure event.
	* @details Starts a measure event in seconds with a tag.
	* @param string intag: String that tags the event.
	*
	*/
	void Start(string intag);


	/**
	* @brief Starts a measure event for serial programs.
	* @details Starts a measure event in seconds with a tag.
	* @param string intag: String that tags the event.
	*
	*/
	void SStart(string intag);


	/**
	* @brief Starts a microsend measure event.
	* @details Starts a measure event in seconds with a tag.
	* @param string intag: String that tags the event.
	*
	*/
	void Startus(string intag);


	/**
	* @brief Stops a measure event.
	* @details Stops a measure event in seconds. Only on the master process.
	* @param string intag: String that tags the event.
	*
	*/
	void Stop();


	/**
	* @brief Stops a measure event for serial programs.
	* @details Stops a measure event in seconds. Only on the master process.
	* @param string intag: String that tags the event.
	*
	*/
	void SStop();


	/**
	* @brief Stops a microsend measure event.
	* @details Stops a measure event in seconds. Only on the master process.
	*
	*/
	void Stopus();

	/**
	* @brief Starts a timer as a cronometer
	* @details Starts a timer in seconds with a tag as a cronometer
	* @param string intag: String that tags the event.
	*
	*/	
	void CrnStart(string intag);
	
	/**
	* @brief Pause the cronometer previously started.
	*/		
	void CrnPause();
	
	/**
	* @brief Resume the cronometer.
	* @details Resume the cronometer, use a internal register to keep track of the time.
	*/	
	void CrnResume();
	
	/**
	* @brief Stops the cronometer.
	*/			
	void CrnStop();

	/** The destructor */
	~Timer();
};


#endif
