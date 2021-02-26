/** @file MultiPartiteGraph.h
 *  @brief Class for a MultiPartiteGraph representation
 *
 *  @details Class for a MultiPartiteGraph representation
 *  @bug Por determinar
 */
#ifndef MGT_MPG_HPP
#define MGT_MPG_HPP

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
#include "Memoirs.h"

using namespace std;
using namespace cv;

//typedef std::map<std::pair<int, int>, double> Dict; //Kind of a extended dictionary, of ints and a double
//typedef Dict::const_iterator It;
typedef PlayerBlob Node; //In this case, a Node is a PlayerBlob
typedef std::vector<Node> Graph; // A Graph is made off Nodes
typedef std::map<unsigned int,Graph> MultiGraph; //MultiGraph is made off hashes of this type:(id,Graph)
//typedef Dict Edge; //Border/conection alias to store data as (src_id,dst_id, weight) 

/**
 * @class MultiPartiteGraph
 * @brief Class for the abstraction of graphs, nodes and their operations 
 * related to the properties of a blob that might be a soccer player.
*/
class MultiPartiteGraph
{

private:
	///Data members
	unsigned int window_size;
	int number_of_graphs;
	std::vector<Edge> edges;
	double position_weight;
	double histogram_weight;
	double contour_weight;
	double area_weight;
	int max_speed;
	int max_merge_distance;
	int enable_boundary_blob_remover;
	int enable_oclusion_solver;
	OclusionMemoir oclusion_memoir;
	AbductionMemoir abduction_memoir;

	///Methods
	void InsertGraph(int graph_id);
	void InsertGraph(int graph_id, Graph graph);	

public:
	///Data members
	int initial_frame;
	int last_frame;
	double min_area;
	double max_area;
	double prom_area;
	double prom_ratio;
	double total_blobs;
	MultiGraph mpg;
		
	///Methods
	/**Simple contructor*/
	MultiPartiteGraph(int n_window = 2);
	
	/**
	 * @brief Insert a node into a graph
	 * @details Insert a node into a graph, if graph doesnt exist it creates
	 *          the corresponding graph with graph_id
	 * @param int graph_id: Unique identifier for a graph, asociated to the frame number
	 * @param Node node: Object node which is a blob
	 * */	
	void InsertNode(int graph_id, Node node);

	/**
	 * @brief Prints basic information of a multi-partite graph
	 * @details Prints the graph id and number of nodes.
	 **/
	void ShowMultiGraphInfo(void);
	
	/**
	 * @brief Get a graph identified by an id
	 * @details Returns a graph object identified by an id.
	 * @param int graph_id: Unique identifier for a graph, asociated to the frame number
	 * @return Graph graph: Object node which is a blob
	 **/	
	Graph GetGraph(int graph_id);
	
	/**
	 * @brief Get a node identified by an id within a graph
	 * @details Returns a node object identified by an id from a graph.
	 * @param int graph_id: Unique identifier for a graph in the multipartite graph,
	 * 					    associated to the frame number.
	 * @param int node_id: Unique identifier for a node within a graph.
	 * @return Node node: Object node which is a blob
	 **/		
	Node GetNode(int graph_id, int blob_id);
	Node DeleteNode(int graph_id, int blob_id);	
	
	/**
	 * @brief Compare node objects
	 * @details Compare node objects by their contour, histogram and positions.
	 * @param Node: node1
	 * @param Node: node2
	 * @return double result: Value which represent dissimilarity between nodes.
	 **/	
	double CompareNodes(Node node1, Node node2);

	/**
	 * @brief Compare node objects
	 * @details Compare node objects by their contour, histogram and positions.
	 * @param Node: node1
	 * @param Node: node2
	 * @return <std::string,double>: Vector made of hashes identifing contour, 
	 *                               hisrogram and position dissimilarity.
	 **/	
	std::map<std::string,double> CompareNodesProperties(Node node1, Node node2);

	/**
	 * @brief Compares graphs
	 * @details Compares two graphs made of player blobs finding the optimized
	 * 			relation between them using the hungarian algorithm
	 * @param int: graph id 1
	 * 	@param int: graph id 2
	 * @return std::vector<Edge>: A vector of edge objects representing the relationship
	 *                            between the node graphs.
	 **/	
	std::vector<Edge> CompareGraphs(int graphid1, int graphid2);
	std::vector<Edge> CompareGraphs(Graph graph1, Graph graph2);

