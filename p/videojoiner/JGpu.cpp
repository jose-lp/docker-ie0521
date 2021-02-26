#include "JGpu.h"

using namespace std;
using namespace cv;
using namespace cv::gpu;

JGpu::JGpu()
{
	gpu_enabled = 0;
		
}

void JGpu::Set(void)
{
	try{
		gpu_enabled = CheckGpu();
	}
	catch (const cv::Exception& e) {
		cerr << e.what() << endl;
	}
}

int JGpu::CheckGpu(void)
{
	gpu::DeviceInfo dinfo;
	int gpus = gpu::getCudaEnabledDeviceCount();
	int support = dinfo.isCompatible();
	//cout << " gpus: " << gpus << " support: " << support << " both: " << (gpus && support) << endl;

	return (gpus && support);
}

void JGpu::gpuwarpPerspective(const Mat& src_img, Mat& dst_img, const Mat& MTrans, Size dsize)
{
	if(gpu_enabled){
		//cout << "Doing gpu warp" << endl;

		GpuMat src_gpu, dst_gpu;

		src_gpu.upload(src_img);		
		dst_img.create(src_img.size(), src_img.type());
		
		gpu::warpPerspective(src_gpu, dst_gpu, MTrans, dsize, cv::INTER_LINEAR);

		dst_gpu.download(dst_img);
	}
	else
		warpPerspective(src_img,dst_img,MTrans,dsize);
}

void JGpu::gpuwarpAffine(const Mat& src_img, Mat& dst_img, const Mat& MTrans, Size dsize)
{
	if(gpu_enabled){
		//cout << "Doing gpu warp" << endl;

		GpuMat src_gpu, dst_gpu;

		src_gpu.upload(src_img);		
		dst_img.create(src_img.size(), src_img.type());
		
		gpu::warpAffine(src_gpu, dst_gpu, MTrans, dsize, cv::INTER_LINEAR);

		dst_gpu.download(dst_img);
	}
	else
		warpAffine(src_img,dst_img,MTrans,dsize);
}

void JGpu::gpucvtColor(const Mat& src_img, Mat& dst_img, int code)
{
	if(gpu_enabled){
		GpuMat src_gpu, dst_gpu;

		src_gpu.upload(src_img);		
		dst_img.create(src_img.size(), src_img.type());
		
		gpu::cvtColor(src_gpu, dst_gpu, code);

		dst_gpu.download(dst_img);
	}
	else		
		cvtColor(src_img,dst_img,code);
}

void JGpu::gpuequalizeHist(const Mat& src_img, Mat& dst_img)
{
	if(gpu_enabled){
		GpuMat src_gpu, dst_gpu;

		src_gpu.upload(src_img);		
		dst_img.create(src_img.size(), src_img.type());
		
		gpu::equalizeHist(src_gpu, dst_gpu);

		dst_gpu.download(dst_img);
	}
	else	
		equalizeHist(src_img, dst_img);	
}


JGpu::~JGpu()
{
	
}
