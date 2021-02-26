#include "Measurer.h"

using namespace std;


Measurer::Measurer()
{
	cm = 1.0; cf = 1.0; cs = 1.0;
	misses = 0; false_positives = 0; 
	missmatches = 0; false_negatives = 0;
	gt_objects = 0; tk_objects = 0;
	missmatch_error = 0; detection_error = 0;
	report_level = BASIC_LEVEL;
	motp_error=0.0; motp_matches = 0;
	precision = 0; recall = 0;
	accuracy = 0; f1measure = 0;
	true_negatives = 0; true_positives = 0;
	mota = 0.0; motp = 0.0;
	start_frame = 0; stop_frame = 999999;
	mota_mean = 0.0; motp_mean = 0.0;
	mota_variance = 0.0; motp_mean = 0.0;
}

Measurer::Measurer(std::map<std::string,double> constants)
{
	cm = constants["miss"];
	cf = constants["false"];
	cs = constants["mismatch"];
	report_level = constants["report_level"];
	missmatch_error = constants["mismatch_error"];
	detection_error = constants["detection_error"];
	motp_error = constants["motp_error"];
	start_frame = constants["start_frame"];
	stop_frame = constants["stop_frame"];

	misses = 0; false_positives = 0; 
	missmatches = 0; false_negatives = 0;
	gt_objects = 0; tk_objects = 0;
	motp_matches = 0;
	precision = 0; recall = 0;
	accuracy = 0; f1measure = 0;
	true_negatives = 0; true_positives = 0;
	mota = 0.0; motp = 0.0;
}

double Measurer::MeasureMota(map<int, vector< TrackPoint >> manotados, map<int, vector< TrackPoint >> mrastreados)
{
	cout << "Analyzing tracking results for MOTA..." << endl;
	
	map<int, vector< TrackPoint >>::iterator ItmAnotados,ItmRastreados;

	std::vector<int> gt_frames;
	int fnumber = -1;
	for (ItmAnotados = manotados.begin(); ItmAnotados != manotados.end(); ItmAnotados++) {
		for (ItmRastreados = mrastreados.begin(); ItmRastreados != mrastreados.end(); ItmRastreados++) {
			if(ItmRastreados->first == ItmRastreados->first){///We have a match on the frame numbers
				vector< TrackPoint > datos_anotados, datos_rastreados;
				datos_anotados = ItmAnotados->second;
				datos_rastreados = ItmRastreados->second;
				
				vector<TrackPoint>::iterator ItAnotados,ItRastreados;
				

				for (ItAnotados = datos_anotados.begin(); ItAnotados != datos_anotados.end(); ItAnotados++, gt_objects++) {	
					for (ItRastreados = datos_rastreados.begin(); ItRastreados != datos_rastreados.end(); ItRastreados++) {
						///Determine the misses			
						if(*ItRastreados==*ItAnotados){	
				
							///Determine missmatches
							if((ItRastreados->point.x > ItAnotados->point.x+missmatch_error) ||
							   (ItRastreados->point.x < ItAnotados->point.x-missmatch_error) ||
							   (ItRastreados->point.y > ItAnotados->point.y+missmatch_error) ||
							   (ItRastreados->point.y < ItAnotados->point.y-missmatch_error))
							   missmatches++;
							else
								true_positives++;
				
							break;
						}
						
						else if (*ItRastreados==datos_rastreados.back()){///Llegue al ultimo y no se encontro
							//cout << "Miss of player "<< ItAnotados->label << " on frame " <<  ItRastreados->frame_number << endl;
							misses++;
						}						
						
					}
					///Create a list of the gt frame numbers
					if(ItAnotados->frame_number != fnumber){
						gt_frames.push_back(ItAnotados->frame_number);
						fnumber = ItAnotados->frame_number;
					}					

				}				
				///Count track objects only on gt frames
				for(unsigned int gtc =0; gtc<gt_frames.size();gtc++){
					int gtfnumber = gt_frames[gtc];
					for (ItRastreados = datos_rastreados.begin(); ItRastreados != datos_rastreados.end(); ItRastreados++) {
						if(gtfnumber==ItRastreados->frame_number)
							tk_objects++;
					}
				}
				
			}///if son el mismo frame number
		}
	}

	//false_positives = tk_objects - gt_objects;
	false_positives = (tk_objects>=gt_objects) ? (tk_objects - gt_objects):(0);
	//false_negatives = gt_objects - tk_objects;
	false_negatives = (gt_objects >= tk_objects) ? (gt_objects - tk_objects):(0);
	missmatches=missmatches/2;
	
	precision = true_positives/(true_positives+false_positives);
	recall = true_positives/(true_positives+false_negatives);
	f1measure = 2 * (precision * recall)/(precision + recall);
	
	cout << "Misses = " << misses << endl;
	cout << "Ture positive = " << true_positives << endl;
	cout << "False positives = " << false_positives << endl;
	cout << "False negatives = " << false_negatives << endl;
	cout << "Missmatches = " << missmatches << endl;
	cout << "Total analyzed frames = " << gt_frames.size() << endl;
	cout << "Total ground truth objects = " << gt_objects << endl;

	mota = 1 - (cm * misses + cf * false_positives + cs * missmatches)/gt_objects;
	
	return mota;	
}

