#include "Segmentator.h"
#include "Defs.h"

using namespace cv;
using namespace std;

Segmentator::Segmentator(void)
{
		hue_min = 60; 
		hue_max = 180; 
		porcentaje_espurias = 9;
		area_max_threshold=9999;
		porcentaje_espuriasmog = 9;
		tamano = 9;
		umbral_hue = 93;
		mode = PRISMODE;
		history = 100;
		learningRate = 13;
		varThreshold = 16;
		enable_morph_line_remover = 0;
		hmorph_axis = 0;
		vmorph_axis = 0;
		enable_hough_line_remover = 0;
		line_kernel = 10;
		minLineLength = 50;
		maxLineGap = 20;
		shape_contour_remover = 0;
		bShadowDetection = 0;
		enable_manual_field_selection = 0;
		enable_white_remover = 0;


		if((mode == MOG2MODE) || (mode == PRISMOG2MODE)){
			if(mode == MOG2MODE)
				cout << "Segmentation mode: MOG2" << endl;
			else
				cout << "Segmentation mode: PRISMOG2" << endl;
				
			mogbg = new BackgroundSubtractorMOG2(history,varThreshold,bShadowDetection);
		}
		else
			cout << "Segmentation mode: PRIS" << endl;		
}

Segmentator::Segmentator(std::map<std::string,int> parameters)
{
		hue_min = parameters["hue_min"]; 
		hue_max = parameters["hue_max"]; 
		porcentaje_espurias = parameters["porcentaje_espurias"];
		area_max_threshold=parameters["area_max_threshold"];
		porcentaje_espuriasmog = parameters["porcentaje_espuriasmog"];
		tamano = parameters["tamano_kernel"];
		umbral_hue = parameters["umbral_hue"];
		mode = parameters["mode"];
		history = parameters["history"];
		bShadowDetection = parameters["bShadowDetection"];
		learningRate = parameters["learningRate"];
		varThreshold = parameters["varThreshold"];
		gaussian_kernel = parameters["gaussian_kernel"];
		enable_morph_line_remover = parameters["enable_morph_line_remover"];
		hmorph_axis = parameters["hmorph_axis"];
		vmorph_axis = parameters["vmorph_axis"];
		enable_hough_line_remover = parameters["enable_hough_line_remover"];
		line_kernel = parameters["line_kernel"];
		minLineLength = parameters["minLineLength"];
		maxLineGap = parameters["maxLineGap"];
		shape_contour_remover = parameters["shape_contour_remover"];
		enable_morph_operations = parameters["enable_morph_operations"];
		morph_erode_kernel = parameters["morph_erode_kernel"];
		morph_dilate_kernel = parameters["morph_dilate_kernel"];
		video_height = parameters["video_height"];
		video_width = parameters["video_width"];
		enable_manual_field_selection = parameters["enable_manual_field_selection"];
		enable_white_remover = parameters["enable_white_remover"];
		top_left_point.x = parameters["top_left_point_x"];
		top_left_point.y = parameters["top_left_point_y"];
		top_right_point.x = parameters["top_right_point_x"];
		top_right_point.y = parameters["top_right_point_y"];
		bottom_left_point.x = parameters["bottom_left_point_x"];
		bottom_left_point.y = parameters["bottom_left_point_y"];
		bottom_right_point.x = parameters["bottom_right_point_x"];
		bottom_right_point.y = parameters["bottom_right_point_y"];

		if((mode == MOG2MODE) || (mode == PRISMOG2MODE)){
			if(mode == MOG2MODE)
				cout << "Segmentation mode: MOG2" << endl;
			else
				cout << "Segmentation mode: PRISMOG2" << endl;
				
			mogbg = new BackgroundSubtractorMOG2(history,varThreshold,bShadowDetection);
		}
		else
			cout << "Segmentation mode: PRIS" << endl;			
}

