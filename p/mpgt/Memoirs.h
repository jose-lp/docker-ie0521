/** @file MultiPartiteGraph.h
 *  @brief Class for a MultiPartiteGraph representation
 *
 *  @details Class for a MultiPartiteGraph representation
 *  @bug Por determinar
 */
#ifndef MGT_MEMOIR_HPP
#define MGT_MEMOIR_HPP

#include "Structs.h"
#include "Defs.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>
#include <string>
#include <map>
#include <float.h>
#include <algorithm>
#include "PlayerBlob.h"
#include "PrisSerial.h"

using namespace std;
using namespace cv;

//typedef std::map<std::pair<int, int>, double> Dict; //Kind of a extended dictionary, of ints and a double
//typedef Dict::const_iterator It;
typedef PlayerBlob Node; //In this case, a Node is a PlayerBlob
typedef std::vector<Node> Graph; // A Graph is made off Nodes
typedef std::map<unsigned int,Graph> MultiGraph; //MultiGraph is made off hashes of this type:(id,Graph)
//typedef Dict Edge; //Border/conection alias to store data as (src_id,dst_id, weight) 

/** @class Memoir
 *  @brief Abstraction of a node as a memory.
 *  @details This class abstracts a node as a human memory which is used
 *  to remember and eventualy with time, forget it.
 */
class Memoir
{
	private:
	
	public:
	Node info; //**< Information(node) to remember */
	int time; //**< Memory life time/span */
	

	/**Simple constructor*/
	Memoir();	

	void SetTime(int seconds);
	Node GetMemoir(void);
	int  GetTimeLeft(void);
	void SetMemoir(Node memoir);
	void Degrade(void);
	void Degrade(int ntimes);

	/**Simple distructor*/
	~Memoir();	
};

bool IsForgotten(Memoir m);

/** @class OclusionMemoir
 *  @brief Class to hold the ocluded nodes as memories.
 */
class OclusionMemoir
{
	private:
	double position_weight; //**< Position weight value used to compare memories */
	double histogram_weight; //**< Histogram weight value used to compare memories */
	double contour_weight; //**< Contour weight value used to compare memories */
	double area_weight; //**< Area weight value used to compare memories */
	double direction_weight; //**< Direction weight value used to compare memories */
	double speed_weight; //**< Speed weight value used to compare memories */
	double depth_weight; //**< Depth weight value used to compare memories */
	double memoir_diff_threshold; //**< Max threshold difference between nodes/memories to be considered with a relation */
	double max_speed; //**< Max speed that a player might have */
	int blob_memoir_lenght; //**< Life span for a memory */
	int match_method;
	int match_threshold;
	double CompareNodes(Node node1, Node node2);
	double CompareNodesWithDepth(Node node1, Node node2);
		
	public:
	std::vector<Memoir> vector_memoir; //**< Group of nodes remembered as memories */
	

	/**Simple constructor*/
	OclusionMemoir();	
	void InitMemoir(std::map<std::string,int> mpgtparms,std::map<std::string,double> weights);
	void Remember(Node to_remember,int how_long);
	void Remember(Node to_remember);
	void Forget(Node to_forget);
	Node GetClosestMemoir(Node to_search);
	Node GetClosestMemoirWithDepth(Node to_search);
	Node GetMemoirByTags(Node to_search, set<int> player_tags);
	Node GetMemoirByTagsAndTM1(Node to_search, set<int> player_tags, map< int,Point2f > merged_centroids);
	Node GetMemoirByTagsAndTM2(Node to_search, set<int> player_tags, map< int,Point2f > merged_centroids);
	Node GetMemoirByTag(int search_player_tag);
	Node ExtractClosestMemoir(Node to_search);
	void DegradeMemoirs(void);
	void DegradeMemoirs(int ntimes);
	bool TagIsInMemoir(int tag);

	/**Simple distructor*/
	~OclusionMemoir();	
};

//using AbductionMemoir = OclusionMemoir;
typedef OclusionMemoir AbductionMemoir; //**< Treat an oclusion memory as a abduction memory */

#endif
