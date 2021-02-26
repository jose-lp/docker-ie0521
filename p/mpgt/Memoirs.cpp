#include "Memoirs.h"
#include "Defs.h"

using namespace cv;
using namespace std;

OclusionMemoir::OclusionMemoir()
{
	
	position_weight=1;
	histogram_weight=1;
	contour_weight=1;
	area_weight=1;
	direction_weight=1;
	speed_weight=1;
	max_speed=1;
	memoir_diff_threshold=999999;
	blob_memoir_lenght=1800;
	match_method = 2;
	match_threshold = 50;
}

void OclusionMemoir::InitMemoir(std::map<std::string,int> mpgtparms,std::map<std::string,double> weights)
{
	position_weight=weights["position"] ;
	histogram_weight=weights["histogram"];
	contour_weight=weights["contour"];
	area_weight=weights["area"];
	direction_weight=weights["direction"];
	depth_weight=weights["depth"];
	speed_weight=weights["speed"];
	max_speed=weights["max_speed"];
	memoir_diff_threshold=mpgtparms["memoir_diff_threshold"];	
	blob_memoir_lenght=mpgtparms["blob_memoir_lenght"];
	match_method = mpgtparms["match_method"];
	match_threshold = mpgtparms["match_threshold"];	
}

void OclusionMemoir::Remember(Node to_remember, int how_long)
{
	Memoir something;
	something.SetMemoir(to_remember);
	something.SetTime(how_long);
	vector_memoir.push_back(something);
}

void OclusionMemoir::Remember(Node to_remember)
{
	Memoir something;
	something.SetMemoir(to_remember);
	something.SetTime(blob_memoir_lenght);
	vector_memoir.push_back(something);
}

double OclusionMemoir::CompareNodes(Node node1, Node node2)
{
		double delta_position=ComparePointsDistance(node1.centroid, node2.centroid);// cout << "Delta posicion = " << delta_position << endl;
		double delta_histogram=CompareHistograms(node1.histogram, node2.histogram);// cout << "Delta histogram = " << delta_histogram << endl;
		double delta_shape=CompareContours(node1.contorno, node2.contorno);// cout << "Delta contornos = " << delta_shape << endl;
		double delta_area=CompareAreas(node1.area, node2.area);
		double delta_direction=CompareDirections(node1.direction,node2.direction);
		double delta_speed=CompareSpeeds(node1.speed,node2.speed);
/*
		cout << "CompareNodesMemoir: " << endl;
		cout << "Blob id 1 = " << node1.blob_id << " and blob id 2 = " << node2.blob_id << endl;
		cout << "position weight= " << delta_position*position_weight << "\t histogram weight= " << delta_histogram*histogram_weight << "\t shape weight= " << delta_shape*contour_weight
		     << "\t area weight= " << delta_area*area_weight << "\t direction weight= " << delta_direction*direction_weight 
		     << "\t speed weight= "<< delta_speed*speed_weight << endl;
*/
		
		if(delta_position > max_speed*position_weight*30) //31.75 ... el 30 es a ojo
			delta_position = 9999999;
		
		double weight = delta_position*position_weight+
		                delta_histogram*histogram_weight+
		                delta_shape*contour_weight+
		                delta_area*area_weight+
		                delta_direction*direction_weight+
		                delta_speed*speed_weight;
		return weight;
}

double OclusionMemoir::CompareNodesWithDepth(Node node1, Node node2)
{
		double delta_depth=ComparePointsDepth(node1.centroid, node2.centroid);// cout << "Delta posicion = " << delta_position << endl;
		double delta_histogram=CompareHistograms(node1.histogram, node2.histogram);// cout << "Delta histogram = " << delta_histogram << endl;
		double delta_shape=CompareContours(node1.contorno, node2.contorno);// cout << "Delta contornos = " << delta_shape << endl;
		double delta_area=CompareAreas(node1.area, node2.area);
		double delta_direction=CompareDirections(node1.direction,node2.direction);
		double delta_speed=CompareSpeeds(node1.speed,node2.speed);
/*
		cout << "CompareNodesMemoirWithDepth: " << endl;
		cout << "Blob id 1 = " << node1.blob_id << " and blob id 2 = " << node2.blob_id << endl;
		cout << "depth weight= " << delta_depth*depth_weight << "\t histogram weight= " << delta_histogram*histogram_weight << "\t shape weight= " << delta_shape*contour_weight
		     << "\t area weight= " << delta_area*area_weight << "\t direction weight= " << delta_direction*direction_weight 
		     << "\t speed weight= "<< delta_speed*speed_weight << endl;
		
*/		
		double weight = delta_depth*depth_weight+
		                delta_histogram*histogram_weight+
		                delta_shape*contour_weight+
		                delta_area*area_weight+
		                delta_direction*direction_weight+
		                delta_speed*speed_weight;
		return weight;
}