double Measurer::MeasureMota(vector< TrackPoint > anotados, vector< TrackPoint > rastreados)
{
	cout << "Analyzing tracking results for MOTA..." << endl;
	///Sort trackpoints by frame number
	std::sort(anotados.begin(), anotados.end());
	std::sort(rastreados.begin(), rastreados.end());
	

	vector<TrackPoint>::iterator ItAnotados,ItRastreados;

	std::vector<int> gt_frames;
	int fnumber = -1;  
	bool found1,found2,found3,found4;
	for (ItAnotados = anotados.begin(); ItAnotados != anotados.end(); ItAnotados++) {
	  if(ItAnotados->frame_number>=start_frame && ItAnotados->frame_number<=stop_frame){	
		gt_objects++; found1 = FALSE; found2 = FALSE; found3 = FALSE;
		for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
		  if((ItRastreados->frame_number>=start_frame) && (ItRastreados->frame_number<=stop_frame) && (found1 == FALSE))	{
			
			///Determine the missmatches			
			if(*ItRastreados==*ItAnotados){			
				//cout << "The player with id " << ItRastreados->label << " found on frame " <<  ItRastreados->frame_number << endl;
				
				///Determine missmatches
				if((ItRastreados->point.x > ItAnotados->point.x+missmatch_error) ||
				   (ItRastreados->point.x < ItAnotados->point.x-missmatch_error) ||
				   (ItRastreados->point.y > ItAnotados->point.y+missmatch_error) ||
				   (ItRastreados->point.y < ItAnotados->point.y-missmatch_error))
				   missmatches++;
				//else
					//true_positives++;
				
				found1 = TRUE; //cout << "Found player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
			}
			else if (ItRastreados==rastreados.end()-1){///Determine the misses
				//cout << "Miss of player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
				misses++; found1 = FALSE;
			}
			
			///Search for FN - Cuando esta en el GT pero no en el SUT
			if((ItAnotados->frame_number == ItRastreados->frame_number) && (found2 == FALSE)){
				if((ItAnotados->point.x < ItRastreados->point.x+detection_error) &&
				   (ItAnotados->point.x > ItRastreados->point.x-detection_error) &&
				   (ItAnotados->point.y < ItRastreados->point.y+detection_error) &&
				   (ItAnotados->point.y > ItRastreados->point.y-detection_error))
				   found2 = TRUE; ///Found a coincidence
				else{
				   //cout << "found posible player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
				   found2 = FALSE;
				}
			}
			
			///Search for TP - Cuando esta en el GT y en el SUT
			if((ItAnotados->frame_number == ItRastreados->frame_number) && (found3 == FALSE)){
				if((ItRastreados->point.x < ItAnotados->point.x+detection_error) &&
				   (ItRastreados->point.x > ItAnotados->point.x-detection_error) &&
				   (ItRastreados->point.y < ItAnotados->point.y+detection_error) &&
				   (ItRastreados->point.y > ItAnotados->point.y-detection_error))
				   found3 = TRUE; ///Found a TP
			}
			
		  }
		  
		}
		if(found2 == FALSE){
			//cout << "False negative of player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
			false_negatives++;
		}
		if(found3 == TRUE){
			//cout << "True positive of player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
			true_positives++;
		}

		
		///Create a list of the gt frame numbers
		if(ItAnotados->frame_number != fnumber){
			gt_frames.push_back(ItAnotados->frame_number);
			fnumber = ItAnotados->frame_number;
		}			

	  }
	}

