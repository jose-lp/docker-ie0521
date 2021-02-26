#include "MultiPartiteGraph.h"
#include "Defs.h"

using namespace cv;
using namespace std;


MultiPartiteGraph::MultiPartiteGraph(int n_window)
{
	number_of_graphs = 0;
	window_size = n_window;
	position_weight = 0.07;
	histogram_weight = 0.001;
	contour_weight = 0.001;
	area_weight = 0.00025;
	enable_oclusion_solver = 1;
	min_area = 9999999.9;
	max_area = 0.0;
	total_blobs = 0;
}

void MultiPartiteGraph::InsertGraph(int graph_id)
{
	mpg[graph_id];
	number_of_graphs++;
}

void MultiPartiteGraph::InsertGraph(int graph_id, Graph graph)
{
	mpg[graph_id] = graph;
	number_of_graphs++;
}


void MultiPartiteGraph::InsertNode(int graph_id, Node node)
{
	///Revisar cuantos grafos tienen el multi y si existe el graph con el id
	if((mpg.size()) > window_size)
		cout << "Multi graph is full, unable to add node." << endl;
	else if(!mpg.count(graph_id) && (mpg.size() <= window_size)){ ///Create graph if doesnt exist.
		//cout << "Creating graph " << graph_id << " and inserting first node" << endl;
		Graph tmp_graph;
		tmp_graph.push_back(node);
		InsertGraph(graph_id, tmp_graph);
		total_blobs++;
	}
	else{
		//cout << "Inserting node " << node.blob_id << endl;
		mpg[graph_id].push_back(node);		
		total_blobs++;
	}	
}

void MultiPartiteGraph::ShowMultiGraphInfo(void)
{
		cout << "\nMultiPartiteGraph info:" << endl;
		cout << "Number of graphs = " << mpg.size() << "\n" << endl;
		
		for(unsigned int i=1;i<=mpg.size();i++){
				cout << "\t Info for graph number: " << i << endl;
				cout << "\t Number of nodes = " << mpg[i-1].size() << "\n" << endl;
		}
		
}

Graph MultiPartiteGraph::GetGraph(int graph_id)
{
		//cout << "Tag set size of first element at GetGraph = " << mpg[graph_id].at(0).player_tag_set.size() << " with player tag = " << mpg[graph_id].at(0).player_tag << endl;
		return mpg[graph_id];
}

Node MultiPartiteGraph::GetNode(int graph_id, int blob_id)
{
	Graph gph = GetGraph(graph_id);
	//cout << "Tag set size of first element at GetNode = " << gph.at(0).player_tag_set.size() << " with player tag = " << gph.at(0).player_tag << endl;
	Node nd;
		for(unsigned int vnit = 0 ; vnit < gph.size(); ++vnit){
			if(gph[vnit].blob_id == blob_id){
				//cout << "Tag set size of requested element "<< vnit <<" at GetNode = " << gph.at(vnit).player_tag_set.size() << " with player tag = " << gph.at(vnit).player_tag << endl;
				nd = gph[vnit];
			}				
		}
		return nd;	
		//return mpg[graph_id].at(node_id);
}

Node MultiPartiteGraph::DeleteNode(int graph_id, int blob_id)
{
	Node nd;
	nd = GetNode(graph_id,blob_id);
	mpg[graph_id].erase(mpg[graph_id].begin()+blob_id);
	
	return nd;
}

double MultiPartiteGraph::CompareNodes(Node node1, Node node2)
{
		double delta_position=ComparePointsDistance(node1.centroid, node2.centroid);// cout << "Delta posicion = " << delta_position << endl;
		double delta_histogram=CompareHistograms(node1.histogram, node2.histogram);// cout << "Delta histogram = " << delta_histogram << endl;
		double delta_shape=CompareContours(node1.contorno, node2.contorno);// cout << "Delta contornos = " << delta_shape << endl;
		double delta_area=CompareAreas(node1.area, node2.area);
		
		double limit= (double) max_speed * (double) position_weight;
		
		if(delta_position > limit){ //31.75
			//cout << " dif pos = " << delta_position << " limit = " << limit <<" max_speed = " << max_speed << " position_weight = " << position_weight<<endl;
			delta_position = 9999999;
		}
		
		double weight = delta_position*position_weight+
		                delta_histogram*histogram_weight+
		                delta_shape*contour_weight+
		                delta_area*area_weight;
		return weight;
}

std::map<std::string,double> MultiPartiteGraph::CompareNodesProperties(Node node1, Node node2)
{
		double delta_position=ComparePointsDistance(node1.centroid, node2.centroid);
		double delta_histogram=CompareHistograms(node1.histogram, node2.histogram);
		double delta_shape=CompareContours(node1.contorno, node2.contorno);
		double delta_area=CompareAreas(node1.area, node2.area);
		
		std::map<std::string,double> deltas;
		deltas["position"]= delta_position; 
		deltas["histogram"]= delta_histogram;
		deltas["contour"]= delta_shape;
		deltas["area"]= delta_area;
		
		return deltas;	
}

std::vector<Edge> MultiPartiteGraph::CompareGraphs(int graphid1, int graphid2)
{	
	Node nodeu, nodev;		
	std::vector<Edge> edges;
	int ubid = 0; int vbid = 0;
	
	Graph graph1,graph2;
	graph1 = GetGraph(graphid1);
	graph2 = GetGraph(graphid2);
	
	for(unsigned int u=0; u < graph1.size(); u++){
		
		double minval = 9999999999;
		Edge uvedge;
		int converge = NO;
		
		for(unsigned int v=0; v < graph2.size(); v++){
			nodeu = graph1[u];
			nodev = graph2[v];
/*			
			cout << "Comparing blob player " << nodeu.blob_id << " of frame " << nodeu.blob_frame_number  
			     << " with blob player " << nodev.blob_id << " of frame " << nodev.blob_frame_number;
*/
//			double retval = CompareNodes(nodeu,nodev);
			std::map<std::string,double> deltas;
			deltas = CompareNodesProperties(nodeu,nodev);
			double limit= (double) max_speed * (double) position_weight;
		
			double retval = deltas["position"]*position_weight+
							deltas["histogram"]*histogram_weight+
							deltas["shape"]*contour_weight+
							deltas["area"]*area_weight;

//			cout << " Function value = " << retval << endl;
			
			//minval = (minval < retval) ? minval : retval;
			if(minval < retval){
				minval = minval;
				ubid = nodeu.blob_id;
				vbid = nodev.blob_id;				
				//cout << "No min value of "<< minval <<" between ubid = " << uvedge.ubid << " and vbid = " << uvedge.vbid << endl;
			}
			else if(deltas["position"] < limit){
				minval = retval;
				ubid = nodeu.blob_id;
				vbid = nodev.blob_id;
				uvedge.ubid=ubid;
				uvedge.vbid=vbid;
				uvedge.uframe=graphid1;
				uvedge.vframe=graphid2;
				uvedge.weight=minval;
				converge = YES;
				//cout << "Min value found of "<< minval <<" between ubid = " << uvedge.ubid << " and vbid = " << uvedge.vbid << endl;
				//cout << "Delta position = " << deltas["position"] << " weight = " << minval << " limit = " << limit << endl;
			}			
		}

		if(converge==YES){
/*
			ubid = nodeu.blob_id;// + " / " + std::to_string(nodeu.blob_frame_number);
			vbid = nodev.blob_id;// + " / " + std::to_string(nodev.blob_frame_number);
			cout << "Min value of "<< uvedge.weight <<" between ubid = " << uvedge.ubid << " and vbid = " << uvedge.vbid;
			cout << " Graph1 size = " << graph1.size() << " Graph2 size = " << graph2.size() << endl;
*/			
			edges.push_back(uvedge);
			converge = NO;
		}
	}
	return edges;
}

std::vector<Edge> MultiPartiteGraph::CompareGraphs(Graph graph1, Graph graph2)
{	
	Node nodeu, nodev;
	std::vector<Edge> edges;
	int ubid = 0; int vbid = 0;
	
	for(unsigned int u=0; u < graph1.size(); u++){
		
		double minval = 9999999999;
		Edge uvedge;
		int converge = NO;
		
		for(unsigned int v=0; v < graph2.size(); v++){
			nodeu = graph1[u];
			nodev = graph2[v];
/*			
			cout << "Comparing blob player " << nodeu.blob_id << " of frame " << nodeu.blob_frame_number  
			     << " with blob player " << nodev.blob_id << " of frame " << nodev.blob_frame_number;
*/
//			double retval = CompareNodes(nodeu,nodev);
			std::map<std::string,double> deltas;
			deltas = CompareNodesProperties(nodeu,nodev);
			double limit= (double) max_speed * (double) position_weight;
		
			double retval = deltas["position"]*position_weight+
							deltas["histogram"]*histogram_weight+
							deltas["shape"]*contour_weight+
							deltas["area"]*area_weight;

//			cout << " Function value = " << retval << endl;
			
			//minval = (minval < retval) ? minval : retval;
			if(minval < retval){
				minval = minval;
				ubid = nodeu.blob_id;
				vbid = nodev.blob_id;				
				//cout << "No min value of "<< minval <<" between ubid = " << uvedge.ubid << " and vbid = " << uvedge.vbid << endl;
			}
			else if(deltas["position"] < limit){
				minval = retval;
				ubid = nodeu.blob_id;
				vbid = nodev.blob_id;
				uvedge.ubid=ubid;
				uvedge.vbid=vbid;
				uvedge.uframe=graph1[u].blob_frame_number;;
				uvedge.vframe=graph2[v].blob_frame_number;
				uvedge.weight=minval;
				converge = YES;
				//cout << "Min value found of "<< minval <<" between ubid = " << uvedge.ubid << " and vbid = " << uvedge.vbid << endl;
				//cout << "Delta position = " << deltas["position"] << " weight = " << minval << " limit = " << limit << endl;
			}			
		}

		if(converge==YES){
/*
			ubid = nodeu.blob_id;// + " / " + std::to_string(nodeu.blob_frame_number);
			vbid = nodev.blob_id;// + " / " + std::to_string(nodev.blob_frame_number);
			cout << "Min value of "<< uvedge.weight <<" between ubid = " << uvedge.ubid << " and vbid = " << uvedge.vbid;
			cout << " Graph1 size = " << graph1.size() << " Graph2 size = " << graph2.size() << endl;
*/			
			edges.push_back(uvedge);
			converge = NO;
		}
	}
	return edges;
}


