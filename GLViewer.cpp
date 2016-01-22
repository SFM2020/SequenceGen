#include "GLViewer.h"
//=============================================================================
/**** CONSTANTS****/

/* WINDOW PARAMETERS */
const char* GLViewer::WINDOW_TITLE = "Sequence Render";

parameters::Parameters GLViewer::params;

int GLViewer::image_width = GLViewer::params.image_width;
int GLViewer::image_height = GLViewer::params.image_height;

/* PERSPECTIVE PROPERTIES */
GLfloat GLViewer::frustum_left = GLViewer::params.frustum_left;
GLfloat GLViewer::frustum_right = GLViewer::params.frustum_right;
GLfloat GLViewer::frustum_bottom = GLViewer::params.frustum_bottom;
GLfloat GLViewer::frustum_top = GLViewer::params.frustum_top;
GLfloat GLViewer::frustum_near = GLViewer::params.frustum_near;
GLfloat GLViewer::frustum_far = GLViewer::params.frustum_far;
vector<float> GLViewer::eye = GLViewer::params.eye;

/* LIGHT PARAMETERS */
vector<float> GLViewer::light_ambient = GLViewer::params.light_ambient;
vector<float> GLViewer::light_position = GLViewer::params.light_position;
vector<float> GLViewer::light_intensity = GLViewer::params.light_intensity;
vector<float> GLViewer::background_colour = GLViewer::params.background_colour;

bool GLViewer::use_gl_light = GLViewer::params.use_gl_light;
bool GLViewer::use_sh_light = GLViewer::params.use_sh_light;

/* FILENAME */
string GLViewer::mesh_prefix = GLViewer::params.mesh_prefix;
string GLViewer::mesh_suffix = GLViewer::params.mesh_suffix;
unsigned int GLViewer::mesh_first_idx = GLViewer::params.mesh_first_idx;

unsigned int GLViewer::num_frames = GLViewer::params.num_frames;

string GLViewer::image_prefix = GLViewer::params.image_prefix;
string GLViewer::image_suffix = GLViewer::params.image_suffix;
unsigned int GLViewer::image_first_idx = GLViewer::params.image_first_idx;

string GLViewer::sh_coeff_filename = GLViewer::params.sh_coeff_filename;

//=============================================================================
/**** VARIABLES ****/

/* MAIN VARIABLES */
MyMesh GLViewer::mesh;

unsigned int GLViewer::frame_idx;

OpenMesh::IO::Options GLViewer::mesh_read_opt;

GLubyte* GLViewer::pixel_data = (GLubyte*)malloc(3 * image_width * image_height);

int GLViewer::window_id = 0;

int GLViewer::sh_order = -1;
int GLViewer::n_sh_coeff = 0;
float* GLViewer::sh_coeff;