/*
	///Count track objects only on gt frames
	for(unsigned int gtc =0; gtc<gt_frames.size();gtc++){
		int gtfnumber = gt_frames[gtc];
		for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
			if(ItRastreados->frame_number>=start_frame && ItRastreados->frame_number<=stop_frame) {
				if(gtfnumber==ItRastreados->frame_number)
					tk_objects++;
			}
		}
	}
*/

	///Search for FP - Cuando esta en el SUT y no en el GT
	for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
		if((ItRastreados->frame_number>=start_frame) && (ItRastreados->frame_number<=stop_frame))	{
			tk_objects++; found4 = FALSE;
			
			for (ItAnotados = anotados.begin(); ItAnotados != anotados.end(); ItAnotados++) {
				if(ItAnotados->frame_number>=start_frame && ItAnotados->frame_number<=stop_frame){

					///Search for FP
					if((ItRastreados->frame_number == ItAnotados->frame_number) && (found4 == FALSE)){
						if((ItRastreados->point.x < ItAnotados->point.x+detection_error) &&
						   (ItRastreados->point.x > ItAnotados->point.x-detection_error) &&
						   (ItRastreados->point.y < ItAnotados->point.y+detection_error) &&
						   (ItRastreados->point.y > ItAnotados->point.y-detection_error))
								found4 = TRUE; ///El SUT esta en el GT
					}					
				}
			}
			if(found4 == FALSE){
				//cout << "False positive of player "<< ItRastreados->label << " on frame " <<  ItRastreados->frame_number << endl;
				false_positives++;
			}			
			
		}
	}

	//false_positives = (tk_objects>=gt_objects) ? (tk_objects - gt_objects):(0);
	missmatches=missmatches/2.0; 
	
	precision = true_positives/(true_positives+false_positives);
	recall = true_positives/(true_positives+false_negatives);
	f1measure = 2.0 * (precision * recall)/(precision + recall);
	
	cout << "Total ground truth objects = " << gt_objects << endl;
	cout << "Total SUT objects = " << tk_objects << endl;
	cout << "Misses = " << misses << endl;
	cout << "True positive = " << true_positives << endl;
	cout << "False positives = " << false_positives << endl;
	cout << "False negatives = " << false_negatives << endl;
	cout << "Missmatches = " << missmatches << endl;
	cout << "Total analyzed frames = " << gt_frames.size() << endl;	

	mota = 1.0 - (cm * misses + cf * false_positives + cs * missmatches)/gt_objects;
	cout << "Finish with MOTA..." << endl;
	return mota;

}