std::vector<Edge> MultiPartiteGraph::GenerateEdgesFromGraphs(Graph graph1, Graph graph2)
{	
	Node nodeu, nodev;
	Edge uvedge;
	//double minval = INF;
	std::vector<Edge> edges;
	
	for(unsigned int u=0; u < graph1.size(); u++){
		for(unsigned int v=0; v < graph2.size(); v++){
			nodeu = graph1[u];
			nodev = graph2[v];
			double retval = CompareNodes(nodeu,nodev);
			int uid = nodeu.blob_id;
			int vid = nodev.blob_id;
			//uvedge[std::make_pair(uid, vid)] = retval;
			uvedge.ubid=uid; uvedge.vbid=vid; uvedge.weight=retval;
			edges.push_back(uvedge);
		}	
	}
	return edges;
}

void MultiPartiteGraph::AddNodePlayertag(int graph_id, int node_id, int tag)
{
	
	mpg[graph_id].at(node_id).player_tag_set.insert(tag);
	
}

void MultiPartiteGraph::SetNodePlayertag(int graph_id, int node_id, int tag)
{
	mpg[graph_id].at(node_id).player_tag=tag;
	//cout << "Previous on set node tag set size = " << mpg[graph_id].at(node_id).player_tag_set.size() << endl;
	mpg[graph_id].at(node_id).player_tag_set.clear();
	mpg[graph_id].at(node_id).player_tag_set.insert(tag);
	//cout << "After on set node insert tag set size = " << mpg[graph_id].at(node_id).player_tag_set.size() << endl;
}

void MultiPartiteGraph::AddTagAndCentroid(int graph_id, int node_id, int tag, Point2f mcentroid)
{
	mpg[graph_id].at(node_id).merged_centroids.insert(std::make_pair(tag, mcentroid));
}

void MultiPartiteGraph::SetTagAndCentroid(int graph_id, int node_id, int tag, Point2f mcentroid)
{
	mpg[graph_id].at(node_id).merged_centroids.clear();
	mpg[graph_id].at(node_id).merged_centroids.insert(std::make_pair(tag, mcentroid));
}

void MultiPartiteGraph::DeleteTagAndCentroid(int graph_id, int node_id, int tag)
{
	for (std::map< int,Point2f >::iterator tcit=mpg[graph_id].at(node_id).merged_centroids.begin(); 
	tcit!=mpg[graph_id].at(node_id).merged_centroids.end(); ++tcit)
		if(tcit->first == tag)
			mpg[graph_id].at(node_id).merged_centroids.erase(tcit);
}

void MultiPartiteGraph::ClearTagAndCentroids(int graph_id, int node_id)
{
	mpg[graph_id].at(node_id).merged_centroids.clear();
}

void MultiPartiteGraph::ClearTagSet(int graph_id, int node_id)
{
	mpg[graph_id].at(node_id).player_tag_set.clear();
}

void MultiPartiteGraph::SetNodePlayerCluster(int graph_id, int node_id, int cluster)
{
	mpg[graph_id].at(node_id).cluster=cluster;
	//cout << "Cluster " << cluster << " set to node " << node_id << " from graph " << graph_id << endl;
}

void MultiPartiteGraph::SetNodeType(int graph_id, int node_id, int type)
{
	mpg[graph_id].at(node_id).blob_type=type;
}

void MultiPartiteGraph::CopyNodePlayerTagSet(int graph_id, int node_id, set< int > tags_set)
{
	mpg[graph_id].at(node_id).player_tag_set=tags_set;
}

void MultiPartiteGraph::CopyMergedCentroidsMap(int graph_id, int node_id, map< int,Point2f > cp_merged_centroids)
{
	mpg[graph_id].at(node_id).merged_centroids.clear();
	mpg[graph_id].at(node_id).merged_centroids=cp_merged_centroids;
}

void MultiPartiteGraph::CopyNodePlayerPositionHistory(int graph_id, int node_id, std::array<Point2f,5> cp_position_history)
{
/*
	cout << "Previous node position history: " << endl;
	for(unsigned int pni=0;pni<5;pni++)
		cout << "Position " << pni << " point x = " << mpg[graph_id].at(node_id).position_history[pni].x << " y = " << mpg[graph_id].at(node_id).position_history[pni].y << endl;
	cout << "Passed node position history: " << endl;
	for(unsigned int cni=0;cni<5;cni++)
		cout << "Position " << cni << " point x = " << cp_position_history[cni].x << " y = " << cp_position_history[cni].y << endl;
*/	
	for(unsigned int pcni=0;pcni<5;pcni++)
		mpg[graph_id].at(node_id).position_history[pcni]=cp_position_history[pcni];
/*
	cout << "After copy"<< endl;
	for(unsigned int pni=0;pni<5;pni++)
		cout << "Position " << pni << " point x = " << mpg[graph_id].at(node_id).position_history[pni].x << " y = " << mpg[graph_id].at(node_id).position_history[pni].y << endl;	
*/
}

void MultiPartiteGraph::UpdateNodeSpeed(int graph_id, int node_id)
{
	mpg[graph_id].at(node_id).UpdateSpeed();
}

void MultiPartiteGraph::UpdateNodeSpeed(int graph_id, int node_id, int frames)
{
	mpg[graph_id].at(node_id).UpdateSpeed(frames);
}

void MultiPartiteGraph::UpdateNodeDirection(int graph_id, int node_id)
{
	mpg[graph_id].at(node_id).UpdateDirection();
}

void MultiPartiteGraph::ChangeNodeStartPointHistory(int graph_id, int node_id, Point2f new_start_point)
{
	mpg[graph_id].at(node_id).ChangeBlobStartPointHistory(new_start_point);
}

void MultiPartiteGraph::SetNodePlayertagFromCoincidence(int graph_id, int node_id)
{
	map<int, int> m = mpg[graph_id].at(node_id).player_tags;
	
	///Lambda, use c++11
	auto x = std::max_element(m.begin(), m.end(),
    [](const pair<int, int>& p1, const pair<int, int>& p2) {
        return p1.second < p2.second; });
        
    SetNodePlayertag(graph_id, node_id, x->first);
        
}

void MultiPartiteGraph::IncrementNodePlayertag(int graph_id, int node_id, int tag)
{
	if (mpg[graph_id].at(node_id).player_tags.count(tag)>1)
		mpg[graph_id].at(node_id).player_tags[tag]+=1;
	else
		mpg[graph_id].at(node_id).player_tags[tag]=1;
}

void MultiPartiteGraph::InsertNodePlayertagToBlob(int graph_id, int blob_id, int tag)
{
	Graph gph = GetGraph(graph_id);
	int node_id=505;

	///Determine the node_id searching the blob_id because node_id is 
	///the position in the node vector, which is the blob_id
	for(unsigned int vnit = 0 ; vnit < gph.size(); ++vnit){
		if(gph[vnit].blob_id == blob_id)
			node_id=vnit;
	}

	mpg[graph_id].at(node_id).player_tag_set.insert(tag);
}

void MultiPartiteGraph::InsertNodePlayerTagCentroidToBlob(int graph_id, int blob_id, int tag, Point2f mcentroid)
{
	Graph gph = GetGraph(graph_id);
	int node_id=505;

	///Determine the node_id searching the blob_id because node_id is 
	///the position in the node vector, which is the blob_id
	for(unsigned int vnit = 0 ; vnit < gph.size(); ++vnit){
		if(gph[vnit].blob_id == blob_id)
			node_id=vnit;
	}

	mpg[graph_id].at(node_id).merged_centroids.insert(std::make_pair(tag, mcentroid));
}

void MultiPartiteGraph::ChangeBlobState(int graph_id, int blob_id, int state)
{
	Graph gph = GetGraph(graph_id);
	int node_id=504;

	///Determine the node_id searching the blob_id because node_id is 
	///the position in the node vector, which is the blob_id
	for(unsigned int vnit = 0 ; vnit < gph.size(); ++vnit){
		if(gph[vnit].blob_id == blob_id)
			node_id=vnit;
	}

	mpg[graph_id].at(node_id).blob_type=state;
}

int MultiPartiteGraph::RemoveNodePlayertagFromBlob(int graph_id, int blob_id, int tag)
{
	Graph gph = GetGraph(graph_id);
	int node_id=503;
	cout << "Removing blob " << blob_id << " from frame " << graph_id << " with tag " << tag << endl;

	///Determine the node_id searching the blob_id because node_id is 
	///the position in the node vector, which is the blob_id
	for(unsigned int vnit = 0 ; vnit < gph.size(); ++vnit){
		if(gph[vnit].blob_id == blob_id)
			node_id=vnit;
	}

	int retval = mpg[graph_id].at(node_id).player_tag_set.erase(tag);
	
	return retval;
}

int MultiPartiteGraph::RemoveTagFromPlayerTagSet(int graph_id, int node_id, int tag)
{
	int retval = mpg[graph_id].at(node_id).player_tag_set.erase(tag);
	return retval;
}