vector< vector< cv::Point > > Segmentator::PrisSegmentator( cv::Mat img_rgb )
{

	///imshow("Source RGB image", img_rgb); waitKey(0);
	//Se obtienen el Hue y Value de la imagen
	cv::normalize( img_rgb, img_rgb, 0, 1, cv::NORM_MINMAX, CV_32FC3, cv::Mat() );		
	cv::Mat img_hsv = ConvertColor( img_rgb, PRIS_RGB2HSV );
	///imshow("Source HSV image", img_hsv); waitKey(0);
	
	// Se elimina todo lo que esta en un rango de color blanco para quitar lineas de campo
	if(enable_white_remover){
		Mat color_mask;
		inRange(img_hsv, Scalar(0,0,95), Scalar(180,0,100), color_mask);
		img_hsv.setTo(Scalar(115,70,70),color_mask);
	}
	
	vector<cv::Mat> canales = Split( img_hsv );
	cv::Mat hue = canales[0];
	cv::Mat value = canales[2];

	// Se establece el tamano del kernel a utilizar para calcular la varianza espacial de la imagen
	if( tamano < 3 ) tamano = 3;
	tamano -= ( tamano + 1 ) % 2;

	//Se calcula la varianza y su logaritmo del Hue
	//y luego se umbraliza segun el parametro "umbral_hue"
	cv::Mat log_varianza_hue = LogVariance( hue, tamano );
	///imshow("Source HSV image", img_hsv); waitKey(0);
	cv::Mat mascara = Normalize( log_varianza_hue, PRIS_8UC1 );
	///imshow("Local Spatial Variance of Hue image", mascara); waitKey(0);
	mascara = Threshold( mascara, umbral_hue );
	///imshow("Local Spatial Variance image", mascara); waitKey(0);

	//Se crea una imagen binaria indicando cuales pixeles de la imagen fuente estan en el rango del color verde
	cv::Mat verde( value.rows, value.cols, CV_8UC1, cv::Scalar(0) );
	if( hue_min >= hue_max ) hue_min = hue_max - 1;
	for( int i=0; i < value.rows; i++ ) {
		for( int j=0; j < value.cols; j++ ) {
			if( hue.at<float>(i,j) > hue_min && hue.at<float>(i,j) <= hue_max ) verde.at<unsigned char>(i,j) = 255;
		}
	}
	///imshow("Green image", verde); waitKey(0);

	//Se calcula la varianza y su logaritmo del Value
	cv::Mat log_varianza_value = LogVariance( value, tamano );
	///imshow("Local Spatial Variance of value image", mascara); waitKey(0);

	//Se aplica un umbral establecido mediante el metodo de Otsu al logaritmo de la varianza espacial,
	// de modo que se crea una imagen binaria estableciendo que pixeles de la imagen fuente son homogeneneas
	cv::Mat homogeneo = Normalize( log_varianza_value, PRIS_8UC1 );
	homogeneo = OtsuThreshold( homogeneo );

	//Se crea una imagen binaria de la interseccion de las imagenes binarias homogena y verde,
	// de modo que se establece que secciones de la imagen corresponden al zacate
	homogeneo = ImageAnd( verde, homogeneo );
	///imshow("Homogeneous image", homogeneo); waitKey(0);

	if(enable_morph_line_remover){
		///Line detect con Morph operations
		//imshow("Homogeneous image", homogeneo); waitKey(0);
		
		Mat bw;
		adaptiveThreshold(~homogeneo, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);
		//imshow("binary", bw);
		/// Create the images that will use to extract the horizontal and vertical lines
		Mat horizontal = bw.clone();
		Mat vertical = bw.clone();
		
		/// Specify size on horizontal axis
		int horizontalsize = horizontal.cols / hmorph_axis;
		/// Create structure element for extracting horizontal lines through morphology operations
		Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize,1));
		/// Apply morphology operations
		erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
		dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));		
		//imshow("horizontal", horizontal);		
		
		/// Specify size on vertical axis
		int verticalsize = vertical.rows / vmorph_axis;
		/// Create structure element for extracting vertical lines through morphology operations
		Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));
		/// Apply morphology operations
		erode(vertical, vertical, verticalStructure, Point(-1, -1));
		dilate(vertical, vertical, verticalStructure, Point(-1, -1));
		//imshow("vertical", vertical);		
	
		cv::add( homogeneo, horizontal, homogeneo,cv::Mat(), CV_8UC1);
		cv::add( homogeneo, vertical, homogeneo,cv::Mat(), CV_8UC1);

		//imshow("homogeneo filtrado morph", homogeneo);	
	}

	if(enable_hough_line_remover){
		//Line detect con Hough
		Mat dst, cdst;
		Canny(homogeneo, dst, 50, 200, 3);
		//cvtColor(dst, cdst, CV_GRAY2BGR);
		dst.copyTo(cdst);
		vector<Vec4i> lines;
		HoughLinesP(dst, lines, 1, CV_PI/180, 50, minLineLength, maxLineGap );
	
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];
			line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255), 3, CV_AA);
		}
	
		//Remover lineas detectadas de homogeneo
		Mat kernel3=getStructuringElement(MORPH_OPEN,Size(3,3));
		Mat kernel4=getStructuringElement(MORPH_OPEN,Size(line_kernel,line_kernel));
		morphologyEx(cdst,cdst,CV_MOP_ERODE,kernel3,Point(-1, -1),1);	
		morphologyEx(cdst,cdst,CV_MOP_DILATE,kernel4,Point(-1, -1),1);	

		///imshow("Detected lines", cdst);waitKey(0);
		cv::add( homogeneo, cdst, homogeneo,cv::Mat(), CV_8UC1);
		///imshow("Fitered lines from field", homogeneo);waitKey(0);
	}
	
	if(enable_manual_field_selection){
		
		cv::Mat manual_field( value.rows, value.cols, CV_8UC1, cv::Scalar(0) );

		vector<Point> points;
		points.push_back(top_right_point);
		points.push_back(top_left_point);
		points.push_back(bottom_left_point);
		points.push_back(bottom_right_point);

		fillConvexPoly(manual_field, points, cv::Scalar(255),4);
		manual_field=~manual_field;
		///imshow("Manual field",manual_field); waitKey( 0 );
		///imshow("Homogeneo antes field",homogeneo); waitKey( 0 );
		cv::add( homogeneo, manual_field, homogeneo,cv::Mat(), CV_8UC1);
		///imshow("Homogeneo despues field",homogeneo); waitKey( 0 );
	}
	
	if(enable_morph_operations){
		Mat kernel5=getStructuringElement(MORPH_OPEN,Size(morph_erode_kernel,morph_erode_kernel));
		Mat kernel6=getStructuringElement(MORPH_OPEN,Size(morph_dilate_kernel,morph_dilate_kernel));
		morphologyEx(homogeneo,homogeneo,CV_MOP_ERODE,kernel5,Point(-1, -1),1);	
		morphologyEx(homogeneo,homogeneo,CV_MOP_DILATE,kernel6,Point(-1, -1),1);
	}
	//imshow("Morphological results", homogeneo);waitKey(0);

	//Se encuentran los contornos exteriores de la imagen binaria correspondiente al zacate
	vector< vector< cv::Point > > contornos_iniciales = Contours( homogeneo );

	//Se busca cual es el contorno que tiene el area mas grande 
	double area_max = 0.0;
	//double area_video = (double)video_height * (double)video_width;
	for( unsigned int i = 0; i < contornos_iniciales.size(); i++ ) {
		double area = ContourArea( contornos_iniciales[i] );
		if( (area > area_max)  ){
			area_max = area; 
		}
	}

	//Se agrupan todos los contornos cuya area sea de al menos 3% del area maxima y se calcula su Convex Hull
	vector< cv::Point > campo;
	for( unsigned int i = 0; i < contornos_iniciales.size(); i++ ) {
		if( ContourArea( contornos_iniciales[i] ) > ( 0.03 *  area_max ) ) {	//Magic Number, Siles usa 20%
			for( unsigned int j=0; j < contornos_iniciales[i].size(); j++ ) campo.push_back( contornos_iniciales[i][j] );
		}
	}
	campo = ConvexHull( campo );

	//Se encuentran las regiones correspondientes a los jugadores
	//( Estan dentro del campo, no son verdes y son heteregeneas )
	cv::Mat objetos( value.rows, value.cols, CV_8UC1, cv::Scalar(0) );
	for( int i=0; i < value.rows; i++ ) {
		for( int j=0; j < value.cols; j++ ) {
			//if ( ContainedInCountour( campo, cv::Point(j,i) ) >= 0 && verde.at<unsigned char>(i,j) == 0 && homogeneo.at<unsigned char>(i,j) == 0 ) objetos.at<unsigned char>(i,j) = 255;
			//if ( ContainedInCountour( campo, cv::Point(j,i) ) >= 0 && homogeneo.at<unsigned char>(i,j) == 0 ) objetos.at<unsigned char>(i,j) = 255;
			if ( ContainedInCountour( campo, cv::Point(j,i) ) >= 0 && homogeneo.at<unsigned char>(i,j) == 0 && mascara.at<unsigned char>(i,j) == 255 ) objetos.at<unsigned char>(i,j) = 255;
		}
	}

	//Se encuentran los contornos exteriores de la imagen binaria correspondiente a los blobs
	vector< vector< cv::Point > > contornos_blobs = Contours( objetos );
	vector< vector< cv::Point > > contornos_objetos;

	///Se eliminan los contornos que sean muy pequenos comparados con el tamana del contorno de area mayor,
	///ya que se consideran espurias. Tambien se eliminan aquellos que no son muy circulares para quitar
	///las lineas del campo por su contorno
	
	area_max = 1.0;
	for( unsigned int i = 0; i < contornos_blobs.size(); i++ ) {
		double area = ContourArea( contornos_blobs[i] );
		if( (area > area_max) && (area < area_max_threshold)) {
			area_max = area;
		}
	};
	
	double descriminar = porcentaje_espurias / 100.0 *  area_max;
	double descrimiarlineas = 0.0;
	if(shape_contour_remover>0)
		 descrimiarlineas = ((double)shape_contour_remover)/100.0; ///Con esto, se quitan lineas por forma, sin Hough

	
	//cout << "Area maxima: " << area_max << " Descriminar: " << descriminar << endl;
	//cout << "Cantidad de blobs detectados sin filtro de area:" << contornos_blobs.size() << endl;
	for( unsigned int i = 0; i < contornos_blobs.size(); i++ ) {
		double areac = ContourArea( contornos_blobs[i]);
		
		double perimetroc = ContourPerimeter( contornos_blobs[i]);
		double circularity = 4.0 * M_PI *  areac/(perimetroc*perimetroc) ;
		//cout << "Circularidad igual a " << circularity << " de " << i << endl;
		if( ( areac > descriminar ) && (circularity > descrimiarlineas)) { //Realmente se meten los que cumplen con ser mayores
		//if( areac  >  descriminar ) { 
			contornos_objetos.push_back( contornos_blobs[i] );
			//cout << "Un blob fuera." << endl;
		}
	}
