#include "cv.h"
#include <opencv2/opencv.hpp>

void cvWindow(const char *name, int x, int y, int w, int h) {
	namedWindow(name, CV_WINDOW_NORMAL | CV_GUI_NORMAL);
	moveWindow(name, x, y);
	resizeWindow(name, w, h);
}


void msgBox(const char *msg) {
	Mat win = Mat::zeros(200, 1200, CV_8UC1);
	putText(win, msg, Point(50,50), FONT_HERSHEY_COMPLEX, 1, 255, 2);
	imshow("Mensaje", win);
	moveWindow("Mensaje", 500, 0);
	waitKey();
	destroyWindow("Mensaje");
}