void MultiPartiteGraph::MVSimpleSolver(std::map<std::string,double> weights, std::map<std::string,int> mpgtparms, int step)
{
	position_weight = weights["position"];
	histogram_weight = weights["histogram"];
	contour_weight = weights["contour"];
	area_weight = weights["area"];
	max_speed = weights["max_speed"];
	max_merge_distance = mpgtparms["max_merge_distance"];
	enable_boundary_blob_remover = mpgtparms["enable_boundary_blob_remover"];
	oclusion_memoir.InitMemoir(mpgtparms,weights);
	
	D(cout << "Doing a simple solver on mpg." << endl;)
	
	/*Reduce number of borders*/
	for(unsigned int i=0; i< window_size-1; i++){
		int frame_number = i+step;
		
		Graph graphn1,graphn2;
		graphn1 = GetGraph(frame_number);
		graphn2 = GetGraph(frame_number+1);
		
		D(cout << "\nEvents between "  << frame_number << " and " << frame_number+1 << endl;)
		std::vector<Edge> back_min_edges=CompareGraphs(frame_number,frame_number+1);
		D(cout << " -Merges" << endl;)
		AnalyzeEdges(back_min_edges,BACK);
/*		
		for(unsigned int iedge = 0; iedge < back_min_edges.size(); iedge++){
			cout << "Source blob = " << back_min_edges[iedge].ubid << " destination blob = " << back_min_edges[iedge].vbid;
			cout << " Weight = " << back_min_edges[iedge].weight << endl;
		}
		
*/		
///		std::vector<Edge> front_min_edges=CompareGraphs(frame_number+1,frame_number);
		cout << " -Splits" << endl;
///		AnalyzeEdges(front_min_edges,FRONT);
		
		///Propagate player tag
			for(unsigned int en = 0; en < back_min_edges.size(); en++){
				Node nodeu, nodev;
				Edge single_edge = back_min_edges[en];
				
/*				
				cout << "Node U id = " << single_edge.ubid << " and node V id = " << single_edge.vbid 
				<< " for edge " << en << " with weight = " << single_edge.weight <<endl;
*/				
				
				
				for(unsigned int u=0; u < graphn1.size(); u++){
					for(unsigned int v=0; v < graphn2.size(); v++){
						nodeu=graphn1[u];
						nodev=graphn2[v];
						
						if((nodeu.blob_id==single_edge.ubid) && (nodev.blob_id==single_edge.vbid)){

							//nodev.player_tag=nodeu.player_tag;
							//nodev.blob_labels=nodeu.blob_labels;
							//mpg[frame_number+1].at(v).player_tag=nodeu.player_tag;
							SetNodePlayertag(frame_number+1,v,nodeu.player_tag);
/*							
							cout << "Found pair for a border between blob u " << nodeu.blob_id << "\t with player tag " << nodeu.player_tag
							                                << "\t and blob v " << nodev.blob_id << "\t with player tag " << nodev.player_tag 
							                                << endl;
*/							                                
						}
					}
				}
				
			}
			
	}


	
}

void MultiPartiteGraph::MVExtendedSimpleSolver(std::map<std::string,double> weights, std::map<std::string,int> mpgtparms, int step)
{
	position_weight = weights["position"];
	histogram_weight = weights["histogram"];
	contour_weight = weights["contour"];
	area_weight = weights["area"];
	max_speed = weights["max_speed"];
	max_merge_distance = mpgtparms["max_merge_distance"];
	enable_boundary_blob_remover = mpgtparms["enable_boundary_blob_remover"];
	enable_oclusion_solver = mpgtparms["enable_oclusion_solver"];
	oclusion_memoir.InitMemoir(mpgtparms,weights);
	abduction_memoir.InitMemoir(mpgtparms,weights);
	
	D(cout << "Doing a extended simple solver on mpg." << endl;)
	
	/*Reduce number of borders*/
	for(unsigned int i=0; i< window_size-1; i++){
		int frame_number = i+step;
		
		Graph graphn1,graphn2;
		graphn1 = GetGraph(frame_number);
		graphn2 = GetGraph(frame_number+1);
		
		cout << "\n--- Events between "  << frame_number << " and " << frame_number+1 << " ---" << endl;
		std::vector<Edge> back_min_edges=CompareGraphs(frame_number,frame_number+1);
		D(cout << " -Merges" << endl;)
		
		///Propagate player tag
		for(unsigned int en = 0; en < back_min_edges.size(); en++){
			Node nodeu, nodev;
			Edge single_edge = back_min_edges[en];
						
			for(unsigned int u=0; u < graphn1.size(); u++){
				for(unsigned int v=0; v < graphn2.size(); v++){
					nodeu=graphn1[u];
					nodev=graphn2[v];
						
					if((nodeu.blob_id==single_edge.ubid) && (nodev.blob_id==single_edge.vbid)){
						//Node nodevtmp=GetNode(frame_number+1,v);
						//cout << "Previous tag set size = " << nodevtmp.player_tag_set.size() << endl;
						//nodevtmp=GetNode(frame_number+1,v);
						//cout << "After tag set size = " << nodevtmp.player_tag_set.size() << endl;
						CopyNodePlayerTagSet(frame_number+1,v,nodeu.player_tag_set); ///To propagate the players contained on a blob
						if(nodeu.merged_centroids.size()==1)
							CopyMergedCentroidsMap(frame_number+1,v,nodeu.merged_centroids); 
						SetNodePlayertag(frame_number+1,v,nodeu.player_tag); ///Set the player id
						AddTagAndCentroid(frame_number+1,v, nodeu.player_tag,nodeu.centroid); ///Set the player id and the centroid (mostly for mergerd blobs)						
						CopyNodePlayerPositionHistory(frame_number+1,v,nodeu.position_history); ///To update position history, used to compute the direction
						UpdateNodeDirection(frame_number+1,v); ///Update player direction depending on previous positions
						UpdateNodeSpeed(frame_number+1,v);

					}
				}
			}		
		}

		///Abduction solver
		if(mpgtparms["enable_abduction_finder"]){
			AnalyzeAbductions(back_min_edges,frame_number,BACK);
			abduction_memoir.DegradeMemoirs();
		}
		
		///Oclusion solver
		if(mpgtparms["enable_oclusion_solver"]==VERSION1){
			AnalyzeEdges(back_min_edges,BACK);	///Check and solve merges
		}
		else if(mpgtparms["enable_oclusion_solver"]==VERSION2){
			AnalyzeEdgesv2(back_min_edges,BACK);	///Check and solve merges
		}
		else if(mpgtparms["enable_oclusion_solver"]==VERSION3){
			AnalyzeEdgesv3(back_min_edges,BACK);	///Check and solve merges
		}
		else if(mpgtparms["enable_oclusion_solver"]==VERSION4){
			AnalyzeEdgesv4(back_min_edges,mpgtparms,BACK);	///Check and solve merges
		}

		std::vector<Edge> front_min_edges=CompareGraphs(frame_number+1,frame_number);
		D(cout << " -Splits" << endl;)

		if(mpgtparms["enable_oclusion_solver"]==VERSION1){
			AnalyzeEdges(front_min_edges,FRONT);	///Check and solve splits
			oclusion_memoir.DegradeMemoirs();
		}
		else if(mpgtparms["enable_oclusion_solver"]==VERSION2){
			AnalyzeEdgesv2(front_min_edges,FRONT);	///Check and solve splits
			oclusion_memoir.DegradeMemoirs();
		}
		else if(mpgtparms["enable_oclusion_solver"]==VERSION3){
			AnalyzeEdgesv3(front_min_edges,FRONT);	///Check and solve splits
			oclusion_memoir.DegradeMemoirs();
		}
		else if(mpgtparms["enable_oclusion_solver"]==VERSION4){
			AnalyzeEdgesv4(front_min_edges,mpgtparms,FRONT);	///Check and solve splits
			oclusion_memoir.DegradeMemoirs();
		}
		
		if(mpgtparms["enable_abduction_finder"]){
			AnalyzeAbductions(front_min_edges,frame_number,FRONT);
			abduction_memoir.DegradeMemoirs();
		}
		
	}
}

void MultiPartiteGraph::MVMPSolver(std::map<std::string,double> weights, int step)
{
	position_weight = weights["position"];
	histogram_weight = weights["histogram"];
	contour_weight = weights["contour"];
	area_weight = weights["area"];
	max_speed = weights["max_speed"];
	
	D(cout << "Doing a multipartite solver using a max coincidence criterion on mpg." << endl;)
	
	/*All against all*/
	for(unsigned int i=0; i< window_size; i++){
		for(unsigned int j=1+i; j< window_size; j++){
		
			Graph graphu,graphv;
			graphu = GetGraph(step+i);
			graphv = GetGraph(step+j);
		
			///Generate the edges between all frames
			std::vector<Edge> min_edges=CompareGraphs(graphu,graphv);
		
			///Create a propagation histogram 
			for(unsigned int en = 0; en < min_edges.size(); en++){
				Node nodeu, nodev;
				Edge single_edge = min_edges[en];
		
				for(unsigned int u=0; u < graphu.size(); u++){
					for(unsigned int v=0; v < graphv.size(); v++){
						nodeu=graphu[u];
						nodev=graphv[v];
						
						if((nodeu.blob_id==single_edge.ubid) && (nodev.blob_id==single_edge.vbid)){

							SetNodePlayertag(step+j,v,nodeu.player_tag);
							IncrementNodePlayertag(step+j,v,nodeu.player_tag);
						/*	
							cout << "Found pair for a border between blob u " << nodeu.blob_id << "\t with player tag " << nodeu.player_tag
							                                << "\t and blob v " << nodev.blob_id << "\t with player tag " << nodev.player_tag 
							                                << endl;
						*/	  					                                
						}
					}
				}
			}
		}	
	}
	
	///Scan all nodes to assign player tag depending on number of coincidences
	for(unsigned int i=1; i< window_size; i++){
		
		Graph graphn;
		graphn = GetGraph(step+i);
			
		for(unsigned int node=0; node < graphn.size(); node++){
			SetNodePlayertagFromCoincidence(step+i, node);
		}					
	}	
}

