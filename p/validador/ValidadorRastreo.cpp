/**
 *  Herramienta para obtener las metricas de validacion a partir de un 
 *  archivo generado por un algoritmo y un archivo de anotacion manual.
 **/

//includes
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <queue>
#include <getopt.h>
#include "Defs.h"
#include "Structs.h"
#include "YamlParser.h"
#include "Measurer.h"
#include <omp.h>

//namespaces
using namespace std;
using namespace cv;

//global vars
std::string input_file = "input_file.yaml";
std::string annotation_file = "annotation_file.yaml";
std::string config_file = "config.yaml";
std::string report_file = "report_file.txt";

//function prototypes
void PrintHelp(void);
void ProcessArgs(int argc, char** argv);

//functions
///************************Funcion principal
int main(int argc, char *argv[])
{
	/// Init program: read args
	ProcessArgs(argc, argv);
	
	/// Init objetos
	YamlParser yp;
	yp.LoadMeasurerConfig(config_file);
	Measurer ms(yp.constants);
	
	/// Load data files
	vector< TrackPoint > datos_anotados, datos_rastreados;
	cout << "Loading GT data..." << endl; 
	datos_anotados = yp.LoadTrackingFile(annotation_file,"non");
	cout << "Loading tracking data..." << endl; 
	datos_rastreados = yp.LoadTrackingFile(input_file,"non");
	
	/// Change data structure of GT and tracking data for a faster comparison
	map<int, vector< TrackPoint >> mdatos_anotados, mdatos_rastreados;
	vector<TrackPoint>::iterator ItvAnotados,ItvRastreados;

/*	
	///Create the new GT structure
	for (ItvAnotados = datos_anotados.begin(); ItvAnotados != datos_anotados.end(); ItvAnotados++) {	
		
		if(mdatos_anotados.count(ItvAnotados->frame_number)==0)///Insert first element		
			mdatos_anotados[ItvAnotados->frame_number].push_back(*ItvAnotados);	
		else
			mdatos_anotados[ItvAnotados->frame_number].push_back(*ItvAnotados);		
		
	}
	
	///Create the new Track structure
	for (ItvRastreados = datos_rastreados.begin(); ItvRastreados != datos_rastreados.end(); ItvRastreados++) {	
		
		if(!mdatos_rastreados.count(ItvRastreados->frame_number))///Insert first element
			mdatos_rastreados[ItvRastreados->frame_number].push_back(*ItvRastreados);
		else
			mdatos_rastreados[ItvRastreados->frame_number].push_back(*ItvRastreados);		
	}	
*/	

	/// Obtain metrics
	double mota = 0.0; double motp = 0.0;
	
	omp_set_nested(1); 	omp_set_dynamic(0);	
	#pragma omp parallel num_threads(2)
	{
		int tid = omp_get_thread_num();
		if(tid==0)
			mota = ms.MeasureMota(datos_anotados, datos_rastreados);
			//mota = ms.MeasureMota(datos_anotados, datos_rastreados,tid);
		else if(tid==1)
			motp = ms.MeasureMotp(datos_anotados, datos_rastreados);
	}
	//#pragma omp barrier
	/// Output and save results
	cout << "MOTA value = " << mota << endl;
	cout << "MOTP value = " << motp << endl;
	ms.SaveResults(report_file, input_file, annotation_file);
	
	return 0;
}


///************************Funcion que imprime ayuda
void PrintHelp(void)
{
    std::cout <<
			"\n Arguments: \n"
            "-t <tfile>:       Tracking file. \n"            
            "-a <afile>:  Annotation ground truth file. \n"
            "-c <cfile>:      Configuration file. \n"
            "-r <rfile>:      Report/result file. \n"
            "--help:                Show help\n";
    
    exit(1);
}

///************************Funcion que procesa argumentos
void ProcessArgs(int argc, char** argv)
{
	
	if (argc < 4) 
        PrintHelp();        
	
    const char* const short_opts = "t:a:c:r:h";
    const option long_opts[] = {
            {"track", required_argument, nullptr, 't'},
            {"annotation", required_argument, nullptr, 'a'},
            {"config", optional_argument, nullptr, 'c'},
            {"report", optional_argument, nullptr, 'r'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, 0}
    };

    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
        {
        case 't':
            input_file = std::string(optarg);
            std::cout << "Tracking file is: " << input_file << std::endl;
            break;

        case 'a':
            annotation_file = std::string(optarg);
            std::cout << "Annoattion file set to: " << annotation_file << std::endl;
            break;

        case 'c':
            config_file = std::string(optarg);
            std::cout << "Configuration file set to: " << config_file << std::endl;
            break;

        case 'r':
            report_file = std::string(optarg);
            std::cout << "Report/result file set to: " << report_file << std::endl;
            break;

        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            PrintHelp();
            break;
        }

    }

}