	/**
	 * @brief Generate the edges between all nodes of two graphs
	 * @details Generates all the edges between nodes of two graphs, there is
	 *          no optimization or pruning of these edges.
	 * @param Graph: graph 1
	 * @param Graph: graph 2
	 * @return std::vector<Edge>: Vector of edge objects.
	 **/	
	std::vector<Edge> GenerateEdgesFromGraphs(Graph graph1, Graph graph2);

	/**
	 * @brief Set the player tag value to a given node within a graph
	 * @details ...
	 * @param ...
	 **/	
	void AddNodePlayertag(int graph_id, int node_id, int tag);
	void SetNodePlayertag(int graph_id, int node_id, int tag);
	void SetNodePlayerCluster(int graph_id, int node_id, int cluster);
	void SetNodeType(int graph_id, int node_id, int type);
	void SetNodePlayertagFromCoincidence(int graph_id, int node_id);
	void IncrementNodePlayertag(int graph_id, int node_id, int tag);
	void InsertNodePlayertagToBlob(int graph_id, int blob_id, int tag);
	void InsertNodePlayerTagCentroidToBlob(int graph_id, int blob_id, int tag, Point2f mcentroid);
	int  RemoveNodePlayertagFromBlob(int graph_id, int blob_id, int tag);
	void AddTagAndCentroid(int graph_id, int node_id, int tag, Point2f mcentroid);
	void SetTagAndCentroid(int graph_id, int node_id, int tag, Point2f mcentroid);
	void DeleteTagAndCentroid(int graph_id, int node_id, int tag);
	void ClearTagAndCentroids(int graph_id, int node_id);
	void ClearTagSet(int graph_id, int node_id);

	/**
	 * @brief Edge analyzer routine
	 * @details Analyze the edges of a MPG to detect oclusions/abductions 
	 * and solve them finding the association between merge and split events
	 * @param std::vector<Edge> min_edges: A vector of edges between adyacent graphs
	 * @param int direction: Direction to analyze the edges, foward to detect splits
	 * and backward merges
	 **/
	void AnalyzeEdges(std::vector<Edge> min_edges, int direction);
	void AnalyzeEdgesv2(std::vector<Edge> min_edges, int direction);
	void AnalyzeEdgesv3(std::vector<Edge> min_edges, int direction);
	void AnalyzeEdgesv4(std::vector<Edge> min_edges, std::map<std::string,int> mpgtparms,int direction);
	void ChangeBlobState(int graph_id, int blob_id, int state);
	void CopyNodePlayerTagSet(int graph_id, int node_id, set< int > tags_set);
	void CopyMergedCentroidsMap(int graph_id, int node_id, map< int,Point2f > cp_merged_centroids);
	int  RemoveTagFromPlayerTagSet(int graph_id, int node_id, int tag);
	void CopyNodePlayerPositionHistory(int graph_id, int node_id, std::array<Point2f,5> cp_position_history);
	void UpdateNodeDirection(int graph_id, int node_id);
	void UpdateNodeSpeed(int graph_id, int node_id);
	void UpdateNodeSpeed(int graph_id, int node_id, int frames);
	void ChangeNodeStartPointHistory(int graph_id, int node_id, Point2f new_start_point);

	/**
	 * @brief Simple solver to perform the tracking of soccer players
	 * @details ...
	 * @param ...
	 **/	
	void MVSimpleSolver(std::map<std::string,double> weights, std::map<std::string,int> mpgtparms, int step);
	void MVMPAssignFromFirstGraph(std::map<std::string,double> weights, int step);
	void MVExtendedSimpleSolver(std::map<std::string,double> weights, std::map<std::string,int> mpgtparms, int step);
	/**
	 * @brief Simple solve between two graphs
	 * @details Use a bipartite graph to make associations between adjacent multipartite graphs
	 * @param ...
	 **/		
	void SolveBorderBPG(std::map<std::string,double> weights, int step, Graph previous_graph);
	void SolveExtendedBorderBPG(std::map<std::string,double> weights, std::map<std::string,int> mpgtparms, int step, Graph previous_graph);

	void MVMPSolver(std::map<std::string,double> weights, int step);
	
	std::vector<Memoir> GetOclusionMemories(void);
	std::vector<Memoir> GetAbductionMemories(void);
	void SetOclusionMemories(std::vector<Memoir> om);
	void SetAbductionMemories(std::vector<Memoir> am);
	void AnalyzeAbductions(std::vector<Edge> min_edges, int source_frame_number,int direction);
	void PrintEdges(std::vector<Edge> back_min_edges);

	/**Simple distructor*/
	~MultiPartiteGraph();
};



#endif