void MultiPartiteGraph::MVMPAssignFromFirstGraph(std::map<std::string,double> weights, int step)
{
	position_weight = weights["position"];
	histogram_weight = weights["histogram"];
	contour_weight = weights["contour"];
	area_weight = weights["area"];
	max_speed = weights["max_speed"];
	
	D(cout << "Doing a multipartite assigner from the first graph with the rest on the MPG." << endl;)
	
	/*All against all*/
	for(unsigned int j=1; j< window_size; j++){
		
		Graph graphu,graphv;
		graphu = GetGraph(step);
		graphv = GetGraph(step+j);
		
		///Generate the edges between all frames with the first graph
		std::vector<Edge> min_edges=CompareGraphs(graphu,graphv);
		
		///Create a coincidence matrix 
		for(unsigned int en = 0; en < min_edges.size(); en++){
			Node nodeu, nodev;
			Edge single_edge = min_edges[en];
		
			for(unsigned int u=0; u < graphu.size(); u++){
				for(unsigned int v=0; v < graphv.size(); v++){
					nodeu=graphu[u];
					nodev=graphv[v];
						
					if((nodeu.blob_id==single_edge.ubid) && (nodev.blob_id==single_edge.vbid)){
						SetNodePlayertag(step+j,v,nodeu.player_tag);
  					    AddTagAndCentroid(step+j,v,nodeu.player_tag,nodev.centroid);                           
					}
				}
			}
		}
	}		
}

void MultiPartiteGraph::SolveBorderBPG(std::map<std::string,double> weights, int step, Graph previous_graph)
{
	position_weight = weights["position"];
	histogram_weight = weights["histogram"];
	contour_weight = weights["contour"];
	area_weight = weights["area"];
	max_speed = weights["max_speed"];
	
	/*Reduce number of borders*/
	int frame_number = step;
	
	//cout << "Solving bipartite graph between borders" << endl;
		
	Graph second_graph;
	second_graph = GetGraph(frame_number);

	cout << "\nSolving bipartite graph between borders: " << previous_graph[0].blob_frame_number 
	     << " - " << second_graph[0].blob_frame_number << endl;
	
			
	std::vector<Edge> min_edges=CompareGraphs(previous_graph,second_graph);
/*
		for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
			cout << "Source blob = " << min_edges[iedge].ubid << " destination blob = " << min_edges[iedge].vbid;
			cout << " Weight = " << min_edges[iedge].weight << endl;
		}	
*/		
			for(unsigned int en = 0; en < min_edges.size(); en++){
				Node nodeu, nodev;
				Edge single_edge = min_edges[en];
				
/*				
				cout << "Node U id = " << single_edge.ubid << " and node V id = " << single_edge.vbid 
				<< " for edge " << en << " with weight = " << single_edge.weight <<endl;
*/				
				
				
				for(unsigned int u=0; u < previous_graph.size(); u++){
					for(unsigned int v=0; v < second_graph.size(); v++){
						nodeu=previous_graph[u];
						nodev=second_graph[v];
						
						if((nodeu.blob_id==single_edge.ubid) && (nodev.blob_id==single_edge.vbid)){

							//nodev.player_tag=nodeu.player_tag;
							//nodev.blob_labels=nodeu.blob_labels;
							//mpg[frame_number+1].at(v).player_tag=nodeu.player_tag;
							///SetNodePlayertag(frame_number,v,nodeu.player_tag);
/*							
							cout << "Found pair for a border between blob u " << nodeu.blob_id << "\t with player tag " << nodeu.player_tag
							                                << "\t and blob v " << nodev.blob_id << "\t with player tag " << nodev.player_tag 
							                                << endl;
*/
						}
					}
				}
				
			}
			AnalyzeEdges(min_edges,BACK);	///Check for merges	
	
}


void MultiPartiteGraph::SolveExtendedBorderBPG(std::map<std::string,double> weights,std::map<std::string,int> mpgtparms,int step, Graph previous_graph)
{
	position_weight = weights["position"];
	histogram_weight = weights["histogram"];
	contour_weight = weights["contour"];
	area_weight = weights["area"];
	max_speed = weights["max_speed"];
	max_merge_distance = mpgtparms["max_merge_distance"];
	enable_boundary_blob_remover = mpgtparms["enable_boundary_blob_remover"];
	enable_oclusion_solver = mpgtparms["enable_oclusion_solver"];
	oclusion_memoir.InitMemoir(mpgtparms,weights);
	abduction_memoir.InitMemoir(mpgtparms,weights);
	
	/*Reduce number of borders*/
	int frame_number = step;
	
	InsertGraph(frame_number-1, previous_graph);
	
	Graph graphn1,graphn2;
	graphn1 = GetGraph(frame_number-1);
	graphn2 = GetGraph(frame_number);	
	

	cout << "\nSolving with extended bipartite graph between borders: " << graphn1[0].blob_frame_number 
	     << " - " << graphn2[0].blob_frame_number << endl;
	
	//cout << "previous graph size = " << previous_graph.size() << " second graph size = " << second_graph.size() << endl;
			
	//std::vector<Edge> back_min_edges=CompareGraphs(previous_graph,second_graph); //cout << "back min edges size: " << back_min_edges.size() << endl;
	std::vector<Edge> back_min_edges=CompareGraphs(frame_number-1,frame_number);
	//std::vector<Edge> front_min_edges=CompareGraphs(second_graph,previous_graph); //cout << "front min edges size: " << front_min_edges.size() << endl;
	std::vector<Edge> front_min_edges=CompareGraphs(frame_number,frame_number-1);	

/*	
	cout << "Inspecting border merge edges" << endl;
	PrintEdges(back_min_edges);
	cout << "Inspecting border split edges" << endl;
	PrintEdges(front_min_edges);
*/	
		
	for(unsigned int en = 0; en < back_min_edges.size(); en++){
		Node nodeu, nodev;
		Edge single_edge = back_min_edges[en];			
				
		for(unsigned int u=0; u < graphn1.size(); u++){
			for(unsigned int v=0; v < graphn2.size(); v++){
				nodeu=graphn1[u];
				nodev=graphn2[v];
				
				if((nodeu.blob_id==single_edge.ubid) && (nodev.blob_id==single_edge.vbid)){					
					CopyNodePlayerTagSet(frame_number,v,nodeu.player_tag_set);
					if(nodeu.merged_centroids.size()==1)
						CopyMergedCentroidsMap(frame_number,v, nodeu.merged_centroids);
					SetNodePlayertag(frame_number,v,nodeu.player_tag);
					AddTagAndCentroid(frame_number,v, nodeu.player_tag,nodeu.centroid);
					CopyNodePlayerPositionHistory(frame_number,v,nodeu.position_history); 
					UpdateNodeDirection(frame_number,v); ///Update player direction depending on previous positions
					UpdateNodeSpeed(frame_number,v);
					
				}
			}
		}
	}
	
	///Abduction solver
	if(mpgtparms["enable_abduction_finder"]){
		AnalyzeAbductions(back_min_edges,frame_number,BACK);
		abduction_memoir.DegradeMemoirs();
	}	
		
	///Oclusion solver
	if(mpgtparms["enable_oclusion_solver"]==VERSION1){
		AnalyzeEdges(back_min_edges,BACK);	///Check and solve merges
	}
	else if(mpgtparms["enable_oclusion_solver"]==VERSION2){
		AnalyzeEdgesv2(back_min_edges,BACK);	///Check and solve merges
	}
	else if(mpgtparms["enable_oclusion_solver"]==VERSION3){
		AnalyzeEdgesv3(back_min_edges,BACK);	///Check and solve merges
	}
	else if(mpgtparms["enable_oclusion_solver"]==VERSION4){			
		AnalyzeEdgesv4(back_min_edges,mpgtparms,BACK);	///Check and solve merges		
	}
		
	if(mpgtparms["enable_abduction_finder"]){
		AnalyzeAbductions(front_min_edges,frame_number,FRONT);
		abduction_memoir.DegradeMemoirs();
	}

	if(mpgtparms["enable_oclusion_solver"]==VERSION1){
		AnalyzeEdges(front_min_edges,FRONT);	///Check and solve splits
		oclusion_memoir.DegradeMemoirs();
	}
	else if(mpgtparms["enable_oclusion_solver"]==VERSION2){
		AnalyzeEdgesv2(front_min_edges,FRONT);	///Check and solve splits
		oclusion_memoir.DegradeMemoirs();
	}
	else if(mpgtparms["enable_oclusion_solver"]==VERSION3){
		AnalyzeEdgesv3(front_min_edges,FRONT);	///Check and solve splits
		oclusion_memoir.DegradeMemoirs();
	}
	else if(mpgtparms["enable_oclusion_solver"]==VERSION4){
		AnalyzeEdgesv4(front_min_edges,mpgtparms,FRONT);	///Check and solve splits
		oclusion_memoir.DegradeMemoirs();
	}		
		
}


