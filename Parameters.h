#ifndef PARAMETERS_H_
#define PARAMETERS_H_

// Image parameters
#define IMAGE_WIDTH			"image_width"
#define IMAGE_HEIGHT		"image_height"

// View parameters
#define FRUSTUM_LEFT		"frustum_left"
#define FRUSTUM_RIGHT		"frustum_right"
#define FRUSTUM_BOTTOM		"frustum_bottom"
#define FRUSTUM_TOP			"frustum_top"
#define FRUSTUM_NEAR		"frustum_near"
#define FRUSTUM_FAR			"frustum_far"
#define EYE					"eye"

// Light parameters
#define USE_GL_LIGHT		"use_gl_light"
#define LIGHT_AMBIENT		"light_ambient"
#define LIGHT_POSITION		"light_position"
#define LIGHT_INTENSITY		"light_intensity"
#define BACKGROUND_COLOUR	"background_colour"
#define SH_COEFF_FILENAME	"sh_coeff_filename"

// I/O parameters
#define MESH_PREFIX		"mesh_prefix"
#define MESH_SUFFIX		"mesh_suffix"
#define MESH_FIRST_IDX	"mesh_first_idx"

#define NUM_FRAMES		"num_frames"

#define IMAGE_PREFIX	"image_prefix"
#define IMAGE_SUFFIX	"image_suffix"
#define IMAGE_FIRST_IDX	"image_first_idx"

#include <cmath>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"

using namespace std;

namespace parameters {
	struct Parameters {

		// Default constructor that sets up a generic sparse problem.
		Parameters() {
			// Image parameters
			image_width = 1600;//1280;//1920;
			image_height = 900;//800;// 1088;

			// View parameters
			frustum_left = -80.f;//-64.f;//-96.f;
			frustum_right = 80.f;// 64.f;//96.f;
			frustum_bottom = -45.f;// -40.f;// -54.4f;
			frustum_top = 45.f; //40.f;//54.4f;
			frustum_near = 1000.f;
			frustum_far = 3000.f;
			eye = { 0.f, 0.f, 0.f };//{ 27.5228f, 3.62954f, 0.f};

			// Light parameters
			use_gl_light = true;
			light_ambient = { 0.1f, 0.1f, 0.1f, 1.f };
			light_position = { -1.f, 0.f, 1.f, 0.f };
			light_intensity = { 1.f, 1.f, 1.f, 1.f };
			background_colour = { 0.46f, 0.46f, 0.54f, 1.f };

			// I/O parameters
			//mesh_prefix = "C:/Users/Qi/Desktop/generated/meshes/mesh_";	
			mesh_prefix = "C:/Users/Qi/Documents/GitHub/_PangaeaResults/first_attempt/mesh_pyramid/mesh";
			// "C:/Users/Qi/Desktop/generated/meshes/_sphere_";
			//mesh_suffix = ".ply";
			mesh_suffix = "_level00.obj";
			mesh_first_idx = 1;// 180;
			num_frames = 200;
			image_prefix = "C:/Users/Qi/Desktop/generated/images/result";
			image_suffix = ".png";
			image_first_idx = 1;
			sh_coeff_filename = "C:/Users/Qi/Desktop/generated/images/x_-1_z_1_a_0.1/_sh_coeff.txt";
		}

		// Image parameters
		int image_width;
		int image_height;

		// View parameters
		float frustum_left;
		float frustum_right;
		float frustum_bottom;
		float frustum_top;
		float frustum_near;
		float frustum_far;
		vector<float> eye;

		// Light parameters
		bool use_gl_light;
		vector<float> light_ambient;
		vector<float> light_position;
		vector<float> light_intensity;
		vector<float> background_colour;

		// I/O parameters
		string mesh_prefix;
		string mesh_suffix;
		int mesh_first_idx;
		int num_frames;
		string image_prefix;
		string image_suffix;
		int image_first_idx;
		string sh_coeff_filename;


		// Load parameters from an ini file
		inline void Parameters::load(const std::string &_filename)
		{
			cv::FileStorage fs(_filename, cv::FileStorage::READ);

			// Image parameters
			if (!fs[IMAGE_WIDTH].empty())
			{
				fs[IMAGE_WIDTH] >> image_width;
			}

			if (!fs[IMAGE_HEIGHT].empty())
			{
				fs[IMAGE_HEIGHT] >> image_height;
			}


			// View parameters
			if (!fs[FRUSTUM_LEFT].empty())
			{
				fs[FRUSTUM_LEFT] >> frustum_left;
			}

			if (!fs[FRUSTUM_RIGHT].empty())
			{
				fs[FRUSTUM_RIGHT] >> frustum_right;
			}

			if (!fs[FRUSTUM_BOTTOM].empty())
			{
				fs[FRUSTUM_BOTTOM] >> frustum_bottom;
			}

			if (!fs[FRUSTUM_TOP].empty())
			{
				fs[FRUSTUM_TOP] >> frustum_top;
			}

			if (!fs[FRUSTUM_NEAR].empty())
			{
				fs[FRUSTUM_NEAR] >> frustum_near;
			}

			if (!fs[FRUSTUM_FAR].empty())
			{
				fs[FRUSTUM_FAR] >> frustum_far;
			}

			if (!fs[EYE].empty())
			{
				fs[EYE] >> eye;
			}


			// Light parameters
			if (!fs[USE_GL_LIGHT].empty())
			{
				fs[USE_GL_LIGHT] >> use_gl_light;
			}

			if (!fs[LIGHT_AMBIENT].empty())
			{
				fs[LIGHT_AMBIENT] >> light_ambient;
			}

			if (!fs[LIGHT_POSITION].empty())
			{
				fs[LIGHT_POSITION] >> light_position;
			}

			if (!fs[LIGHT_INTENSITY].empty())
			{
				fs[LIGHT_INTENSITY] >> light_intensity;
			}

			if (!fs[BACKGROUND_COLOUR].empty())
			{
				fs[BACKGROUND_COLOUR] >> background_colour;
			}

			if (!fs[SH_COEFF_FILENAME].empty())
			{
				fs[SH_COEFF_FILENAME] >> sh_coeff_filename;
			}


			// I/O parameters
			if (!fs[MESH_PREFIX].empty())
			{
				fs[MESH_PREFIX] >> mesh_prefix;
			}

			if (!fs[MESH_SUFFIX].empty())
			{
				fs[MESH_SUFFIX] >> mesh_suffix;
			}

			if (!fs[MESH_FIRST_IDX].empty())
			{
				fs[MESH_FIRST_IDX] >> mesh_first_idx;
			}


			if (!fs[NUM_FRAMES].empty())
			{
				fs[NUM_FRAMES] >> num_frames;
			}


			if (!fs[IMAGE_PREFIX].empty())
			{
				fs[IMAGE_PREFIX] >> image_prefix;
			}

			if (!fs[IMAGE_SUFFIX].empty())
			{
				fs[IMAGE_SUFFIX] >> image_suffix;
			}

			if (!fs[IMAGE_FIRST_IDX].empty())
			{
				fs[IMAGE_FIRST_IDX] >> image_first_idx;
			}
		}

		inline void Parameters::save(const std::string &_filename)
		{

		}
	};
}

#endif  // PARAMETERS_H_