/**	
	cv::Mat contornos_graf = DrawContour( contornos_objetos, value.rows, value.cols );
	contornos_graf = ConvertColor( contornos_graf, PRIS_GRY2RGB );
	cv::Mat detectado = Add( img_rgb, contornos_graf );
	detectado = Normalize( detectado, PRIS_8UC1 );
	imshow("Resulting image", detectado);waitKey(0);
**/
	return contornos_objetos;
}


void Segmentator::MOG2UpdateFgModel( cv::Mat frame )
{
		
	//update the background model	
	mogbg->operator()(frame, foreground_model,(double)learningRate/1000.0); //Hack 0.015 para ajustar segmentacion
	Mat kernel=getStructuringElement(MORPH_ELLIPSE,Size(5,5));
	Mat kernel2=getStructuringElement(MORPH_ELLIPSE,Size(10,10));
	morphologyEx(foreground_model,foreground_model,CV_MOP_ERODE,kernel);
	morphologyEx(foreground_model,foreground_model,CV_MOP_DILATE,kernel2);	
	GaussianBlur(foreground_model, foreground_model, Size(gaussian_kernel,gaussian_kernel), 3.5,3.5);
		
	//threshold(foreground_model, foreground_model, 10,255,THRESH_BINARY);
	
	//show the current frame and the fg masks
	//imshow("Frame", frame);
	//imshow("FG Mask MOG 2", foreground_model); waitKey(0);
	//Mat backgroundImage;
	//mogbg.getBackgroundImage( backgroundImage);
	//imshow("Background image", backgroundImage); waitKey(0);
}