Node OclusionMemoir::GetClosestMemoir(Node to_search)
{
	Node found;
	
	//Buscar el que se parece mas
	std::vector<Memoir>::iterator memit;

	double distance = 9999999;
	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		double retval = CompareNodes(to_search,memit->info);

/*		cout << "Return distance between blob id " << to_search.blob_id << " from frame " << to_search.blob_frame_number 
		     << " and memoir blob id " << memit->info.blob_id << " from frame " << memit->info.blob_frame_number
		     << " equal to " << retval << endl;
*/
		if((retval<distance) && (retval<memoir_diff_threshold)){
			distance = retval;
			found = memit->GetMemoir();
		}
	}

	return found;
}

Node OclusionMemoir::GetClosestMemoirWithDepth(Node to_search)
{
	Node found;
	
	//Buscar el que se parece mas
	std::vector<Memoir>::iterator memit;

	double distance = 9999999;
	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		double retval = CompareNodesWithDepth(to_search,memit->info);
/*
		cout << "Return distance between blob id " << to_search.blob_id << " from frame " << to_search.blob_frame_number 
		     << " and memoir blob id " << memit->info.blob_id << " from frame " << memit->info.blob_frame_number
		     << " equal to " << retval << endl;
*/
		if((retval<distance) && (retval<memoir_diff_threshold)){
			distance = retval;
			found = memit->GetMemoir();
		}
	}

	return found;
}

bool OclusionMemoir::TagIsInMemoir(int tag)
{
	bool found=false;
	
	//Buscar los tags
	std::vector<Memoir>::iterator memit;

	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){		
		if(tag == memit->info.player_tag){
			found = true;
		}
	}

	return found;
}

Node OclusionMemoir::GetMemoirByTags(Node to_search, set<int> player_tags)
{
	Node found;
	
	//Buscar el que tenga el tag indicado y que se parezca al nodo to_search
	double retval;
	double distance = 9999999999;

	std::vector<Memoir>::iterator memit;
	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		
		//if((player_tags.count(memit->info.player_tag)) && (memit->info.player_tag_set.size()<=1) ){///Compare only the nodes on the Memoir with a tag contained on the set
		if((player_tags.count(memit->info.player_tag))){///Compare only the nodes on the Memoir with a tag contained on the set
		
			retval = CompareNodesWithDepth(to_search,memit->info);
			if((retval<distance) && (retval<memoir_diff_threshold)){
				distance = retval;
				found = memit->GetMemoir();
			}
		}
	}

	return found;
}

Node OclusionMemoir::GetMemoirByTagsAndTM1(Node to_search, set<int> player_tags, map< int,Point2f > merged_centroids)
{
	Node found;
	
	//Buscar el que tenga el tag indicado y que se parezca al nodo to_search
	
	double retval;
	double distance = 9999999999;
	
	std::vector<Memoir>::iterator memit;
	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		
		//if((merged_centroids.count(memit->info.player_tag))){///Compare only the nodes on the Memoir with a tag contained on the merged_centroids
		if(player_tags.count(memit->info.player_tag) >= 1){///Compare only the nodes on the Memoir with a tag contained on the player tag set
			
			Node tmp_memoir_node = memit->info;
			std::map< int,Point2f >::iterator mrgcit;
			for (mrgcit = merged_centroids.begin(); mrgcit!=merged_centroids.end(); ++mrgcit) {//Use the last centroid within the merged blob as the centroid of the memoir
				if(mrgcit->first == tmp_memoir_node.player_tag){
					cout << "Reasignando el centroide al blob " << tmp_memoir_node.player_tag << endl; 
					tmp_memoir_node.centroid = mrgcit->second;
				}	
			}
		
			retval = CompareNodes(to_search,tmp_memoir_node);
			//retval = CompareNodesWithDepth(to_search,tmp_memoir_node);
			
			if((retval<distance) && (retval<memoir_diff_threshold)){
				distance = retval;
				found = memit->GetMemoir();
			}
			
		}
	}

	//cout << "Compare node value = " << distance << " and match value = " << match_value << endl; 
	return found;
}

