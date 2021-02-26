/** @file BiPartiteTracket.h
 *  @brief Class for a BiPartite Graph Trackler
 *
 *  @details Class for a BiPartite Graph Trackler
 *  @bug Por determinar
 */
#ifndef MGT_BPT_HPP
#define MGT_BPT_HPP


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


using namespace std;
using namespace cv;

//typedef std::map<std::pair<int, int>, double> Dict; //Kind of a extended dictionary, of ints and a double
//typedef Dict::const_iterator It;
typedef PlayerBlob Node; //In this case, a Node is a PlayerBlob
typedef std::vector<Node> Graph; // A Graph is made off Nodes
typedef std::vector<TrackPoint> VectorTrackingPoints;
typedef std::map<int,VectorTrackingPoints> Tracklets; //A Tracklet in the form of player_tag,TrackPoint

/**
 * @class BiPartiteTracklerGraph
 * @brief Alternative class for the representation of tracks as tracklet
 * structures rather than a multipartitegraph.
*/
class BiPartiteTracklerGraph
{

private:


public:
	double position_weight;
	double histogram_weight;
	double contour_weight;
	double area_weight;
	double max_speed;
	Graph start_graph;
	Graph end_graph;
	Tracklets tklts;

	/**Simple contructor*/
	BiPartiteTracklerGraph();
	BiPartiteTracklerGraph(std::map<std::string,double> weights);

	void InsertGraph(Graph gph, int position);
	Graph GetGraph(int position);
	void InsertTrackpointToTracklet(int id, TrackPoint tp);
	std::vector<TrackPoint> GetVectorTrackingPointsFromTracklet(int id);
	std::vector<TrackPoint> GetVectorTrackingPointsFromFrame(int fnumber);
	void SetLabelToTracklet(int tracklet_id, int label);

	/**Simple distructor*/
	~BiPartiteTracklerGraph();
};

#endif
