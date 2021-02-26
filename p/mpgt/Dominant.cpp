#include "Dominant.h"


cv::Mat get_dominant_palette(std::vector<cv::Vec3b> colors) {
    const int tile_size = 64;
    cv::Mat ret = cv::Mat(tile_size, tile_size*colors.size(), CV_8UC3, cv::Scalar(0));

    for(unsigned int i=0;i<colors.size();i++) {
        cv::Rect rect(i*tile_size, 0, tile_size, tile_size);
        cv::rectangle(ret, rect, cv::Scalar(colors[i][0], colors[i][1], colors[i][2]), CV_FILLED);
    }

    return ret;
}

std::vector<t_color_node*> get_leaves(t_color_node *root) {
    std::vector<t_color_node*> ret;
    std::queue<t_color_node*> queue;
    queue.push(root);

    while(queue.size() > 0) {
        t_color_node *current = queue.front();
        queue.pop();

        if(current->left && current->right) {
            queue.push(current->left);
            queue.push(current->right);
            continue;
        }

        ret.push_back(current);
    }

    return ret;
}

std::vector<cv::Vec3b> get_dominant_colors(t_color_node *root) {
    std::vector<t_color_node*> leaves = get_leaves(root);
    std::vector<cv::Vec3b> ret;

    for(unsigned int i=0;i<leaves.size();i++) {
        cv::Mat mean = leaves[i]->mean;
        ret.push_back(cv::Vec3b(mean.at<double>(0)*255.0f,
                                mean.at<double>(1)*255.0f,
                                mean.at<double>(2)*255.0f));
    }

    return ret;
}

int get_next_classid(t_color_node *root) {
    int maxid = 0;
    std::queue<t_color_node*> queue;
    queue.push(root);

    while(queue.size() > 0) {
        t_color_node* current = queue.front();
        queue.pop();

        if(current->classid > maxid)
            maxid = current->classid;

        if(current->left != NULL)
            queue.push(current->left);

        if(current->right)
            queue.push(current->right);
    }

    return maxid + 1;
}

void get_class_mean_cov(cv::Mat img, cv::Mat classes, t_color_node *node) {
    const int width = img.cols;
    const int height = img.rows;
    const uchar classid = node->classid;

    cv::Mat mean = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));
    cv::Mat cov = cv::Mat(3, 3, CV_64FC1, cv::Scalar(0));

    // We start out with the average color
    double pixcount = 0;
    for(int y=0;y<height;y++) {
        cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y);
        uchar* ptrClass = classes.ptr<uchar>(y);
        for(int x=0;x<width;x++) {
            if(ptrClass[x] != classid)
                continue;

            cv::Vec3b color = ptr[x];
            cv::Mat scaled = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));
            scaled.at<double>(0) = color[0]/255.0f;
            scaled.at<double>(1) = color[1]/255.0f;
            scaled.at<double>(2) = color[2]/255.0f;

            mean += scaled;
            cov = cov + (scaled * scaled.t());

            pixcount++;
        }
    }

    cov = cov - (mean * mean.t()) / pixcount;
    mean = mean / pixcount;

    // The node mean and covariance
    node->mean = mean.clone();
    node->cov = cov.clone();

    return;
}

void partition_class(cv::Mat img, cv::Mat classes, uchar nextid, t_color_node *node) {
    const int width = img.cols;
    const int height = img.rows;
    const int classid = node->classid;

    const uchar newidleft = nextid;
    const uchar newidright = nextid+1;

    cv::Mat mean = node->mean;
    cv::Mat cov = node->cov;
    cv::Mat eigenvalues, eigenvectors;
    cv::eigen(cov, eigenvalues, eigenvectors);

    cv::Mat eig = eigenvectors.row(0);
    cv::Mat comparison_value = eig * mean;

    node->left = new t_color_node();
    node->right = new t_color_node();

    node->left->classid = newidleft;
    node->right->classid = newidright;

    // We start out with the average color
    for(int y=0;y<height;y++) {
        cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y);
        uchar* ptrClass = classes.ptr<uchar>(y);
        for(int x=0;x<width;x++) {
            if(ptrClass[x] != classid)
                continue;

            cv::Vec3b color = ptr[x];
            cv::Mat scaled = cv::Mat(3, 1,
                                  CV_64FC1,
                                  cv::Scalar(0));

            scaled.at<double>(0) = color[0]/255.0f;
            scaled.at<double>(1) = color[1]/255.0f;
            scaled.at<double>(2) = color[2]/255.0f;

            cv::Mat this_value = eig * scaled;

            if(this_value.at<double>(0, 0) <= comparison_value.at<double>(0, 0)) {
                ptrClass[x] = newidleft;
            } else {
                ptrClass[x] = newidright;
            }
        }
    }
    return;
}