double Measurer::MeasureMota(vector< TrackPoint > anotados, vector< TrackPoint > rastreados, int tid)
{
	
	///Sort trackpoints by frame number
	std::sort(anotados.begin(), anotados.end());
	std::sort(rastreados.begin(), rastreados.end());
	std::vector<int> gt_frames;
	
	if(tid == 0){
	cout << "Analyzing tracking results for MOTA..." << endl;
	vector<TrackPoint>::iterator ItAnotados,ItRastreados;

	
	int fnumber = -1;  
	bool found1,found2,found3;
	for (ItAnotados = anotados.begin(); ItAnotados != anotados.end(); ItAnotados++) {
	  if(ItAnotados->frame_number>=start_frame && ItAnotados->frame_number<=stop_frame){	
		gt_objects++; found1 = FALSE; found2 = FALSE; found3 = FALSE;
		for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
		  if((ItRastreados->frame_number>=start_frame) && (ItRastreados->frame_number<=stop_frame) && (found1 == FALSE))	{
			
			///Determine the missmatches			
			if(*ItRastreados==*ItAnotados){			
				//cout << "The player with id " << ItRastreados->label << " found on frame " <<  ItRastreados->frame_number << endl;
				
				///Determine missmatches
				if((ItRastreados->point.x > ItAnotados->point.x+missmatch_error) ||
				   (ItRastreados->point.x < ItAnotados->point.x-missmatch_error) ||
				   (ItRastreados->point.y > ItAnotados->point.y+missmatch_error) ||
				   (ItRastreados->point.y < ItAnotados->point.y-missmatch_error))
				   missmatches++;
				//else
					//true_positives++;
				
				found1 = TRUE; //cout << "Found player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
			}
			else if (ItRastreados==rastreados.end()-1){///Determine the misses
				//cout << "Miss of player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
				misses++; found1 = FALSE;
			}
			
			///Search for FN - Cuando esta en el GT pero no en el SUT
			if((ItAnotados->frame_number == ItRastreados->frame_number) && (found2 == FALSE)){
				if((ItAnotados->point.x < ItRastreados->point.x+detection_error) &&
				   (ItAnotados->point.x > ItRastreados->point.x-detection_error) &&
				   (ItAnotados->point.y < ItRastreados->point.y+detection_error) &&
				   (ItAnotados->point.y > ItRastreados->point.y-detection_error))
				   found2 = TRUE; ///Found a coincidence
				else{
				   //cout << "found posible player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
				   found2 = FALSE;
				}
			}
			
			///Search for TP - Cuando esta en el GT y en el SUT
			if((ItAnotados->frame_number == ItRastreados->frame_number) && (found3 == FALSE)){
				if((ItRastreados->point.x < ItAnotados->point.x+detection_error) &&
				   (ItRastreados->point.x > ItAnotados->point.x-detection_error) &&
				   (ItRastreados->point.y < ItAnotados->point.y+detection_error) &&
				   (ItRastreados->point.y > ItAnotados->point.y-detection_error))
				   found3 = TRUE; ///Found a TP
			}
			
		  }
		  
		}
		if(found2 == FALSE){
			//cout << "False negative of player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
			false_negatives++;
		}
		if(found3 == TRUE){
			//cout << "True positive of player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
			true_positives++;
		}

		
		///Create a list of the gt frame numbers
		if(ItAnotados->frame_number != fnumber){
			gt_frames.push_back(ItAnotados->frame_number);
			fnumber = ItAnotados->frame_number;
		}			

	  }
	}
	cout << "Finish with MOTA on thread " << tid << endl;
	}///tid=0

	///Search for FP - Cuando esta en el SUT y no en el GT
	if(tid==1){
	vector<TrackPoint>::iterator ItAnotados,ItRastreados;
	bool found4;
	for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
		if((ItRastreados->frame_number>=start_frame) && (ItRastreados->frame_number<=stop_frame))	{
			tk_objects++; found4 = FALSE;
			
			for (ItAnotados = anotados.begin(); ItAnotados != anotados.end(); ItAnotados++) {
				if(ItAnotados->frame_number>=start_frame && ItAnotados->frame_number<=stop_frame){

					///Search for FP
					if((ItRastreados->frame_number == ItAnotados->frame_number) && (found4 == FALSE)){
						if((ItRastreados->point.x < ItAnotados->point.x+detection_error) &&
						   (ItRastreados->point.x > ItAnotados->point.x-detection_error) &&
						   (ItRastreados->point.y < ItAnotados->point.y+detection_error) &&
						   (ItRastreados->point.y > ItAnotados->point.y-detection_error))
								found4 = TRUE; ///El SUT esta en el GT
					}					
				}
			}
			if(found4 == FALSE){
				//cout << "False positive of player "<< ItRastreados->label << " on frame " <<  ItRastreados->frame_number << endl;
				false_positives++;
			}			
			
		}
	}
	cout << "Finish with MOTA on thread " << tid << endl;
	}///tid = 1
	#pragma omp barrier

	//false_positives = (tk_objects>=gt_objects) ? (tk_objects - gt_objects):(0);
	missmatches=missmatches/2.0; 
	
	precision = true_positives/(true_positives+false_positives);
	recall = true_positives/(true_positives+false_negatives);
	f1measure = 2.0 * (precision * recall)/(precision + recall);
	
	cout << "Total ground truth objects = " << gt_objects << endl;
	cout << "Total SUT objects" << tk_objects << endl;
	cout << "Misses = " << misses << endl;
	cout << "True positive = " << true_positives << endl;
	cout << "False positives = " << false_positives << endl;
	cout << "False negatives = " << false_negatives << endl;
	cout << "Missmatches = " << missmatches << endl;
	cout << "Total analyzed frames = " << gt_frames.size() << endl;	

	mota = 1.0 - (cm * misses + cf * false_positives + cs * missmatches)/gt_objects;
	cout << "Finish with MOTA..." << endl;
	return mota;

}