void MultiPartiteGraph::AnalyzeEdges(std::vector<Edge> min_edges, int direction)
{
	///Figure it out if there is any event
	std::map< int, std::set<int> > oclusion_map; //In the form of destination to a list of sources
	
	///Create an event/oclusion map
	for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
		int source_blob_id = min_edges[iedge].ubid;
		int destination_blob_id = min_edges[iedge].vbid;
		//cout << "Source blob = " << source_blob_id << " destination blob = " << destination_blob_id;
		//cout << " Weight = " << min_edges[iedge].weight << endl;
		oclusion_map[destination_blob_id].insert(source_blob_id);
	}
	
	///Inspect merge/split oclusion map
	std::map<int,std::set<int>>::iterator omit;
	for (omit=oclusion_map.begin(); omit!=oclusion_map.end(); ++omit){
		if(omit->second.size()>1){///Process only detected merges/splits
			if(direction==BACK){///Merges
			
				std::cout << "--Merge v1--" << endl;
				std::cout << "From " ;
			
				for (auto sourcev : omit->second)
					std::cout << sourcev << " ";
		
				std::cout << "to " << omit->first << endl;
				
				///Search the frame number for source blob id
				std::vector<Edge>::iterator edgesit;;
				double best_blob_fit_weight=9999999;
				
				for (auto sourcev : omit->second) ///Sweep through the sources of the merge event
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){///Search on borders to match the sourcev id
						if(min_edges[iedge].ubid==sourcev){
							cout << "Trying to remember blob " << min_edges[iedge].ubid << " from frame " << min_edges[iedge].uframe << endl;
							Node to_remember = GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);
							cout << "Tag set size of node to remenber = " << to_remember.player_tag_set.size() << " with tag = " << to_remember.player_tag << " and blobid = " <<  to_remember.blob_id << endl;
							//oclusion_memoir.Remember(to_remember); //Add node to short term memory, both single and merged
							if(to_remember.player_tag_set.size()<=1){///If is a individual blob thats is going to merge
								oclusion_memoir.Remember(to_remember);//Remember only single blobs
								cout << "Inserting single player tag " << to_remember.player_tag << " to set and changing state to blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;
								InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag); //Add player tag to list.
							}

							else{///If the blob is already a merged blob, copy their tags to the new merged blob, in the if append the new single blob
								oclusion_memoir.Remember(to_remember); ///Coment to avoid remembering merged blobs, but this helps to recover splits without merges
								cout << "Node is a merged one with "<< to_remember.player_tag_set.size() << " tags, inserting player tags to the next merged blob"<<endl;							
								InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag); //Add player tag to list.

								for (std::set<int>::iterator tsit=to_remember.player_tag_set.begin(); tsit!=to_remember.player_tag_set.end(); ++tsit){
									//cout << "Inserting player tag " << *tsit << " to set and changing state to blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;
									InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,*tsit);									
								}

							}

							ChangeBlobState(min_edges[iedge].vframe,min_edges[iedge].vbid,MULTI_PLAYER);
							
							///Assign to the merged blob the id of the most likely blob
							if(min_edges[iedge].weight < best_blob_fit_weight){
								best_blob_fit_weight=min_edges[iedge].weight;
								SetNodePlayertag(min_edges[iedge].vframe, min_edges[iedge].vbid, to_remember.player_tag);
								AddTagAndCentroid(min_edges[iedge].vframe,min_edges[iedge].vbid, to_remember.player_tag,to_remember.centroid);

								cout << "SetNodePlayertag of " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe 
								     << " to " << to_remember.player_tag << " with weight " << min_edges[iedge].weight << endl;

							}
						}
				}
			}
			else if(direction==FRONT){///Splits

				std::cout << "--Split v1--" << endl;
				std::cout << "From " << omit->first << " to ";			
				for (auto sourcev : omit->second)
					std::cout << sourcev << " ";				
				std::cout << endl;			
				
				///Search the frame number for source blob id
				std::vector<Edge>::iterator edgesit;
				//double best_blob_fit_weight=9999999;
				
				for (auto sourcev : omit->second)
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
						if(min_edges[iedge].ubid==sourcev){
							cout << "Trying to recover blob " << min_edges[iedge].ubid << " from oclusion memoir." << endl;
							//Search in oclusion memoir nodes with player tags
							Node to_recover=GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);							
/*	
							///Assign to the best fit split blob the id of the merged blob - to solve splits without merges
							Node origin_merged_blob=GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);
							if(min_edges[iedge].weight < best_blob_fit_weight){
								best_blob_fit_weight=min_edges[iedge].weight;
								SetNodePlayertag(min_edges[iedge].uframe,min_edges[iedge].ubid,origin_merged_blob.player_tag);

								cout << "SetNodePlayertag of " << min_edges[iedge].ubid << " from frame " << min_edges[iedge].uframe 
								     << " with origin tag " << origin_merged_blob.player_tag << " and weight " << min_edges[iedge].weight << endl;
								
							}
*/							
							if(oclusion_memoir.vector_memoir.size()>0){///Is only possible to recover something lost
								//Node likely_node=oclusion_memoir.ExtractClosestMemoir(to_recover);
								Node merged = GetNode(min_edges[iedge].vframe,min_edges[iedge].vbid); ///This is the merged blob								
								ChangeNodeStartPointHistory(min_edges[iedge].uframe,min_edges[iedge].ubid,merged.centroid); ///Aca cambio el start point a to_recover para que tenga el centroide del merged por si sigue la misma direccion
								Node likely_node=oclusion_memoir.GetClosestMemoir(to_recover);///Return the node that resembles more to the split node
								
								cout << "Assign for blob " << to_recover.blob_id << " of frame " << to_recover.blob_frame_number 
							     << " from memorized blob " << likely_node.blob_id << " of frame " << likely_node.blob_frame_number << " with tag " << likely_node.player_tag << endl;
																		
														
								if(merged.player_tag_set.count(likely_node.player_tag)>0){	///Buscar si el player tag ya fue removido sino haga lo que sigue, para que funcione debo transferir tambien el set de tags						
										cout << "El blob compuesto tiene el id del nodo por recuperar, en total tiene " << merged.player_tag_set.size() << " ids." << endl;
										///Copy player tag from likely to recover and remove player tag from set
										//if(to_recover.player_tag_set.size()<=1){
											cout << "Tag set size from blob to recover = " << to_recover.player_tag_set.size() << endl;
											SetNodePlayertag(to_recover.blob_frame_number, min_edges[iedge].ubid, likely_node.player_tag);
											AddTagAndCentroid(to_recover.blob_frame_number,min_edges[iedge].ubid, likely_node.player_tag,likely_node.centroid);
											RemoveNodePlayertagFromBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,likely_node.player_tag);
											oclusion_memoir.Forget(to_recover); ///Aca esta borrando, esto es un problema si es un merged xq borra el tag heredado
										/*}
										else{
											for (std::set<int>::iterator trit=to_recover.player_tag_set.begin(); trit!=to_recover.player_tag_set.end(); ++trit){
												SetNodePlayertag(to_recover.blob_frame_number, min_edges[iedge].ubid, *trit); 
											}
										}
										*/
								}
							}
						}
					}				
			}
		}
	}
}

void MultiPartiteGraph::AnalyzeEdgesv2(std::vector<Edge> min_edges, int direction)
{
	///Figure it out if there is any event
	std::map< int, std::set<int> > oclusion_map; //In the form of destination to a list of sources
	
	///Create an event/oclusion map
	for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
		int source_blob_id = min_edges[iedge].ubid;
		int destination_blob_id = min_edges[iedge].vbid;
		//cout << "Source blob = " << source_blob_id << " destination blob = " << destination_blob_id;
		//cout << " Weight = " << min_edges[iedge].weight << endl;
		oclusion_map[destination_blob_id].insert(source_blob_id);
	}
	
	///Inspect merge/split oclusion map
	std::map<int,std::set<int>>::iterator omit;
	for (omit=oclusion_map.begin(); omit!=oclusion_map.end(); ++omit){
		//cout << "Merge size v2 for blob " << omit->first << " is " << omit->second.size() << endl;
		if(omit->second.size()>1){///Process only detected merges/splits
			if(direction==BACK){///Merges
			
				std::cout << "--Merge v2--" << endl;
				std::cout << "From " ;
			
				for (auto sourcev : omit->second)
					std::cout << sourcev << " ";
		
				std::cout << "to " << omit->first << endl;
				
				///Search the frame number for source blob id
				std::vector<Edge>::iterator edgesit;;
				double best_blob_fit_weight=9999999;
				
				for (auto sourcev : omit->second) ///Sweep through the sources of the merge event
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){///Search on borders to match the sourcev id
						if(min_edges[iedge].ubid==sourcev){
							cout << "Trying to remember blob " << min_edges[iedge].ubid << " from frame " << min_edges[iedge].uframe << endl;
							Node to_remember = GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);
							cout << "Tag set size of node to remenber = " << to_remember.player_tag_set.size() << " with tag = " << to_remember.player_tag << " and blobid = " <<  to_remember.blob_id << endl;
							//oclusion_memoir.Remember(to_remember); //Add node to short term memory, both single and merged
							if(to_remember.player_tag_set.size()<=1){///If is a individual blob thats is going to merge
								oclusion_memoir.Remember(to_remember);//Remember only single blobs
								cout << "Inserting single player tag " << to_remember.player_tag << " to set and changing state to blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;
								InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag); //Add player tag to list.
							}

							else{///If the blob is already a merged blob, copy their tags to the new merged blob, in the if append the new single blob
								oclusion_memoir.Remember(to_remember); ///Coment to avoid remembering merged blobs, but this helps to recover splits without merges
								cout << "Node is a merged one with "<< to_remember.player_tag_set.size() << " tags, inserting player tags to the next merged blob"<<endl;							
								InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag); //Add player tag to list.

								for (std::set<int>::iterator tsit=to_remember.player_tag_set.begin(); tsit!=to_remember.player_tag_set.end(); ++tsit){
									//cout << "Inserting player tag " << *tsit << " to set and changing state to blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;
									InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,*tsit);									
								}

							}

							ChangeBlobState(min_edges[iedge].vframe,min_edges[iedge].vbid,MULTI_PLAYER);
							
							///Assign to the merged blob the id of the most likely blob
							if(min_edges[iedge].weight < best_blob_fit_weight){
								best_blob_fit_weight=min_edges[iedge].weight;
								SetNodePlayertag(min_edges[iedge].vframe, min_edges[iedge].vbid, to_remember.player_tag);
								AddTagAndCentroid(min_edges[iedge].vframe, min_edges[iedge].vbid, to_remember.player_tag, to_remember.centroid);

								cout << "SetNodePlayertag of " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe 
								     << " to " << to_remember.player_tag << " with weight " << min_edges[iedge].weight << endl;

							}
						}
				}
			}
			else if(direction==FRONT){///Splits

				std::cout << "--Split v2--" << endl;
				std::cout << "From " << omit->first << " to ";			
				for (auto sourcev : omit->second)
					std::cout << sourcev << " ";				
				std::cout << endl;			
				
				///Search the frame number for source blob id
				std::vector<Edge>::iterator edgesit;				
				
				for (auto sourcev : omit->second)
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
						if(min_edges[iedge].ubid==sourcev){
							cout << "Trying to recover blob " << min_edges[iedge].ubid << " from oclusion memoir." << endl;
							//Search in oclusion memoir nodes with player tags
							Node to_recover=GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);							
						
							if(oclusion_memoir.vector_memoir.size()>0){///Is only possible to recover something lost
								//Node likely_node=oclusion_memoir.ExtractClosestMemoir(to_recover);
								Node merged = GetNode(min_edges[iedge].vframe,min_edges[iedge].vbid); ///This is the merged blob								
								ChangeNodeStartPointHistory(min_edges[iedge].uframe,min_edges[iedge].ubid,merged.centroid); ///Aca cambio el start point a to_recover para que tenga el centroide del merged por si sigue la misma direccion
								Node likely_node=oclusion_memoir.GetMemoirByTags(to_recover,merged.player_tag_set);///Return the node that resembles more to the split node
								
								cout << "Assign for blob " << to_recover.blob_id << " of frame " << to_recover.blob_frame_number 
							     << " from memorized blob " << likely_node.blob_id << " of frame " << likely_node.blob_frame_number 
							     << " with tag " << likely_node.player_tag << " and tag set size " << likely_node.player_tag_set.size() << endl;
																		
														
								if(merged.player_tag_set.count(likely_node.player_tag)>0){	///Buscar si el player tag ya fue removido sino haga lo que sigue, para que funcione debo transferir tambien el set de tags						
										cout << "El blob compuesto tiene el id del nodo por recuperar, en total tiene " << merged.player_tag_set.size() << " ids." << endl;
										///Copy player tag from likely to recover and remove player tag from set
										//if(to_recover.player_tag_set.size()<=1){
											cout << "Tag set size from blob to recover = " << to_recover.player_tag_set.size() << endl;
											SetNodePlayertag(to_recover.blob_frame_number, min_edges[iedge].ubid, likely_node.player_tag); 
											AddTagAndCentroid(to_recover.blob_frame_number, min_edges[iedge].ubid, likely_node.player_tag, likely_node.centroid);
											RemoveNodePlayertagFromBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,likely_node.player_tag);///Quitar el tag del merged tag set
											oclusion_memoir.Forget(to_recover); ///Aca esta borrando, esto es un problema si es un merged xq borra el tag heredado
										/*}
										else{
											for (std::set<int>::iterator trit=to_recover.player_tag_set.begin(); trit!=to_recover.player_tag_set.end(); ++trit){
												SetNodePlayertag(to_recover.blob_frame_number, min_edges[iedge].ubid, *trit); 
											}
										}
										*/
								}
/*								
								///Ahora hay que limpiar el tag set del split/to recover blob
								for (std::set<int>::iterator trit=to_recover.player_tag_set.begin(); trit!=to_recover.player_tag_set.end(); ++trit){
									if(!oclusion_memoir.TagIsInMemoir(*trit))
										cout << "Tag " << *trit << " not found on the oclusion memoir thats on the tag set from blob id " 
										     << min_edges[iedge].ubid << " of frame " << to_recover.blob_frame_number << endl;
										RemoveNodePlayertagFromBlob(to_recover.blob_frame_number,min_edges[iedge].ubid,*trit);///Quitar el tag del split tag set
								}
*/								
							}
						}
					}				
			}
		}
	}
}

