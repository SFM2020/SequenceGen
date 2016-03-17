#include "GLViewer.h"
//=============================================================================
int main(int argc, char **argv)
{
	GLViewer::initialize(&argc, argv);
	GLViewer::run();
	GLViewer::destroy();

	return 0;
}
//=============================================================================