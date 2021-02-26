
#include "Timer.h"


Timer::Timer()
{
	running = false;
	accum = 0;
	running_time = 0;
}


void Timer::Start()
{
	tag="Default";
	if (!running) {
		start_time = clock();
		running = true;
	}

}


void Timer::Start(string intag)
{
	tag=intag;
	if (!running) {
		start_time = GetTimeSeconds();
		running = true;
	}

}

void Timer::Stop()
{
	if (running) {
		double stop_time = GetTimeSeconds();
		accum += stop_time - start_time;
		running = false;
		cout << "Elapsed time for "<< tag << " = " << accum << " s" <<endl;
	}
}

void Timer::SStart(string intag)
{
	tag=intag;
	start_time = GetTimeSeconds();
}

void Timer::SStop()
{
	double stop_time = GetTimeSeconds();
	accum += stop_time - start_time;
	cout << "Elapsed time for "<< tag << " = " << accum << " s" <<endl;
}

double Timer::GetTimeSeconds()
{
	clock_t tv;
	tv = clock();
	return tv/CLOCKS_PER_SEC;
}



void Timer::CrnStart(string intag)
{
	tag=intag;
	if (!running) {
		start_time = GetTimeSeconds();
		running = true;
	}

}

void Timer::CrnPause()
{
	if (running) {
		double pause_time = GetTimeSeconds();
		running_time += (pause_time-start_time);
		running = false;
	}
	else
		cout << "Cant pause cronometer:" << tag << endl;
}

void Timer::CrnResume()
{
	if (!running) {
		start_time = GetTimeSeconds();
		running = true;
	}
	else
		cout << "Cant resume cronometer:" << tag << endl;
}

void Timer::CrnStop()
{
	if (running) {
		double stop_time = GetTimeSeconds();
		running_time += (stop_time-start_time);
		cout << "Elapsed time for "<< tag << " = " << running_time << " s" <<endl;
	}
	else
		cout << "Elapsed time for "<< tag << " = " << running_time << " s" <<endl;
		
}

Timer::~Timer()
{

}

