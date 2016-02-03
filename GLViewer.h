#pragma once

#include <GL/glew.h>
#include <GL/glut.h>

#include "FileIO.h"
#include "Parameters.h"

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

	const static char* WINDOW_TITLE;

	//=========================================================================

	static parameters::Parameters params;

	//=========================================================================

	/**** LOADED FROM CONFIG FILE ****/

	/* IMAGE PARAMETERS */
	static int image_width;
	static int image_height;

	/* PERSPECTIVE PROPERTIES */
	static GLfloat frustum_left;
	static GLfloat frustum_right;
	static GLfloat frustum_bottom;
	static GLfloat frustum_top;
	static GLfloat frustum_near;
	static GLfloat frustum_far;
	static vector<float> eye;

	/* LIGHT PARAMETERS */
	static vector<float> light_ambient;
	static vector<float> light_position;
	static vector<float> light_intensity;
	static vector<float> background_colour;

	static bool use_gl_light;
	static bool use_sh_light;

	/* FILENAME */
	static string mesh_prefix;
	static string mesh_suffix;
	static unsigned int mesh_first_idx;
	static unsigned int num_frames;
	static string image_prefix;
	static string image_suffix;
	static unsigned int image_first_idx;

	//=========================================================================

	/**** VARIABLES ****/

	/* MAIN VARIABLES */
	static MyMesh mesh;
	static unsigned int frame_idx;

	static OpenMesh::IO::Options mesh_read_opt;

	static GLubyte* pixel_data;

	static int window_id;

	static int sh_order;
	static vector<float> sh_coeff;

	//=========================================================================

	/**** PRIVATE FUNCTIONS ****/

	/* INITIALIZATION METHODS */
	static void initGLUT(int *argc, char **argv);
	static void initParameters(const char* _filename);

	/* GLUT FUNCTIONS */
	static void display(void);

	/* DRAWING FUNCTIONS */
	static void drawModel();

	// Save rendered image
	static void saveRenderedImage();

	static string cvtIntToString(int _n, int _no_digits);
	static int numDigits(int _number);

	static float getSHIrradiance(const float* _normal);

public:
	static void initialize(int *argc, char **argv);
	static void run();
	static void destroy();
};
//=============================================================================