double Measurer::MeasureMota_old(vector< TrackPoint > anotados, vector< TrackPoint > rastreados)
{
	cout << "Analyzing tracking results for MOTA..." << endl;
	///Sort trackpoints by frame number
	std::sort(anotados.begin(), anotados.end());
	std::sort(rastreados.begin(), rastreados.end());
	

	vector<TrackPoint>::iterator ItAnotados,ItRastreados;


	std::vector<int> gt_frames;
	int fnumber = -1;  bool found;
	for (ItAnotados = anotados.begin(); ItAnotados != anotados.end(); ItAnotados++) {
	  if(ItAnotados->frame_number>=start_frame && ItAnotados->frame_number<=stop_frame){	
		gt_objects++; found = FALSE;
		for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
		  if((ItRastreados->frame_number>=start_frame) && (ItRastreados->frame_number<=stop_frame) && (found == FALSE))	{
			
			///Determine the misses			
			if(*ItRastreados==*ItAnotados){			
				//cout << "The player with id " << ItRastreados->label << " found on frame " <<  ItRastreados->frame_number << endl;
				
				///Determine missmatches
				if((ItRastreados->point.x > ItAnotados->point.x+missmatch_error) ||
				   (ItRastreados->point.x < ItAnotados->point.x-missmatch_error) ||
				   (ItRastreados->point.y > ItAnotados->point.y+missmatch_error) ||
				   (ItRastreados->point.y < ItAnotados->point.y-missmatch_error))
				   missmatches++;
				else
					true_positives++;
				
				found = TRUE; //cout << "Found player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
			}
			//else if (*ItRastreados==rastreados.back()){
			else if (ItRastreados==rastreados.end()-1){
				//cout << "Miss of player "<< ItAnotados->label << " on frame " <<  ItAnotados->frame_number << endl;
				misses++; found = FALSE;
			}
		  }
		  ///Search for FP
		}
		///Create a list of the gt frame numbers
		if(ItAnotados->frame_number != fnumber){
			gt_frames.push_back(ItAnotados->frame_number);
			fnumber = ItAnotados->frame_number;
		}
	  }
	}
	
	///Count track objects only on gt frames
	for(unsigned int gtc =0; gtc<gt_frames.size();gtc++){
		int gtfnumber = gt_frames[gtc];
		for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
			if(ItRastreados->frame_number>=start_frame && ItRastreados->frame_number<=stop_frame) {
				if(gtfnumber==ItRastreados->frame_number)
					tk_objects++;
			}
		}
	}

	
	//gt_objects = anotados.size();
	//tk_objects = rastreados.size();
	
	//false_positives = tk_objects - gt_objects;
	false_positives = (tk_objects>=gt_objects) ? (tk_objects - gt_objects):(0);
	//false_negatives = gt_objects - tk_objects;
	false_negatives = (gt_objects >= tk_objects) ? (gt_objects - tk_objects):(0);
	missmatches=missmatches/2.0; 
	
	precision = true_positives/(true_positives+false_positives);
	recall = true_positives/(true_positives+false_negatives);
	f1measure = 2.0 * (precision * recall)/(precision + recall);
	
	cout << "Total ground truth objects = " << gt_objects << endl;
	cout << "Total SUT objects" << tk_objects << endl;
	cout << "Misses = " << misses << endl;
	cout << "True positive = " << true_positives << endl;
	cout << "False positives = " << false_positives << endl;
	cout << "False negatives = " << false_negatives << endl;
	cout << "Missmatches = " << missmatches << endl;
	cout << "Total analyzed frames = " << gt_frames.size() << endl;	

	mota = 1.0 - (cm * misses + cf * false_positives + cs * missmatches)/gt_objects;
	
	return mota;

}

double Measurer::MeasureMotp(vector< TrackPoint > anotados, vector< TrackPoint > rastreados)
{

	cout << "Analyzing tracking results for MOTP..." << endl;
	///Sort trackpoints by frame number
	std::sort(anotados.begin(), anotados.end());
	std::sort(rastreados.begin(), rastreados.end());
	

	vector<TrackPoint>::iterator ItAnotados,ItRastreados;

	//Change this for a binary search!!!
	double distance_error = 0.0;
	for (ItAnotados = anotados.begin(); ItAnotados != anotados.end(); ItAnotados++) {
	  if(ItAnotados->frame_number>=start_frame && ItAnotados->frame_number<=stop_frame){		
		for (ItRastreados = rastreados.begin(); ItRastreados != rastreados.end(); ItRastreados++) {
		  if(ItRastreados->frame_number>=start_frame && ItRastreados->frame_number<=stop_frame)	{		
			if(*ItRastreados==*ItAnotados){
				motp_matches++;
				
				///Obtain euclidian distance
				double x = (double) (ItAnotados->point.x - ItRastreados->point.x);
				double y = (double) (ItAnotados->point.y - ItRastreados->point.y);
				
				double dist;
				dist = pow(x, 2) + pow(y, 2);       
				dist = sqrt(dist);
				dist = (dist-motp_error>=0.0) ? (dist-motp_error):(0.0);     
				
				distance_error=distance_error+dist;
				
				break;
			}
		  }
		}
	  }
	}	
	
	motp = distance_error/motp_matches;
	cout << "Finish with MOTP..." << endl;
	return motp;
}