vector< vector< cv::Point > > Segmentator::MOG2Segmentator( cv::Mat img_rgb )
{		
	
	//cv::Mat img_hsv = ConvertColor( img_rgb, PRIS_RGB2HSV );
	MOG2UpdateFgModel(img_rgb);

	if(enable_morph_operations){
		Mat kernel5=getStructuringElement(MORPH_OPEN,Size(morph_erode_kernel,morph_erode_kernel));
		Mat kernel6=getStructuringElement(MORPH_OPEN,Size(morph_dilate_kernel,morph_dilate_kernel));
		morphologyEx(foreground_model,foreground_model,CV_MOP_ERODE,kernel5,Point(-1, -1),1);	
		morphologyEx(foreground_model,foreground_model,CV_MOP_DILATE,kernel6,Point(-1, -1),1);
	}
	

	vector< vector< cv::Point > > contornos_blobs = Contours( foreground_model );	
	vector< vector< cv::Point > > contornos_objetos;
	
	double area_max = 1.0;
	for( unsigned int i = 0; i < contornos_blobs.size(); i++ ) {
		double area = ContourArea( contornos_blobs[i] );
		if( area > area_max ) {
			area_max = area;
		}
	};
	
	double descriminar = porcentaje_espurias / 25.0 *  area_max; //Hack 25 para ajustar segmentacion
	for( unsigned int i = 0; i < contornos_blobs.size(); i++ ) {
		double areac = ContourArea( contornos_blobs[i]);
		if( areac  >  descriminar ) { 
			contornos_objetos.push_back( contornos_blobs[i] );
			//cout << "Un blob fuera." << endl;
		}
	}

	return contornos_objetos;		
}