//=============================================================================
void GLViewer::initGLUT(int *argc, char **argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(image_width, image_height);
	window_id = glutCreateWindow(WINDOW_TITLE);

	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - image_width) / 2, 10);

	// Uncomment to enable transparencies
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);

	glutDisplayFunc(display);

	glewInit();

	if (use_gl_light)
	{
		/* Enable a single OpenGL light. */
		glLightfv(GL_LIGHT0, GL_POSITION, light_position.data());
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity.data());
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient.data());
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
	}
	else
	{
		// Disable opengl lighting and illuminate using SH
		glDisable(GL_LIGHTING);
	}

	glEnable(GL_COLOR_MATERIAL);

	glClearColor(background_colour[0], background_colour[1],
		background_colour[2], background_colour[3]);

	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Anti-aliasing*/
	glEnable(GL_MULTISAMPLE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
//=============================================================================
void GLViewer::display(void)
{
	if (frame_idx > 0)
	{
		saveRenderedImage();

		if (frame_idx == num_frames)
		{
			glutDestroyWindow(window_id);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(frustum_left, frustum_right, frustum_bottom, frustum_top,
		frustum_near, frustum_far);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2],	/* eye */
		eye[0], eye[1], 1.f,	/* center */
		0.f, -1.f, 0.f);	/* up is in positive y direction */

	glPushMatrix();
	drawModel();
	glPopMatrix();

	glutSwapBuffers();

	glutPostRedisplay();
}
//=============================================================================
void GLViewer::drawModel()
{
	if (frame_idx < num_frames)
	{
		string idx = cvtIntToString(mesh_first_idx + frame_idx, 4);

		string path = mesh_prefix + idx + mesh_suffix;

		readMesh(mesh, path.c_str(), mesh_read_opt);

		mesh.update_face_normals();
		mesh.update_vertex_normals();

		frame_idx++;
	}

	glBegin(GL_TRIANGLES);

	for (MyMesh::FaceIter f_it = mesh.faces_begin();
		f_it != mesh.faces_end(); ++f_it)
	{
		MyMesh::FaceVertexIter fv_it;
		for (fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			//int v_idx = fv_it.handle().idx();

			MyMesh::Point p = mesh.point(*fv_it);
			float point[3] {p[0], p[1], p[2]};

			MyMesh::Normal n = mesh.normal(*fv_it);
			float normal[3] {n[0], n[1], n[2]};

			MyMesh::Color c = mesh.color(*fv_it);

			float red = c[0];
			float green = c[1];
			float blue = c[2];

			//float red = ((float)c[0]) / 255.f;
			//float green = ((float)c[1]) / 255.f;	
			//float blue = ((float)c[2]) / 255.f;

			//float red = 0.796078f;
			//float green = 0.521569f;
			//float blue = 0.435294f;			  

			if (!use_gl_light && use_sh_light)
			{
				float irradiance = getSHIrradiance(normal);

				red *= irradiance;
				green *= irradiance;
				blue *= irradiance;
			}

			glColor3f(red, green, blue);

			glNormal3fv(normal);
			glVertex3fv(point);
		}
	}
	glEnd();
}
//=============================================================================
void GLViewer::saveRenderedImage()
{
	glReadPixels(0, 0, image_width, image_height, GL_BGR, GL_UNSIGNED_BYTE,
		pixel_data);

	cv::Mat img = cv::Mat(image_height, image_width, CV_8UC3, (void*)pixel_data);

	string idx = cvtIntToString(image_first_idx + frame_idx - 1, 4);

	string path = image_prefix + idx + image_suffix;
	cv::imwrite(path.c_str(), img);
}
//=============================================================================
void GLViewer::initialize(int *argc, char **argv)
{
	initGLUT(argc, argv);

	mesh.request_vertex_colors();
	mesh.request_face_normals();
	mesh.request_vertex_normals();

	mesh_read_opt += OpenMesh::IO::Options::VertexColor;
	mesh_read_opt += OpenMesh::IO::Options::VertexNormal;
	//mesh_read_opt += OpenMesh::IO::Options::Binary;
	//mesh_read_opt += OpenMesh::IO::Options::LSB;

	//params.load(argv[1]);

	if (!use_gl_light && use_sh_light)
	{
		readVector(sh_coeff_filename.c_str(), sh_coeff, n_sh_coeff);
		sh_order = sqrt(n_sh_coeff) - 1;
	}
}
//=============================================================================
void GLViewer::run()
{
	glutMainLoop();
}
//=============================================================================
void GLViewer::destroy()
{
	delete pixel_data;

	if (!use_gl_light && use_sh_light)
	{
		delete[] sh_coeff;
	}
}
//=============================================================================
string GLViewer::cvtIntToString(int _n, int _no_digits)
{
	int no_zeros = _no_digits - numDigits(_n);

	return string(no_zeros, '0').append(to_string(_n));
}
//=============================================================================
int GLViewer::numDigits(int _number)
{
	int digits = 0;
	while (_number) {
		_number /= 10;
		digits++;
	}

	return digits;
}
//=============================================================================
float GLViewer::getSHIrradiance(const float* _normal)
{
	float* sh_function = new float[n_sh_coeff];

	if (sh_order > -1)
	{
		sh_function[0] = 1;

	}

	if (sh_order > 0)
	{
		sh_function[1] = _normal[0];
		sh_function[2] = _normal[1];
		sh_function[3] = _normal[2];
	}

	if (sh_order > 1)
	{
		sh_function[4] = _normal[0] * _normal[1];
		sh_function[5] = _normal[0] * _normal[2];
		sh_function[6] = _normal[1] * _normal[2];
		sh_function[7] = _normal[0] * _normal[0] - _normal[1] * _normal[1];
		sh_function[8] = 3.f * _normal[2] * _normal[2] - 1;
	}

	float intensity = 0.f;
	for (int i = 0; i < n_sh_coeff; i++)
	{
		intensity += sh_coeff[i] * sh_function[i];
	}

	delete[] sh_function;

	return intensity;
}
//=============================================================================