void Measurer::SaveResults(std::string report_file,std::string input_file,std::string annotation_file)
{
	cout<< "Saving results to " << report_file << " ... " << endl;
	ofstream rfile;

	rfile.open (report_file);
	
	rfile <<"--- Resulting report ---" << endl;
	
	rfile << "\n- Input files -" << endl;
	rfile <<"Tracking file: " << input_file << endl;
	rfile <<"GT file: " << annotation_file << endl;
	
	rfile <<"\n- Configuration parameters -" << endl;
	rfile <<"Miss constant = " << cm << endl;
	rfile <<"False positive constant = " << cf << endl;
	rfile <<"Missmatch constant = " << cs << endl;
	rfile <<"Precision error = " << motp_error << endl;
	rfile <<"Mismatch error = " << missmatch_error << endl;
	rfile <<"Detection error = " << detection_error << endl;
	
	rfile <<"\n- Metrics values -" << endl;
	if(report_level == BASIC_LEVEL){
		int space = 12;
		rfile << std::setw(space) << "F1" 
		      << std::setw(space) << "MOTA" 
		      << std::setw(space) << "MOTP" << endl;
		rfile << std::setw(space) << to_string_with_precision(f1measure) 
			  << std::setw(space) << to_string_with_precision((double)mota*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision(motp) + " px" << endl;
	}
	
	else if(report_level == FULL_LEVEL){
		int space = 12;
		rfile << std::setw(space) << "GTobj" 
			  << std::setw(space) << "SUTobj" 
			  << std::setw(space) << "FP" 
			  << std::setw(space) << "FN" 
			  << std::setw(space) << "TP"
		      << std::setw(space) << "Rcll"
		      << std::setw(space) << "Prcn" 
		      << std::setw(space) << "F1"  << endl;
		rfile << std::setw(space) << to_string_with_precision(gt_objects,0)
			  << std::setw(space) << to_string_with_precision(tk_objects,0)
			  << std::setw(space) << to_string_with_precision((double)false_positives/(double)tk_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision((double)false_negatives/(double)gt_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision((double)true_positives/(double)gt_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision(recall) 
			  << std::setw(space) << to_string_with_precision(precision) 
			  << std::setw(space) << to_string_with_precision(f1measure) << endl;

	}
	else if(report_level == FULLP_LEVEL){
		int space = 12;
		rfile << std::setw(space) << "GTobj" 
			  << std::setw(space) << "SUTobj" 
			  << std::setw(space) << "FP" 
			  << std::setw(space) << "FN" 
			  << std::setw(space) << "TP"
			  << std::setw(space) << "MS" 
		      << std::setw(space) << "MSM"
		      << std::setw(space) << "Rcll"
		      << std::setw(space) << "Prcn" 
		      << std::setw(space) << "F1" 
		      << std::setw(space) << "MOTA" 
		      << std::setw(space) << "MOTP" << endl;
		rfile << std::setw(space) << to_string_with_precision(gt_objects,0)
			  << std::setw(space) << to_string_with_precision(tk_objects,0)
			  << std::setw(space) << to_string_with_precision((double)false_positives/(double)tk_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision((double)false_negatives/(double)gt_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision((double)true_positives/(double)gt_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision((double)misses/(double)gt_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision((double)missmatches/(double)gt_objects*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision(recall) 
			  << std::setw(space) << to_string_with_precision(precision) 
			  << std::setw(space) << to_string_with_precision(f1measure) 
			  << std::setw(space) << to_string_with_precision((double)mota*100.0) + "\%" 
			  << std::setw(space) << to_string_with_precision(motp) + " px" << endl;
	}
}

double Measurer::CalculateMean(vector< double > data)
{
	double sum=0.0;
	double mean=0.0;
	
	for(unsigned int i=0; i<data.size();i++)
		sum += data[i];
		
	mean = sum/data.size();
	
	return mean;	
}

double Measurer::CalculateVariance(vector< double > data)
{
	double sum=0.0;
	double mean=0.0;
	double variance=0.0;
	
	for(unsigned int i=0; i<data.size();i++)
		sum += data[i];
		
	mean = sum/data.size();
	
	for(unsigned int j=0; j<data.size();j++)
		variance += pow(data[j] - mean, 2);
		
	variance=variance/data.size();
	
	return variance;	
}

Measurer::~Measurer()
{
	
}