vector< vector< cv::Point > > Segmentator::PrisMOG2Segmentator( cv::Mat img_rgb )
{		
	
	//cv::Mat img_hsv = ConvertColor( img_rgb, PRIS_RGB2HSV );
	MOG2UpdateFgModel(img_rgb);

	if(enable_morph_operations){
		Mat kernel5=getStructuringElement(MORPH_OPEN,Size(morph_erode_kernel,morph_erode_kernel));
		Mat kernel6=getStructuringElement(MORPH_OPEN,Size(morph_dilate_kernel,morph_dilate_kernel));
		morphologyEx(foreground_model,foreground_model,CV_MOP_ERODE,kernel5,Point(-1, -1),1);	
		morphologyEx(foreground_model,foreground_model,CV_MOP_DILATE,kernel6,Point(-1, -1),1);
	}
	
	///imshow("Source RGB image", img_rgb); waitKey(0);
	//Se obtienen el Hue y Value de la imagen
	cv::normalize( img_rgb, img_rgb, 0, 1, cv::NORM_MINMAX, CV_32FC3, cv::Mat() );		
	cv::Mat img_hsv = ConvertColor( img_rgb, PRIS_RGB2HSV );
	///imshow("Source HSV image", img_hsv); waitKey(0);
	vector<cv::Mat> canales = Split( img_hsv );
	cv::Mat hue = canales[0];
	cv::Mat value = canales[2];

	// Se establece el tamano del kernel a utilizar para calcular la varianza espacial de la imagen
	if( tamano < 3 ) tamano = 3;
	tamano -= ( tamano + 1 ) % 2;

	//Se calcula la varianza y su logaritmo del Hue
	//y luego se umbraliza segun el parametro "umbral_hue"
	cv::Mat log_varianza_hue = LogVariance( hue, tamano );
	///imshow("Source HSV image", img_hsv); waitKey(0);
	cv::Mat mascara = Normalize( log_varianza_hue, PRIS_8UC1 );
	///imshow("Local Spatial Variance of Hue image", mascara); waitKey(0);
	mascara = Threshold( mascara, umbral_hue );
	///imshow("Local Spatial Variance image", mascara); waitKey(0);

	//Se crea una imagen binaria indicando cuales pixeles de la imagen fuente estan en el rango del color verde
	cv::Mat verde( value.rows, value.cols, CV_8UC1, cv::Scalar(0) );
	if( hue_min >= hue_max ) hue_min = hue_max - 1;
	for( int i=0; i < value.rows; i++ ) {
		for( int j=0; j < value.cols; j++ ) {
			if( hue.at<float>(i,j) > hue_min && hue.at<float>(i,j) <= hue_max ) verde.at<unsigned char>(i,j) = 255;
		}
	}
	///imshow("Green image", verde); waitKey(0);

	//Se calcula la varianza y su logaritmo del Value
	cv::Mat log_varianza_value = LogVariance( value, tamano );
	///imshow("Local Spatial Variance of value image", mascara); waitKey(0);

	//Se aplica un umbral establecido mediante el metodo de Otsu al logaritmo de la varianza espacial,
	// de modo que se crea una imagen binaria estableciendo que pixeles de la imagen fuente son homogeneneas
	cv::Mat homogeneo = Normalize( log_varianza_value, PRIS_8UC1 );
	homogeneo = OtsuThreshold( homogeneo );

	//Se crea una imagen binaria de la interseccion de las imagenes binarias homogena y verde,
	// de modo que se establece que secciones de la imagen corresponden al zacate
	homogeneo = ImageAnd( verde, homogeneo );
	///imshow("Homogeneous image", homogeneo); waitKey(0);
	///imshow("Foreground image", ~foreground_model); waitKey(0);

	//Se combinan los metodos de segmentacion
	homogeneo = ImageAnd( ~foreground_model, homogeneo );
	///imshow("Combine image", homogeneo); waitKey(0);

	if(enable_morph_line_remover){
		///Line detect con Morph operations
		//imshow("Homogeneous image", homogeneo); waitKey(0);
		
		Mat bw;
		adaptiveThreshold(~homogeneo, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);
		//imshow("binary", bw);
		/// Create the images that will use to extract the horizontal and vertical lines
		Mat horizontal = bw.clone();
		Mat vertical = bw.clone();
		
		/// Specify size on horizontal axis
		int horizontalsize = horizontal.cols / hmorph_axis;
		/// Create structure element for extracting horizontal lines through morphology operations
		Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize,1));
		/// Apply morphology operations
		erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
		dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));		
		//imshow("horizontal", horizontal);		
		
		/// Specify size on vertical axis
		int verticalsize = vertical.rows / vmorph_axis;
		/// Create structure element for extracting vertical lines through morphology operations
		Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));
		/// Apply morphology operations
		erode(vertical, vertical, verticalStructure, Point(-1, -1));
		dilate(vertical, vertical, verticalStructure, Point(-1, -1));
		//imshow("vertical", vertical);		
	
		cv::add( homogeneo, horizontal, homogeneo,cv::Mat(), CV_8UC1);
		cv::add( homogeneo, vertical, homogeneo,cv::Mat(), CV_8UC1);

		//imshow("homogeneo filtrado morph", homogeneo);	
	}

	if(enable_hough_line_remover){
		//Line detect con Hough
		Mat dst, cdst;
		Canny(homogeneo, dst, 50, 200, 3);
		//cvtColor(dst, cdst, CV_GRAY2BGR);
		dst.copyTo(cdst);
		vector<Vec4i> lines;
		HoughLinesP(dst, lines, 1, CV_PI/180, 50, minLineLength, maxLineGap );
	
		for( size_t i = 0; i < lines.size(); i++ )
		{
			Vec4i l = lines[i];
			line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255), 3, CV_AA);
		}
	
		//Remover lineas detectadas de homogeneo
		Mat kernel3=getStructuringElement(MORPH_OPEN,Size(3,3));
		Mat kernel4=getStructuringElement(MORPH_OPEN,Size(line_kernel,line_kernel));
		morphologyEx(cdst,cdst,CV_MOP_ERODE,kernel3,Point(-1, -1),1);	
		morphologyEx(cdst,cdst,CV_MOP_DILATE,kernel4,Point(-1, -1),1);	

		///imshow("Detected lines", cdst);waitKey(0);
		cv::add( homogeneo, cdst, homogeneo,cv::Mat(), CV_8UC1);
		///imshow("Fitered lines from field", homogeneo);waitKey(0);
	}
	
	if(enable_manual_field_selection){
		
		cv::Mat manual_field( value.rows, value.cols, CV_8UC1, cv::Scalar(0) );

		vector<Point> points;
		points.push_back(top_left_point);
		points.push_back(top_right_point);
		points.push_back(bottom_left_point);
		points.push_back(bottom_right_point);

		fillConvexPoly(manual_field, points, cv::Scalar(255),4);
		manual_field=~manual_field;
		//imshow("Manual field",manual_field); waitKey( 0 );
		cv::add( homogeneo, manual_field, homogeneo,cv::Mat(), CV_8UC1);
	}
	
	if(enable_morph_operations){
		Mat kernel5=getStructuringElement(MORPH_OPEN,Size(morph_erode_kernel,morph_erode_kernel));
		Mat kernel6=getStructuringElement(MORPH_OPEN,Size(morph_dilate_kernel,morph_dilate_kernel));
		morphologyEx(homogeneo,homogeneo,CV_MOP_ERODE,kernel5,Point(-1, -1),1);	
		morphologyEx(homogeneo,homogeneo,CV_MOP_DILATE,kernel6,Point(-1, -1),1);
	}
	//imshow("Morphological results", homogeneo);waitKey(0);

	//Se encuentran los contornos exteriores de la imagen binaria correspondiente al zacate
	vector< vector< cv::Point > > contornos_iniciales = Contours( homogeneo );

	//Se busca cual es el contorno que tiene el area mas grande 
	double area_max = 0.0;
	//double area_video = (double)video_height * (double)video_width;
	for( unsigned int i = 0; i < contornos_iniciales.size(); i++ ) {
		double area = ContourArea( contornos_iniciales[i] );
		if( (area > area_max)  ){
			area_max = area; 
		}
	}

	//Se agrupan todos los contornos cuya area sea de al menos 3% del area maxima y se calcula su Convex Hull
	vector< cv::Point > campo;
	for( unsigned int i = 0; i < contornos_iniciales.size(); i++ ) {
		if( ContourArea( contornos_iniciales[i] ) > ( 0.03 *  area_max ) ) {	//Magic Number, Siles usa 20%
			for( unsigned int j=0; j < contornos_iniciales[i].size(); j++ ) campo.push_back( contornos_iniciales[i][j] );
		}
	}
	campo = ConvexHull( campo );

	//Se encuentran las regiones correspondientes a los jugadores
	//( Estan dentro del campo, no son verdes y son heteregeneas )
	cv::Mat objetos( value.rows, value.cols, CV_8UC1, cv::Scalar(0) );
	for( int i=0; i < value.rows; i++ ) {
		for( int j=0; j < value.cols; j++ ) {
			//if ( ContainedInCountour( campo, cv::Point(j,i) ) >= 0 && verde.at<unsigned char>(i,j) == 0 && homogeneo.at<unsigned char>(i,j) == 0 ) objetos.at<unsigned char>(i,j) = 255;
			//if ( ContainedInCountour( campo, cv::Point(j,i) ) >= 0 && homogeneo.at<unsigned char>(i,j) == 0 ) objetos.at<unsigned char>(i,j) = 255;
			if ( ContainedInCountour( campo, cv::Point(j,i) ) >= 0 && homogeneo.at<unsigned char>(i,j) == 0 && mascara.at<unsigned char>(i,j) == 255 ) objetos.at<unsigned char>(i,j) = 255;
		}
	}

	//Se encuentran los contornos exteriores de la imagen binaria correspondiente a los blobs
	vector< vector< cv::Point > > contornos_blobs = Contours( objetos );
	vector< vector< cv::Point > > contornos_objetos;

	///Se eliminan los contornos que sean muy pequenos comparados con el tamana del contorno de area mayor,
	///ya que se consideran espurias. Tambien se eliminan aquellos que no son muy circulares para quitar
	///las lineas del campo por su contorno
	
	area_max = 1.0;
	for( unsigned int i = 0; i < contornos_blobs.size(); i++ ) {
		double area = ContourArea( contornos_blobs[i] );
		if( (area > area_max) && (area < area_max_threshold)) {
			area_max = area;
		}
	};
	
	double descriminar = porcentaje_espurias / 100.0 *  area_max;
	double descrimiarlineas = 0.0;
	if(shape_contour_remover>0)
		 descrimiarlineas = ((double)shape_contour_remover)/100.0; ///Con esto, se quitan lineas por forma, sin Hough

	
	//cout << "Area maxima: " << area_max << " Descriminar: " << descriminar << endl;
	//cout << "Cantidad de blobs detectados sin filtro de area:" << contornos_blobs.size() << endl;
	for( unsigned int i = 0; i < contornos_blobs.size(); i++ ) {
		double areac = ContourArea( contornos_blobs[i]);
		
		double perimetroc = ContourPerimeter( contornos_blobs[i]);
		double circularity = 4.0 * M_PI *  areac/(perimetroc*perimetroc) ;
		if( ( areac > descriminar ) && (circularity > descrimiarlineas)) { //Realmente se meten los que cumplen con ser mayores
		//if( areac  >  descriminar ) { 
			contornos_objetos.push_back( contornos_blobs[i] );
			//cout << "Un blob fuera." << endl;
		}
	}
/**	
	cv::Mat contornos_graf = DrawContour( contornos_objetos, value.rows, value.cols );
	contornos_graf = ConvertColor( contornos_graf, PRIS_GRY2RGB );
	cv::Mat detectado = Add( img_rgb, contornos_graf );
	detectado = Normalize( detectado, PRIS_8UC1 );
	imshow("Resulting image", detectado);waitKey(0);
**/
	return contornos_objetos;
		
}

vector< vector< cv::Point > > Segmentator::Segment( cv::Mat img_rgb )
{	
	vector< vector< cv::Point > > contornos_iniciales;
	
	if(mode==PRISMODE)	
		contornos_iniciales = PrisSegmentator(img_rgb);
	else if(mode==MOG2MODE)			
		contornos_iniciales = MOG2Segmentator(img_rgb);	
	else if(mode==PRISMOG2MODE)			
		contornos_iniciales = PrisMOG2Segmentator(img_rgb);
	else{
		cout << "Unknown segmentation mode" << endl;
		exit(1);
	}	
	
	return contornos_iniciales;
}

Segmentator::~Segmentator()
{
	foreground_model.release();	
}