cv::Mat get_quantized_image(cv::Mat classes, t_color_node *root) {
    std::vector<t_color_node*> leaves = get_leaves(root);

    const int height = classes.rows;
    const int width = classes.cols;
    cv::Mat ret(height, width, CV_8UC3, cv::Scalar(0));

    for(int y=0;y<height;y++) {
        uchar *ptrClass = classes.ptr<uchar>(y);
        cv::Vec3b *ptr = ret.ptr<cv::Vec3b>(y);
        for(int x=0;x<width;x++) {
            uchar pixel_class = ptrClass[x];
            for(unsigned int i=0;i<leaves.size();i++) {
                if(leaves[i]->classid == pixel_class) {
                    ptr[x] = cv::Vec3b(leaves[i]->mean.at<double>(0)*255,
                                       leaves[i]->mean.at<double>(1)*255,
                                       leaves[i]->mean.at<double>(2)*255);
                }
            }
        }
    }

    return ret;
}

cv::Mat get_viewable_image(cv::Mat classes) {
    const int height = classes.rows;
    const int width = classes.cols;

    const int max_color_count = 12;
    cv::Vec3b *palette = new cv::Vec3b[max_color_count];
    palette[0]  = cv::Vec3b(  0,   0,   0);
    palette[1]  = cv::Vec3b(255,   0,   0);
    palette[2]  = cv::Vec3b(  0, 255,   0);
    palette[3]  = cv::Vec3b(  0,   0, 255);
    palette[4]  = cv::Vec3b(255, 255,   0);
    palette[5]  = cv::Vec3b(  0, 255, 255);
    palette[6]  = cv::Vec3b(255,   0, 255);
    palette[7]  = cv::Vec3b(128, 128, 128);
    palette[8]  = cv::Vec3b(128, 255, 128);
    palette[9]  = cv::Vec3b( 32,  32,  32);
    palette[10] = cv::Vec3b(255, 128, 128);
    palette[11] = cv::Vec3b(128, 128, 255);

    cv::Mat ret = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
    for(int y=0;y<height;y++) {
        cv::Vec3b *ptr = ret.ptr<cv::Vec3b>(y);
        uchar *ptrClass = classes.ptr<uchar>(y);
        for(int x=0;x<width;x++) {
            int color = ptrClass[x];
            if(color >= max_color_count) {
                printf("You should increase the number of predefined colors!\n");
                continue;
            }
            ptr[x] = palette[color];
        }
    }

    return ret;
}

t_color_node* get_max_eigenvalue_node(t_color_node *current) {
    double max_eigen = -1;
    cv::Mat eigenvalues, eigenvectors;

    std::queue<t_color_node*> queue;
    queue.push(current);

    t_color_node *ret = current;
    if(!current->left && !current->right)
        return current;

    while(queue.size() > 0) {
        t_color_node *node = queue.front();
        queue.pop();

        if(node->left && node->right) {
            queue.push(node->left);
            queue.push(node->right);
            continue;
        }

        cv::eigen(node->cov, eigenvalues, eigenvectors);
        double val = eigenvalues.at<double>(0);
        if(val > max_eigen) {
            max_eigen = val;
            ret = node;
        }
    }

    return ret;
}

std::vector<cv::Vec3b> find_dominant_colors(cv::Mat img, int count) {
    const int width = img.cols;
    const int height = img.rows;

    cv::Mat classes = cv::Mat(height, width, CV_8UC1, cv::Scalar(1));
    t_color_node *root = new t_color_node();

    root->classid = 1;
    root->left = NULL;
    root->right = NULL;

    t_color_node *next = root;
    get_class_mean_cov(img, classes, root);
    for(int i=0;i<count-1;i++) {
        next = get_max_eigenvalue_node(root);
        partition_class(img, classes, get_next_classid(root), next);
        get_class_mean_cov(img, classes, next->left);
        get_class_mean_cov(img, classes, next->right);
    }

    std::vector<cv::Vec3b> colors = get_dominant_colors(root);
/*
    cv::Mat quantized = get_quantized_image(classes, root);
    cv::Mat viewable = get_viewable_image(classes);
    cv::Mat dom = get_dominant_palette(colors);

    cv::imwrite("./classification.png", viewable);
    cv::imwrite("./quantized.png", quantized);
    cv::imwrite("./palette.png", dom);
*/
    return colors;
}

