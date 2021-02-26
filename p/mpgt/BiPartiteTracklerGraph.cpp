#include "BiPartiteTracklerGraph.h"
#include "Defs.h"

using namespace cv;
using namespace std;

BiPartiteTracklerGraph::BiPartiteTracklerGraph()
{
	position_weight = 0.07;
	histogram_weight = 0.001;
	contour_weight = 0.001;
	area_weight = 0.00025;
	max_speed = 600;
}

BiPartiteTracklerGraph::BiPartiteTracklerGraph(std::map<std::string,double> weights)
{
	position_weight = weights["position"];
	histogram_weight = weights["histogram"];
	contour_weight = weights["contour"];
	area_weight = weights["area"];
	max_speed = weights["max_speed"];
}

void BiPartiteTracklerGraph::InsertGraph(Graph gph, int position)
{
	if(position==BACK){
		start_graph=gph;
	}
	else if(position==FRONT){
		end_graph=gph;
	}
}

Graph BiPartiteTracklerGraph::GetGraph(int position)
{
	if(position==BACK){
		return start_graph;
	}
	else {
		return end_graph;
	}
}

void BiPartiteTracklerGraph::InsertTrackpointToTracklet(int id, TrackPoint tp)
{
	tklts[id].push_back(tp);
}

std::vector<TrackPoint> BiPartiteTracklerGraph::GetVectorTrackingPointsFromTracklet(int id)
{
	VectorTrackingPoints vtp;
	vtp=tklts[id];
	return vtp;
}

std::vector<TrackPoint> BiPartiteTracklerGraph::GetVectorTrackingPointsFromFrame(int fnumber)
{
	VectorTrackingPoints vtp;
	
	for (std::map<int,VectorTrackingPoints>::iterator itklts=tklts.begin(); itklts!=tklts.end(); ++itklts){
		for (std::vector<TrackPoint>::iterator itvtkp=itklts->second.begin(); itvtkp!=itklts->second.end(); ++itvtkp)
			if((*itvtkp).frame_number==fnumber)
				vtp.push_back(*itvtkp);
	}
	
	return vtp;
}

void BiPartiteTracklerGraph::SetLabelToTracklet(int tracklet_id, int label)
{
	for (std::map<int,VectorTrackingPoints>::iterator itklts=tklts.begin(); itklts!=tklts.end(); ++itklts){
		for (std::vector<TrackPoint>::iterator itvtkp=itklts->second.begin(); itvtkp!=itklts->second.end(); ++itvtkp)
			(*itvtkp).label=label;
	}	
}

BiPartiteTracklerGraph::~BiPartiteTracklerGraph()
{

}
