#pragma once

#include <GL/glew.h>
#include <GL/glut.h>

#include "FileIO.h"
#include "Parameters.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//=============================================================================
using namespace std;
using namespace Eigen;
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
	static vector<float> eye_center;

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
	static unsigned int angle_idx;

	static OpenMesh::IO::Options mesh_read_opt;

	static GLubyte* pixel_data;

	static int window_id;

	static int sh_order;
	static vector<float> sh_coeff;

	// Intrinsic matrix
	static Matrix3f KK;
	static Vector3f translation;
	static MatrixXf centred_vertices;
	static MatrixXf vertices;
	static MatrixXf points2d;
	static MatrixXf face_normals;
	static vector<vector<int>> faces;

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

	static void rotateEye(vector<float> &_eye, const vector<float> &_center,
		const float _angle_x, const float _angle_y, const float _angle_z);

	static void initIntrinsicMatrix();
	static void copyMeshVertices(const MyMesh &_mesh, MatrixXf &_vertices);
	static void copyMeshFaces(const MyMesh &_mesh, 
		vector<vector<int>> &_faces);
	static void copyMeshFaceNormals(MyMesh &_mesh, MatrixXf &_face_normals);
	static void computeImageProjection(const MatrixXf &_vertices, const Matrix3f &_KK,
		MatrixXf &_image_projection);

	static void rotateMesh(const float &_angle, MyMesh &_mesh);

	static void updateVisibility(vector<bool> &visibility);
	static bool pointInTriangleTest2(const Vector2f &pointP,
		const Vector2f &pointA, const Vector2f &pointB, const Vector2f &pointC);
	static bool visibilityTest(const Vector3f &vertex, const Vector3f &center,
		const Vector3f & normal, const Vector3f & vertex1,
		const Vector3f & vertex2, const Vector3f & vertex3);

	static void createDepthMap(const MatrixXf &_vertices, const MatrixXf &_points2d);

public:
	static void initialize(int *argc, char **argv);
	static void run();
	static void destroy();
};
//=============================================================================