void MultiPartiteGraph::AnalyzeEdgesv3(std::vector<Edge> min_edges, int direction)
{
	///Figure it out if there is any event
	std::map< int, std::set<int> > oclusion_map; //In the form of destination to a list of sources
	
	///Create an event/oclusion map
	for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
		int source_blob_id = min_edges[iedge].ubid;
		int destination_blob_id = min_edges[iedge].vbid;
		//cout << "Source blob = " << source_blob_id << " destination blob = " << destination_blob_id;
		//cout << " Weight = " << min_edges[iedge].weight << endl;
		oclusion_map[destination_blob_id].insert(source_blob_id);
	}
	
	///Inspect merge/split oclusion map
	std::map<int,std::set<int>>::iterator omit;
	for (omit=oclusion_map.begin(); omit!=oclusion_map.end(); ++omit){
		if(omit->second.size()>1){///Process only detected merges/splits
			if(direction==BACK){///Merges
			
				std::cout << "--Merge v3--" << endl;
				std::cout << "From " ;
			
				for (auto sourcev : omit->second)
					std::cout << sourcev << " ";
		
				std::cout << "to " << omit->first << endl;
				
				///Search the frame number for source blob id
				std::vector<Edge>::iterator edgesit;;
				double best_blob_fit_weight=9999999;
				
				for (auto sourcev : omit->second) ///Sweep through the sources of the merge event
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){///Search on borders to match the sourcev id
						if(min_edges[iedge].ubid==sourcev){
							cout << "Trying to remember blob " << min_edges[iedge].ubid << " from frame " << min_edges[iedge].uframe << endl;
							Node to_remember = GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);
							cout << "Tag set size of node to remenber = " << to_remember.player_tag_set.size() << " with tag = " << to_remember.player_tag << " and blobid = " <<  to_remember.blob_id << endl;
							//oclusion_memoir.Remember(to_remember); //Add node to short term memory, both single and merged
							if(to_remember.player_tag_set.size()<=1){///If is a individual blob thats is going to merge
								oclusion_memoir.Remember(to_remember);//Remember only single blobs
								cout << "Inserting single player tag " << to_remember.player_tag << " to set and changing state to blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;
								InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag); //Add player tag to list.
							}

							else{///If the blob is already a merged blob, copy their tags to the new merged blob
								//oclusion_memoir.Remember(to_remember); ///Coment to avoid remembering merged blobs, but this helps to recover splits without merges
								cout << "Node is a merged one with "<< to_remember.player_tag_set.size() << " tags, inserting player tags to the next merged blob"<<endl;							
								//InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag); //Add player tag to list.

								for (std::set<int>::iterator tsit=to_remember.player_tag_set.begin(); tsit!=to_remember.player_tag_set.end(); ++tsit){
									//cout << "Inserting player tag " << *tsit << " to set and changing state to blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;
									InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,*tsit);									
								}

							}

							ChangeBlobState(min_edges[iedge].vframe,min_edges[iedge].vbid,MULTI_PLAYER);
							
							///Assign to the merged blob the id of the most likely blob
							if(min_edges[iedge].weight < best_blob_fit_weight){
								best_blob_fit_weight=min_edges[iedge].weight;
								SetNodePlayertag(min_edges[iedge].vframe, min_edges[iedge].vbid, to_remember.player_tag);

								cout << "SetNodePlayertag of " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe 
								     << " to " << to_remember.player_tag << " with weight " << min_edges[iedge].weight << endl;

							}
						}
				}
			}
			else if(direction==FRONT){///Splits

				std::cout << "--Split v3--" << endl;
				std::cout << "From " << omit->first << " to ";			
				for (auto sourcev : omit->second)
					std::cout << sourcev << " ";				
				std::cout << endl;			
				
				///Search the frame number for source blob id
				std::vector<Edge>::iterator edgesit;				
				
				for (auto sourcev : omit->second)
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
						if(min_edges[iedge].ubid==sourcev){
							cout << "Trying to recover blob " << min_edges[iedge].ubid << " from oclusion memoir." << endl;
							//Search in oclusion memoir nodes with player tags
							Node to_recover=GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);							
						
							if(oclusion_memoir.vector_memoir.size()>0){///Is only possible to recover something lost
								//Node likely_node=oclusion_memoir.ExtractClosestMemoir(to_recover);
								Node merged = GetNode(min_edges[iedge].vframe,min_edges[iedge].vbid); ///This is the source merged blob								
								ChangeNodeStartPointHistory(min_edges[iedge].uframe,min_edges[iedge].ubid,merged.centroid); ///Aca cambio el start point a to_recover para que tenga el centroide del merged por si sigue la misma direccion
								Node likely_node=oclusion_memoir.GetMemoirByTags(to_recover,merged.player_tag_set);///Return the node that resembles more to the split node
								
								cout << "Assign for blob " << to_recover.blob_id << " with tag set size " << to_recover.player_tag_set.size() << " of frame " << to_recover.blob_frame_number 
							     << " from memorized blob " << likely_node.blob_id << " of frame " << likely_node.blob_frame_number 
							     << " with tag " << likely_node.player_tag << " and tag set size " << likely_node.player_tag_set.size() << endl;
																		
														
								if(merged.player_tag_set.count(likely_node.player_tag)>0){	///Buscar si el player tag ya fue removido sino haga lo que sigue, para que funcione debo transferir tambien el set de tags						
										cout << "El blob compuesto tiene el id del nodo por recuperar, en total tiene " << merged.player_tag_set.size() << " ids." << endl;
										///Copy player tag from likely to recover and remove player tag from set
										//if(to_recover.player_tag_set.size()<=1){
											//cout << "Single blob to recover with tag set size = " << to_recover.player_tag_set.size() << endl;
											SetNodePlayertag(to_recover.blob_frame_number, min_edges[iedge].ubid, likely_node.player_tag); 
											RemoveNodePlayertagFromBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,likely_node.player_tag);
											oclusion_memoir.Forget(to_recover); ///Aca esta borrando, esto es un problema si es un merged xq borra el tag heredado
										//}
/*
										else{
											cout << "The blob to recover is a merged, not recovering from memoir, tag size = " << to_recover.player_tag_set.size() << endl;
											for (std::set<int>::iterator trit=to_recover.player_tag_set.begin(); trit!=to_recover.player_tag_set.end(); ++trit){
												SetNodePlayertag(to_recover.blob_frame_number, min_edges[iedge].ubid, *trit); 
											}
										}
*/										
								}
							}
						}
					}				
			}
		}
	}
}

