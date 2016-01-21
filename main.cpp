#include "GLViewer.h"
//=============================================================================
int main(int argc, char **argv)
{
	GLViewer glViewer;
	glViewer.initialize(&argc, argv);
	glViewer.run();
	glViewer.destroy();

	return 0;
}
//=============================================================================