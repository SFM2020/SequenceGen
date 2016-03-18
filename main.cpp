#include "GLViewer.h"
//=============================================================================
int main(int argc, char **argv)
{
	GLViewer::initialize(&argc, argv);
	GLViewer::run();
	GLViewer::destroy();

	//cv::Mat depth_img = cv::imread("C:/Users/Qi/Documents/GitHub/data/levi/images/new_rigid/rgb0001_00000.exr", CV_LOAD_IMAGE_ANYDEPTH);


	return 0;
}
//=============================================================================