void MultiPartiteGraph::AnalyzeEdgesv4(std::vector<Edge> min_edges, std::map<std::string,int> mpgtparms,int direction)
{
	int match_method = mpgtparms["match_method"];
	int match_threshold = mpgtparms["match_threshold"];
	int merged_blob_area_threshold =  mpgtparms["merged_blob_area_threshold"];
	int min_blob_area_threshold =  mpgtparms["min_blob_area_threshold"];
	
	//cout << "Umbral para considerar blob como merged = " << merged_blob_area_threshold << endl;
	//cout << "Umbral para considerar blob como parcial o bola = " << min_blob_area_threshold << endl;
	
	///Figure it out if there is any event
	std::map< int, std::set<int> > oclusion_map; //In the form of destination to a list of sources
	//cout << "Analyze events between " << min_edges[0].uframe << " and " << min_edges[0].vframe << " with direction " << direction << endl;

/*
	if(direction == BACK){
		cout << "Inspecting border merge edges" << endl;
		PrintEdges(min_edges);
	}
	else{
		cout << "Inspecting border split edges" << endl;
		PrintEdges(min_edges);
	}
*/	
	
	///Create an event/oclusion map
	for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
		int source_blob_id = min_edges[iedge].ubid;
		int destination_blob_id = min_edges[iedge].vbid;
		//cout << "Source blob = " << source_blob_id << " destination blob = " << destination_blob_id;
		//cout << " Weight = " << min_edges[iedge].weight << endl;
		oclusion_map[destination_blob_id].insert(source_blob_id);
	}
	
	///Inspect merge/split oclusion map
	std::map<int,std::set<int>>::iterator omit;
	for (omit=oclusion_map.begin(); omit!=oclusion_map.end(); ++omit){
		//cout << "Merge size " << omit->second.size() << endl;
		if(omit->second.size()>1){///Process only detected merges/splits
			if(direction==BACK){///Merges
			
				std::cout << "--Merge v4--" << endl;
				std::cout << "From " ;
			
				///Imprimir las correspondencias de varios nodos a un nodo
				for (auto sourcev : omit->second)
					std::cout << sourcev << " ";
		
				std::cout << "to " << omit->first << endl;
	
				
				for (auto sourcev : omit->second) ///Sweep through the sources of the merge event
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){///Search on borders to match the sourcev id
						if(min_edges[iedge].ubid==sourcev){
							cout << "Trying to remember blob " << min_edges[iedge].ubid << " from frame " << min_edges[iedge].uframe << endl;
							Node to_remember = GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);
							Node merged_blob = GetNode(min_edges[iedge].vframe,min_edges[iedge].vbid);
							cout << "Tag set size of node to remenber = " << to_remember.player_tag_set.size() << " with tag = " << to_remember.player_tag << " and blobid = " <<  to_remember.blob_id << endl;
							
							///Para detectar que es un blob con varios jugadores juntos
							if(to_remember.area > ( prom_area * merged_blob_area_threshold/100.0))
								to_remember.SetBlobType(MULTI_PLAYER);
							else
								to_remember.SetBlobType(SINGLE_PLAYER);

							///Para descartar partes de jugadores o la bola
							if(to_remember.area < ( prom_area * min_blob_area_threshold/100.0))
								to_remember.SetBlobType(NON_PLAYER);							
							
							//oclusion_memoir.Remember(to_remember); //Add node to short term memory, both single and merged
							if((to_remember.merged_centroids.size()<=1) && (to_remember.blob_type == SINGLE_PLAYER ) ){///If is a individual blob thats is going to merge (filtering small blobs such as the ball and partial parts of a player)
								oclusion_memoir.Remember(to_remember);//Remember only single blobs
								cout << "Inserting single player tag " << to_remember.player_tag << " with centroid to merged centroids on destiny blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;
								///Search on the destiny blob for the centroid of the merged blob with template matching
								
								if((to_remember.contourRegion.cols <= merged_blob.contourRegion.cols) && 
								   (to_remember.contourRegion.rows <= merged_blob.contourRegion.rows)){
									double match_value;
									cv::Point new_centroid = MatchBlobTemplate(merged_blob.contourRegion, merged_blob.centroid, to_remember.contourRegion, match_method, &match_value);
									//cout << "Match value of " << match_value << " with the matching method " << match_method << endl;
									if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ){//min
										if(match_value <= match_threshold)
											InsertNodePlayerTagCentroidToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag,new_centroid); //Add player tag and centroid to destiny blob list.
									}
									else{//max
										if(match_value >= match_threshold)
											InsertNodePlayerTagCentroidToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag,new_centroid); //Add player tag and centroid to destiny blob list.
									}
									InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_remember.player_tag); //Add player tag to list.
								}
								//else
								//	cout << "Unable to search merged blob within new blob because of oversize template dimension." << endl;
							}

						
							else if((to_remember.merged_centroids.size()>1) && (to_remember.area > prom_area * min_blob_area_threshold/100.0 )){///If the blob is already a merged blob, copy their tags to the new merged blob, in the if append the new single blob
								//oclusion_memoir.Remember(to_remember); ///Coment to avoid remembering merged blobs, but this helps to recover splits without merges
								cout << "Node is a merged one with "<< to_remember.merged_centroids.size() << " merged centroids, inserting player tag and centroid to the next merged blob"<<endl;															

								///Copy source merged centroids and player tags to destiny blob
								for (map< int,Point2f >::iterator mipit=to_remember.merged_centroids.begin(); mipit!=to_remember.merged_centroids.end(); ++mipit){
									//cout << "Inserting player tag " << *tsit.first << " to set and changing state to blob " << min_edges[iedge].vbid << " from frame " << min_edges[iedge].vframe << endl;

									///Search for the merged centroid on the oclusion memoir
									for(unsigned int omvi = 0; omvi < oclusion_memoir.vector_memoir.size(); omvi++){
										Node to_search = oclusion_memoir.vector_memoir[omvi].info;
										if(mipit->first == to_search.player_tag){

											if((to_search.contourRegion.cols <= merged_blob.contourRegion.cols) && 
											  (to_search.contourRegion.rows <= merged_blob.contourRegion.rows)){
												///Search on the destiny blob for the centroid of the merged blob with template matching
												double match_value;
												cv::Point new_centroid = MatchBlobTemplate(merged_blob.contourRegion, merged_blob.centroid, to_search.contourRegion, match_method, &match_value);

												//cout << "Match value of " << match_value << " with the matching method " << match_method << endl;
												if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ){//min
													if(match_value <= match_threshold)
														InsertNodePlayerTagCentroidToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_search.player_tag,new_centroid);
												}
												else{//max
													if(match_value >= match_threshold)
														InsertNodePlayerTagCentroidToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,to_search.player_tag,new_centroid);
												}																								
												InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,mipit->first);									
											}
										}
									}
									
									
								}

							}
							
						}
				}
			}
		
			else if(direction==FRONT){///Splits

				std::cout << "--Split v4--" << endl;
				std::cout << "From " << omit->first << " to ";			
				for (auto destv : omit->second)
					std::cout << destv << " ";				
				std::cout << endl;			
				
				///Search the frame number for source blob id
				std::vector<Edge>::iterator edgesit;				
				
				for (auto destv : omit->second)
					for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
						if(min_edges[iedge].ubid==destv){
							cout << "Trying to recover blob " << min_edges[iedge].ubid << " from oclusion memoir." << endl;
							//Search in oclusion memoir nodes with player tags
							Node to_recover=GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);
							//cout << "To recover blob area = " << to_recover.area << endl;
							//cout << "Prom area = " << prom_area << " merged_blob_area_threshold size = " << prom_area * merged_blob_area_threshold/100.0 << endl;
							
							///Para detectar que es un blob con varios jugadores juntos
							if(to_recover.area > ( prom_area * merged_blob_area_threshold/100.0))
								to_recover.SetBlobType(MULTI_PLAYER);
							else
								to_recover.SetBlobType(SINGLE_PLAYER);

							///Para descartar partes de jugadores o la bola
							if(to_recover.area < ( prom_area * min_blob_area_threshold/100.0))
								to_recover.SetBlobType(NON_PLAYER);

						
							if(oclusion_memoir.vector_memoir.size()>0){///Is only possible to recover something lost
								//Node likely_node=oclusion_memoir.ExtractClosestMemoir(to_recover);
								Node merged = GetNode(min_edges[iedge].vframe,min_edges[iedge].vbid); ///This is the merged blob								
								ChangeNodeStartPointHistory(min_edges[iedge].uframe,min_edges[iedge].ubid,merged.centroid); ///Aca cambio el start point a to_recover para que tenga el centroide del merged por si sigue la misma direccion
								//Node likely_node=oclusion_memoir.GetMemoirByTags(to_recover,merged.player_tag_set);
								Node likely_node=oclusion_memoir.GetMemoirByTagsAndTM1(to_recover,merged.player_tag_set,merged.merged_centroids);///Return the node that resembles more to the split node
								
								cout << "Assign for blob " << to_recover.blob_id << " of frame " << to_recover.blob_frame_number 
							     << " from memorized blob " << likely_node.blob_id << " of frame " << likely_node.blob_frame_number 
							     << " with tag " << likely_node.player_tag << " and tag set size " << likely_node.player_tag_set.size() << endl;
																		
														
								if((merged.merged_centroids.count(likely_node.player_tag)>0) && 
								   (to_recover.blob_type == SINGLE_PLAYER)){
								//if((merged.merged_centroids.count(likely_node.player_tag)>0) ){
										cout << "El blob compuesto tiene el id del nodo por recuperar, en total tiene " << merged.merged_centroids.size() << " merged centroids." << endl;
										///Copy player tag from likely to recover and remove player tag from set
										//if(to_recover.player_tag_set.size()<=1){
											cout << "Merged centroids size from blob to recover = " << to_recover.merged_centroids.size() << endl;
											ClearTagSet(to_recover.blob_frame_number, min_edges[iedge].ubid);
											ClearTagAndCentroids(to_recover.blob_frame_number, min_edges[iedge].ubid);
											SetNodePlayertag(to_recover.blob_frame_number, min_edges[iedge].ubid, likely_node.player_tag); 
											SetTagAndCentroid(to_recover.blob_frame_number, min_edges[iedge].ubid, likely_node.player_tag, to_recover.centroid);
											RemoveNodePlayertagFromBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,likely_node.player_tag);///Quitar el tag del merged tag set
											oclusion_memoir.Forget(likely_node); 
											//DeleteTagAndCentroid(min_edges[iedge].vframe,min_edges[iedge].vbid,likely_node.player_tag);

								}
								else if(to_recover.blob_type == NON_PLAYER){///Es muy  pequeno, se descarta
									cout << "The blob to recover is too small." << endl;
								}
								else{//Es un merged blob, buscar cuales blobs no se han separado y transferirlos al nuevo
									cout << "The blob to recover is a merged blob" << endl;

									ClearTagAndCentroids(min_edges[iedge].uframe,min_edges[iedge].ubid);
									ClearTagSet(min_edges[iedge].uframe,min_edges[iedge].ubid);
									Node merged = GetNode(min_edges[iedge].vframe,min_edges[iedge].vbid); 
									//Node likely_node=oclusion_memoir.GetMemoirByTagsAndTM1(to_recover,merged.player_tag_set,merged.merged_centroids);
									CopyNodePlayerTagSet(min_edges[iedge].uframe,min_edges[iedge].ubid,merged.player_tag_set);
									CopyMergedCentroidsMap(min_edges[iedge].uframe,min_edges[iedge].ubid,merged.merged_centroids);
									SetNodeType(min_edges[iedge].uframe,min_edges[iedge].ubid,MULTI_PLAYER);
								}
									
							}
						}
					}				
			}									
			//}
			
		}//if only detected merge 

	
		else if(omit->second.size()==1){///Process one to one blob coincidence but only the merged blobs
			if(direction==BACK){
				
				auto sourcev = omit->second;
				//auto destu = omit->first;
				
				for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){///Search on borders to match the sourcev id
					std::set<int>::iterator srcit = omit->second.begin();
					if((*srcit == min_edges[iedge].ubid) && (omit->first == min_edges[iedge].vbid)){///el second es la fuente, el first es el destino
						Node sourcen = GetNode(min_edges[iedge].uframe,min_edges[iedge].ubid);
						Node destn = GetNode(min_edges[iedge].vframe,min_edges[iedge].vbid);
						
						///Buscar los tags que estan en el merged centroid del source para hacer el template matching en el siguiente y asignar los nuevos centroides	
						if(sourcen.merged_centroids.size()>1){
							//cout << "Blob " << sourcen.blob_id << " en " << sourcen.blob_frame_number << " con " << sourcen.merged_centroids.size()<< " merged centroids" << endl;
							//cout << "Blob fuente en " << sourcen.centroid.x << " x " << sourcen.centroid.y << endl;
							//cout << "Blob destino en " << destn.centroid.x << " x " << destn.centroid.y << endl;

							std::map< int,Point2f >::iterator srcmcit;
							for (srcmcit = sourcen.merged_centroids.begin(); srcmcit!=sourcen.merged_centroids.end(); ++srcmcit) {							
								Node templ_blob = oclusion_memoir.GetMemoirByTag(srcmcit->first);
								//cout << "Blob template en " << templ_blob.centroid.x << " x " << templ_blob.centroid.y << " con tag " << templ_blob.player_tag << " desde frame " << templ_blob.blob_frame_number << endl;


								if((templ_blob.contourRegion.cols <= destn.contourRegion.cols) && 
								   (templ_blob.contourRegion.rows <= destn.contourRegion.rows) &&
							       (templ_blob.contourRegion.cols>0 && templ_blob.contourRegion.rows>0)){
									double match_value;
									cv::Point new_centroid = MatchBlobTemplate(destn.contourRegion, destn.centroid, templ_blob.contourRegion, match_method, &match_value);
									//cout << "Nuevo centroide de Blob template en " << new_centroid.x << " x " << new_centroid.y << endl;
									//cout << "Match value of " << match_value << " with the matching method " << match_method << endl;
									if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ){//min
										if(match_value <= match_threshold)
											InsertNodePlayerTagCentroidToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,templ_blob.player_tag,new_centroid); //Add player tag and centroid to destiny blob list.								
									}
									else{//max
										if(match_value >= match_threshold)
											InsertNodePlayerTagCentroidToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,templ_blob.player_tag,new_centroid); //Add player tag and centroid to destiny blob list.								
									}									
									InsertNodePlayertagToBlob(min_edges[iedge].vframe,min_edges[iedge].vbid,templ_blob.player_tag); //Add player tag to list.
								}
								//else
								//	cout << "Unable to search merged blob within new blob because of oversize template dimension." << endl;
							
							}
						}
					}	
						
					
					
				}				
			}
		}

	}//for loop
}

