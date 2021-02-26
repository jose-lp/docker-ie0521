
#ifndef DOMINANT_HPP
#define DOMINANT_HPP

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <queue>

using namespace std;
using namespace cv;

typedef struct t_color_node {
    cv::Mat       mean;       // The mean of this node
    cv::Mat       cov;
    uchar         classid;    // The class ID

    t_color_node  *left;
    t_color_node  *right;
} t_color_node;

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

typedef Mat_<Vec3f> Mat3f;

cv::Mat get_dominant_palette(std::vector<cv::Vec3b> colors);
std::vector<t_color_node*> get_leaves(t_color_node *root);
std::vector<cv::Vec3b> get_dominant_colors(t_color_node *root);
int get_next_classid(t_color_node *root);
void get_class_mean_cov(cv::Mat img, cv::Mat classes, t_color_node *node);
void partition_class(cv::Mat img, cv::Mat classes, uchar nextid, t_color_node *node);
cv::Mat get_quantized_image(cv::Mat classes, t_color_node *root);
cv::Mat get_viewable_image(cv::Mat classes);
t_color_node* get_max_eigenvalue_node(t_color_node *current);
std::vector<cv::Vec3b> find_dominant_colors(cv::Mat img, int count);
std::vector<double> find_dominant_hcolors(cv::Mat rgb_roi, int how_many);
std::vector<double> find_dominant_hcolors2(cv::Mat rgb_roi, int how_many);
std::vector<double> find_dominant_hcolors3(cv::Mat rgb_roi, int how_many);
void RGB2HSV(float r, float g, float b,float &h, float &s, float &v);
hsv   rgb2hsv(rgb in);
rgb   hsv2rgb(hsv in);

#endif