std::vector<double> find_dominant_hcolors(cv::Mat rgb_roi, int how_many)
{
	std::vector<double> hue_dcolors;
	std::vector<cv::Vec3b> bgr_dominants;
	bgr_dominants = find_dominant_colors(rgb_roi, how_many);

	std::vector<cv::Vec3b>::iterator dit; int i = 1;
	for( dit = bgr_dominants.begin() ; dit != bgr_dominants.end(); ++dit){

		cv::Vec3b color = *dit;
		float R, G, B, H, S, V;
		R = (float)color[2];
		G = (float)color[1];
		B = (float)color[0];
		RGB2HSV(R,G,B,H,S,V);
		
		// Convert from floats to 8-bit integers.
		int bH = (int)(0.5f + H * 255.0f);
		int bS = (int)(0.5f + S * 255.0f);
		int bV = (int)(0.5f + V * 255.0f);
		// Clip the values to make sure it fits within the 8bits.
		if (bH > 255)
			bH = 255;
		if (bH < 0)
			bH = 0;
		if (bS > 255)
			bS = 255;
		if (bS < 0)
			bS = 0;
		if (bV > 255)
			bV = 255;
		if (bV < 0)
			bV = 0;		
		
		//cout << "Dominant values for color " << i << " R = " << R << " G = " << G << " B = " << B << endl;
		//cout << " H = " << bH << " S = " << bS << " V = " << bV << endl;
		i++;
		hue_dcolors.push_back(bH);
	} 
	//imshow("blob", rgb_roi);waitKey(0);
	//cv::Mat dom = get_dominant_palette(bgr_dominants);
	//imshow("colors", dom);waitKey(0);	
	
	return hue_dcolors;
}

std::vector<double> find_dominant_hcolors2(cv::Mat rgb_roi, int how_many)
{
	std::vector<double> hue_dcolors;
	std::vector<cv::Vec3b> bgr_dominants;
	bgr_dominants = find_dominant_colors(rgb_roi, how_many);

	std::vector<cv::Vec3b>::iterator dit; int i = 1;
	for( dit = bgr_dominants.begin() ; dit != bgr_dominants.end(); ++dit){

		cv::Vec3b color = *dit;
	
		rgb inpx; hsv outpx;
		inpx.r = (double) color[2];
		inpx.g = (double) color[1];
		inpx.b = (double) color[0];
		
		outpx = rgb2hsv(inpx);
		
		//cout << "Dominant values for color " << i << " R = " << inpx.r << " G = " << inpx.g << " B = " << inpx.b << endl;
		//cout << " H = " << outpx.h << " S = " << outpx.s << " V = " << outpx.v << endl;
		i++;
		hue_dcolors.push_back(outpx.h);
	} 
	//imshow("blob", rgb_roi);waitKey(0);
	//cv::Mat dom = get_dominant_palette(bgr_dominants);
	//imshow("colors", dom);waitKey(0);	
	
	return hue_dcolors;
}

std::vector<double> find_dominant_hcolors3(cv::Mat rgb_roi, int how_many)
{
	std::vector<double> hue_dcolors;
	std::vector<cv::Vec3b> bgr_dominants;
	bgr_dominants = find_dominant_colors(rgb_roi, how_many+1);

	std::vector<cv::Vec3b>::iterator dit; int i = 0;
	for( dit = bgr_dominants.begin() ; dit != bgr_dominants.end(); ++dit){

		cv::Vec3b color = *dit;
	
		rgb inpx; hsv outpx;
		inpx.r = (double) color[2];
		inpx.g = (double) color[1];
		inpx.b = (double) color[0];
		
		outpx = rgb2hsv(inpx);
		
		//cout << "Dominant values for color " << i << " R = " << inpx.r << " G = " << inpx.g << " B = " << inpx.b << endl;
		//cout << " H = " << outpx.h << " S = " << outpx.s << " V = " << outpx.v << endl;
		i++;
		if(i!=1)
			hue_dcolors.push_back(outpx.h);
	} 
	//imshow("blob", rgb_roi);waitKey(0);
	//cv::Mat dom = get_dominant_palette(bgr_dominants);
	//imshow("colors", dom);waitKey(0);	
	
	return hue_dcolors;
}

void RGB2HSV(float r, float g, float b,
                    float &h, float &s, float &v)
{
	//float tmp1,tmp2;
    float K = 0.f;
    if (g < b)
    {
        std::swap(g, b);
        //tmp1 = g; tmp2 = b;
        //b = tmp1; g = tmp2;
        K = -1.f;
    }
    float min_gb = b;
    if (r < g)
    {
        std::swap(r, g);
        //tmp1 = r; tmp2 = g;
        //g = tmp1; r = tmp2;
        K = -2.f / 6.f - K;
        min_gb = std::min(g, b);
    }
    float chroma = r - min_gb;
    h = fabs(K + (g - b) / (6.f * chroma + 1e-20f));
    s = chroma / (r + 1e-20f);
    v = r;
    

}

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}
