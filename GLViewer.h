#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include "FileIO.h"
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
//=============================================================================
using namespace std;
//=============================================================================
class GLViewer
{
private:
	/**** CONSTANTS ****/

	/* WINDOW PARAMETERS */
	const static char* WINDOW_TITLE;
	const static int WINDOW_WIDTH;
	const static int WINDOW_HEIGHT;

	/* PERSPECTIVE PROPERTIES */
	const static GLfloat FRUSTUM_LEFT;
	const static GLfloat FRUSTUM_RIGHT;
	const static GLfloat FRUSTUM_BOTTOM;
	const static GLfloat FRUSTUM_TOP;
	const static GLfloat FRUSTUM_NEAR;
	const static GLfloat FRUSTUM_FAR;
	const static GLfloat EYE_DISTANCE; /* initial eye z-position  */
	const static GLfloat ZOOM_INCR;	/* zoom increment on */

	/* VIEWER COLOURS */
	const static GLfloat LIGHT_AMBIENT[4];
	const static GLfloat LIGHT_POSITION[4];
	const static GLfloat BACKGROUND_COLOUR[4];
	const static MyMesh::Color MODEL_COLOR;

	/* FILENAME */
	const static char* FILENAME_PREFIX;
	const static char* FILENAME_SUFFIX;
	const static unsigned int FIRST_IDX;
	const static unsigned int NUM_FRAMES;

	//=========================================================================

	/**** VARIABLES ****/

	/* MAIN VARIABLES */
	static MyMesh mesh;
	static unsigned int frame_idx;

	static OpenMesh::IO::Options mesh_read_opt;

	static GLubyte* pixel_data;

	/* VIEW VARIABLES */
	static GLfloat eye[3]; /* eye position*/
	static GLfloat aspectRatio; /* view aspect ratio*/

	//=========================================================================

	/**** PRIVATE FUNCTIONS ****/

	/* INITIALIZATION METHODS */
	static void initGLUT(int *argc, char **argv);

	/* GLUT AND GLUI FUNCTIONS */
	static void display(void);

	/* DRAWING FUNCTIONS */
	static void drawModel();

public:
	static void initialize(int *argc, char **argv);
	static void run();
	static void destroy();
};
//=============================================================================
