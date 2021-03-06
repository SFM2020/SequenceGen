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
#define USE_SH_LIGHT		"use_sh_light"
#define USE_GL_LIGHT		"use_gl_light"
#define LIGHT_AMBIENT0				"light_ambient0"
#define LIGHT_POSITION0				"light_position0"
#define LIGHT_INTENSITY0			"light_intensity0"
#define LIGHT_SPECULAR_INTENSITY0	"light_specular_intensity0"
#define LIGHT_AMBIENT1				"light_ambient1"
#define LIGHT_POSITION1				"light_position1"
#define LIGHT_INTENSITY1			"light_intensity1"
#define LIGHT_SPECULAR_INTENSITY1	"light_specular_intensity1"
#define MATERIAL_SPECULAR_INTENSITY	"material_specular_intensity"
#define MATERIAL_SPECULAR_SHININESS	"material_specular_shininess"
#define LIGHT_ROTATION		"light_rotation"
#define LIGHT_INCREMENT		"light_increment"
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
			use_sh_light = false;

			use_gl_light = { true, true };

			light_ambient = { 
				{ 0.1f, 0.1f, 0.1f, 1.f },
				{ 0.1f, 0.1f, 0.1f, 1.f }
			};
			light_position = {
				{ -1.f, 0.f, 1.f, 0.f },
				{ -1.f, 0.f, 1.f, 0.f }
			};
			light_intensity = {
				{ 1.f, 1.f, 1.f, 1.f },
				{ 1.f, 1.f, 1.f, 1.f }
			};
			light_specular_intensity = {
				{ 1.f, 1.f, 1.f, 1.f },
				{ 1.f, 1.f, 1.f, 1.f }
			};

			light_rotation = { 0, 0 };
			light_increment = { 0, 0 };

			material_specular_intensity = { 1.f, 1.f, 1.f, 1.f };
			material_specular_shininess = 10.f;
			light_rotation = { (float)(M_PI / 90), (float)(M_PI / 90) };
			background_colour = { 0.46f, 0.46f, 0.54f, 1.f };

			// I/O parameters
			//mesh_prefix = "C:/Users/Qi/Desktop/generated/meshes/mesh_";	
			mesh_prefix = "C:/Users/Qi/Documents/GitHub/_PangaeaResults/low_res_gray/mesh_pyramid/mesh";
			//mesh_prefix = "C:/Users/Qi/Desktop/generated/templates/cropped/template_12500_";
			// "C:/Users/Qi/Desktop/generated/meshes/_sphere_";
			//mesh_suffix = ".ply";
			mesh_suffix = "_level00.obj";
			mesh_first_idx = 1;// 180;
			num_frames = 200;
			image_prefix = "C:/Users/Qi/Desktop/generated/images/result_";
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
		bool use_sh_light;

		vector<int> use_gl_light;
		vector<vector<float>> light_ambient;
		vector<vector<float>> light_position;
		vector<vector<float>> light_intensity;
		vector<vector<float>> light_specular_intensity;
		vector<float> light_rotation;
		vector<float> light_increment;
		vector<float> material_specular_intensity;
		float material_specular_shininess;
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

			if (!fs[USE_SH_LIGHT].empty())
			{
				fs[USE_SH_LIGHT] >> use_sh_light;
			}

			if (!fs[LIGHT_AMBIENT0].empty())
			{
				fs[LIGHT_AMBIENT0] >> light_ambient[0];
			}

			if (!fs[LIGHT_POSITION0].empty())
			{
				fs[LIGHT_POSITION0] >> light_position[0];
			}

			if (!fs[LIGHT_INTENSITY0].empty())
			{
				fs[LIGHT_INTENSITY0] >> light_intensity[0];
			}

			if (!fs[LIGHT_SPECULAR_INTENSITY0].empty())
			{
				fs[LIGHT_SPECULAR_INTENSITY0] >> light_specular_intensity[0];
			}

			if (!fs[LIGHT_AMBIENT1].empty())
			{
				fs[LIGHT_AMBIENT1] >> light_ambient[1];
			}

			if (!fs[LIGHT_POSITION1].empty())
			{
				fs[LIGHT_POSITION1] >> light_position[1];
			}

			if (!fs[LIGHT_INTENSITY1].empty())
			{
				fs[LIGHT_INTENSITY1] >> light_intensity[1];
			}

			if (!fs[LIGHT_SPECULAR_INTENSITY1].empty())
			{
				fs[LIGHT_SPECULAR_INTENSITY1] >> light_specular_intensity[1];
			}

			if (!fs[MATERIAL_SPECULAR_INTENSITY].empty())
			{
				fs[MATERIAL_SPECULAR_INTENSITY] >> material_specular_intensity;
			}

			if (!fs[MATERIAL_SPECULAR_SHININESS].empty())
			{
				fs[MATERIAL_SPECULAR_SHININESS] >> material_specular_shininess;
			}

			if (!fs[LIGHT_ROTATION].empty())
			{
				fs[LIGHT_ROTATION] >> light_rotation;
			}

			if (!fs[LIGHT_INCREMENT].empty())
			{
				fs[LIGHT_INCREMENT] >> light_increment;
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
