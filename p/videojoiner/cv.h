/**
 * @file cv.h
 * @brief Interfaz de OpenCV y funciones utilitarias
 */

#ifndef CV_H
#define CV_H

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void cvWindow(const char *name, int x = 0, int y = 0, int w = 1280, int h = 720);
void msgBox(const char *msg);

#endif