Node OclusionMemoir::GetMemoirByTagsAndTM2(Node to_search, set<int> player_tags, map< int,Point2f > merged_centroids)
{
	Node found;
	
	//Buscar el que tenga el tag indicado y que se parezca al nodo to_search
	std::vector<Memoir>::iterator memit;
	double retval;
	double distance = 9999999999;
	double match_value = 0.0;

	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		
		//if((player_tags.count(memit->info.player_tag)) && (memit->info.player_tag_set.size()<=1) ){///Compare only the nodes on the Memoir with a tag contained on the set
		//if((merged_centroids.count(memit->info.player_tag))){///Compare only the nodes on the Memoir with a tag contained on the merged_centroids
		if(player_tags.count(memit->info.player_tag) >= 1){///Compare only the nodes on the Memoir with a tag contained on the player tag set
			
			Node tmp_memoir_node = memit->info;
			std::map< int,Point2f >::iterator mrgcit;
			for (mrgcit = merged_centroids.begin(); mrgcit!=merged_centroids.end(); ++mrgcit) {//Use the last centroid within the merged blob as the centroid of the memoir
				//if(mrgcit->first == tmp_memoir_node.player_tag){
				if(tmp_memoir_node.merged_centroids.count(mrgcit->first)){
					tmp_memoir_node.centroid = mrgcit->second;
				}	
			}
		
			//retval = CompareNodes(to_search,tmp_memoir_node);
			retval = CompareNodesWithDepth(to_search,tmp_memoir_node);
									
			if((to_search.contourRegion.cols <= tmp_memoir_node.contourRegion.cols) && 
			   (to_search.contourRegion.rows <= tmp_memoir_node.contourRegion.rows)){							
				MatchBlobTemplate(tmp_memoir_node.contourRegion, tmp_memoir_node.centroid, to_search.contourRegion,match_method, &match_value);
				
				if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ){
					if((match_value <= match_threshold) && (retval<distance) && (retval<memoir_diff_threshold)){
						distance = retval;
						found = memit->GetMemoir();
					}
				}
				else{
					if((match_value >= match_threshold) && (retval<distance) && (retval<memoir_diff_threshold)){
						distance = retval;
						found = memit->GetMemoir();				
					}
				}		
			}

			else{///Resize to search node
				Size size(tmp_memoir_node.contourRegion.cols-2,tmp_memoir_node.contourRegion.rows-2);
				Mat to_search_resize;
				resize(to_search.contourRegion,to_search_resize,size);

				MatchBlobTemplate(tmp_memoir_node.contourRegion, tmp_memoir_node.centroid, to_search_resize,match_method, &match_value);
				
				if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED ){
					if((match_value <= match_threshold) && (retval<distance) && (retval<memoir_diff_threshold)){
						distance = retval;
						found = memit->GetMemoir();
					}
				}
				else{
					if((match_value >= match_threshold) && (retval<distance) && (retval<memoir_diff_threshold)){
						distance = retval;
						found = memit->GetMemoir();				
					}
				}				
			}

/*			
			else if((retval<distance) && (retval<memoir_diff_threshold)){
				distance = retval;
				found = memit->GetMemoir();
			}
*/			
		}
	}

	//cout << "Compare node value = " << distance << " and match value = " << match_value << endl; 
	return found;
}

Node OclusionMemoir::GetMemoirByTag(int search_player_tag)
{
	Node found;
	
	//Buscar el que tenga el tag indicado
	std::vector<Memoir>::iterator memit;

	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){

		if(memit->info.player_tag == search_player_tag){
			found = memit->GetMemoir();
			break;
		}
	}

	return found;
}

Node OclusionMemoir::ExtractClosestMemoir(Node to_search)
{
	Node found;
	
	//Buscar el que se parece mas
	std::vector<Memoir>::iterator memit;
	unsigned int position=0;

	double distance = 9999999999;
	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		double retval = CompareNodes(to_search,memit->info);
		if((retval<distance) && (retval<memoir_diff_threshold)){
			distance = retval;
			found = memit->GetMemoir();
			position = std::distance( vector_memoir.begin(), memit );
		}
	}
	
	vector_memoir.erase(vector_memoir.begin()+position);
	//vector_memoir.shrink_to_fit();

	return found;
}

void OclusionMemoir::Forget(Node to_delete)
{
	Node found;
	
	//Buscar el que se parece mas
	std::vector<Memoir>::iterator memit;
	unsigned int position=0;

	double distance = 9999999999;
	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		double retval = CompareNodes(to_delete,memit->info);
		if((retval<distance) && (retval<memoir_diff_threshold)){
			distance = retval;
			found = memit->GetMemoir();
			position = std::distance( vector_memoir.begin(), memit );
		}
	}
	
	vector_memoir.erase(vector_memoir.begin()+position);
	//vector_memoir.shrink_to_fit();

}

void OclusionMemoir::DegradeMemoirs(void)
{
	
	std::vector<Memoir>::iterator memit;

	for (memit=vector_memoir.begin(); memit!=vector_memoir.end(); ++memit){
		if(memit->GetTimeLeft() != 0)
			memit->Degrade();
	}
	
	vector_memoir.erase(std::remove_if(vector_memoir.begin(),vector_memoir.end(),IsForgotten),vector_memoir.end());
	//vector_memoir.shrink_to_fit();
}

void OclusionMemoir::DegradeMemoirs(int ntimes)
{
	for(int n=0;n<ntimes;n++)
		DegradeMemoirs();
}

OclusionMemoir::~OclusionMemoir()
{

}

////////////////////////////////////////////////////////////////////////
Memoir::Memoir()
{
	time = 0;
}

void Memoir::SetTime(int seconds)
{
	time = seconds;
}

void Memoir::SetMemoir(Node memoir)
{
	info = memoir;
}

Node Memoir::GetMemoir(void)
{
	return info;
}

void Memoir::Degrade(void)
{
	if(time>0)
		time--;
	else
		time=0;
}

void Memoir::Degrade(int ntimes)
{
	for(int n=0;n<ntimes;n++)
		Degrade();
}

int Memoir::GetTimeLeft(void)
{
	return time;
}

bool IsForgotten(Memoir m)
{
	if(m.GetTimeLeft()==0)
		return true;
	else
		return false;
}

Memoir::~Memoir()
{

}

