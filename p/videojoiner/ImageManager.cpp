#include "ImageManager.h"
#include "Defs.h"

using namespace cv;
using namespace std;
using namespace boost::filesystem;

ImageManager::ImageManager()
{
	image_index=0;
	height=0;
	width=0;
	start_image=0;
	stop_image=0;
}


int ImageManager::LoadImage(string image_path)
{
	cout << "Loading image: " << image_path << endl;
	image = imread(image_path, CV_LOAD_IMAGE_COLOR);
	current_image = image_path;
	height=image.rows;
	width=image.cols;
	
    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
		return -1;
    }
    
    else
		return 0;
}
	
	
void ImageManager::LoadDirectory(string dir_path)
{
	D(cout << "Input image directory: " << dir_path << endl;)
	path p (dir_path);   
    directory_iterator di(p);
    directory_iterator di_end;

    while(di != di_end)
    {
		D(cout << "Input image filename loading: " << di->path().string() << endl;)
        fnames.push_back( di->path().string() );
        ++di;
    }
    std::sort(fnames.begin(),fnames.end());
}
	
	
Mat ImageManager::LoadImage(void)
{
	LoadImage(fnames[image_index]);
	Mat limage;
	image.copyTo(limage);
	image_index++;
	return image;
}

Mat ImageManager::GetImage(int nimage)
{
	image_index = nimage;
	LoadImage(fnames[image_index]);
	//Mat limage;
	//image.copyTo(limage);
	image_index++;
	return image;
}

Mat ImageManager::LoadPreviousImage(void)
{
	Mat pimage;
	if(image_index>0){
		image_index--;
			
	}
	return pimage;
}

void ImageManager::SaveImage(string save_path)
{
	imwrite( save_path, image );
}

void ImageManager::SaveImage(Mat simage, string save_path)
{
	imwrite( save_path, simage);
}

void ImageManager::SaveImage(Mat simage, int id)
{
	std::string sid;
	sid=std::to_string(id);
	size_t lastdot = current_image.find_last_of(".");
	if (lastdot != std::string::npos)
		current_image = current_image.substr(0, lastdot);  
	
	string img_name = current_image + sid + ".jpg";
	cout << "Saving image: " << img_name << endl;
	imwrite( img_name, simage);
}

void ImageManager::SaveImage(Mat simage, string save_dir, int id)
{
	if (!exists(save_dir))
		create_directory(save_dir);

	path imagep(current_image);
	string filename = imagep.stem().string();
	
	std::string sid;
	sid=std::to_string(id);
	
	
	string img_name = save_dir + filename +"_"+ sid + ".jpg";
	cout << "Saving image: " << img_name << endl;
	imwrite( img_name, simage);
}

void ImageManager::ShowImage(Mat vimage)
{
	namedWindow( "Display image", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	imshow("Display image", vimage);
	waitKey();	
}


void ImageManager::ShowImage(void)
{
	namedWindow( "Display image", CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	imshow("Display image", image);
	waitKey();
}


ImageManager::~ImageManager()
{
	
}