void MultiPartiteGraph::AnalyzeAbductions(std::vector<Edge> min_edges, int source_frame_number, int direction)
{
	
	
		int frame_number = source_frame_number;
		
		Graph source_gph;
		source_gph = GetGraph(frame_number);
	
		///Identify nodes without a destination node
		if(direction==BACK){///Remember blob if disappears
			cout << "--Inspecting for lost blobs" << endl;
			for(unsigned int unit = 0 ; unit < source_gph.size(); ++unit){///Look only on source blobs, if theres no match consider a abduction

				Node source_node;
				source_node = source_gph[unit];
				//cout << "---Inspecting blob id = " << source_node.blob_id << " from frame " << source_node.blob_frame_number << endl; 
				bool found=false;
				for(unsigned int iedge = 0; iedge < min_edges.size() and not found; iedge++){
					int source_blob_id = min_edges[iedge].ubid; int source_frame_id = min_edges[iedge].uframe;				
					//cout << "Edge blob id = " << source_blob_id << " from frame " << source_frame_id << endl;				
					if((source_node.blob_id==source_blob_id) && (source_node.blob_frame_number==source_frame_id))
						found=true;
				}
			
				if(not found){///Solo existen los bordes que convergen, esto es que existe alguna asociacion, sino quiere decir que desaparecio
					cout << "Blob " << source_node.blob_id << " from frame " << source_node.blob_frame_number << " with tag " << source_node.player_tag << " abduced." << endl;
					abduction_memoir.Remember(source_node);
					cout << "Abduction memory size = " << abduction_memoir.vector_memoir.size() << endl;
				}
			}
		}
		
		if(direction==FRONT){///Recover blob if reapears
			cout << "--Inspecting for abducted blobs" << endl;
			Graph destination_gph;
			destination_gph = GetGraph(frame_number+1);
			///Identify nodes without a source node
			for(unsigned int unit = 0; unit < destination_gph.size(); ++unit){///Look only on destination blobs, if theres no match consider a reborn :P

				Node destination_node;
				destination_node = destination_gph[unit];
	
				bool found=false;			
				for(unsigned int iedge = 0; iedge < min_edges.size(); iedge++){
					int destination_blob_id = min_edges[iedge].ubid; int destination_frame_id = min_edges[iedge].uframe;				
					if((destination_node.blob_id==destination_blob_id) and (destination_node.blob_frame_number==destination_frame_id))
						found=true;
				}
				if(not found and abduction_memoir.vector_memoir.size()>0){				
					Node reborn_node = abduction_memoir.GetClosestMemoir(destination_node);
					//Node reborn_node = abduction_memoir.GetClosestMemoir(destination_node);
					cout << "Blob " << destination_node.blob_id << " from frame " << destination_node.blob_frame_number << " with tag " << destination_node.player_tag << " untracked, trying to recover from abduction memory." << endl;
					if(reborn_node.player_tag != 0){///Esto porque si no encuentra es igual a cero
						cout << "Recovering with blob " << reborn_node.blob_id << " from frame " << reborn_node.blob_frame_number << " with tag " << reborn_node.player_tag << endl;
						cout << "Abduction memory size = " << abduction_memoir.vector_memoir.size() << endl;						
						abduction_memoir.Forget(reborn_node);
						SetNodePlayertag(destination_node.blob_frame_number,destination_node.blob_id,reborn_node.player_tag);
						CopyNodePlayerTagSet(destination_node.blob_frame_number,destination_node.blob_id,reborn_node.player_tag_set);
						CopyMergedCentroidsMap(destination_node.blob_frame_number,destination_node.blob_id,reborn_node.merged_centroids);						
					}
				}			
			}
		
		}
		
}

std::vector<Memoir> MultiPartiteGraph::GetOclusionMemories(void)
{
		return oclusion_memoir.vector_memoir;
}

std::vector<Memoir> MultiPartiteGraph::GetAbductionMemories(void)
{
		return abduction_memoir.vector_memoir;
}

void MultiPartiteGraph::SetOclusionMemories(std::vector<Memoir> om)
{
		oclusion_memoir.vector_memoir = om;
}

void MultiPartiteGraph::SetAbductionMemories(std::vector<Memoir> am)
{
		abduction_memoir.vector_memoir = am;
}

void MultiPartiteGraph::PrintEdges(std::vector<Edge> edges)
{
	for(unsigned int iedge = 0; iedge < edges.size(); iedge++){
		int source_blob_id = edges[iedge].ubid;
		int destination_blob_id = edges[iedge].vbid;
		cout << "Source blob = " << source_blob_id << " ------- destination blob = " << destination_blob_id;
		cout << " Weight = " << edges[iedge].weight << endl;
	}	
}

MultiPartiteGraph::~MultiPartiteGraph()
{

}

