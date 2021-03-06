#include "GLViewer.h"
//=============================================================================
/* WINDOW PARAMETERS */
const char* GLViewer::WINDOW_TITLE = "Sequence Render";

parameters::Parameters GLViewer::params;

/* IMAGE SIZE */
int GLViewer::image_width;
int GLViewer::image_height;

/* PERSPECTIVE PROPERTIES */
GLfloat GLViewer::frustum_left;
GLfloat GLViewer::frustum_right;
GLfloat GLViewer::frustum_bottom;
GLfloat GLViewer::frustum_top;
GLfloat GLViewer::frustum_near;
GLfloat GLViewer::frustum_far;
vector<float> GLViewer::eye;

/* LIGHT PARAMETERS */
vector<vector<float>> GLViewer::light_ambient;
vector<vector<float>> GLViewer::light_position;
vector<vector<float>> GLViewer::light_intensity;
vector<vector<float>> GLViewer::light_specular_intensity;
vector<float> GLViewer::background_colour;

vector<float> GLViewer::light_increment;

bool GLViewer::use_gl_light;
bool GLViewer::use_sh_light;

/* FILENAME */
string GLViewer::mesh_prefix;
string GLViewer::mesh_suffix;
unsigned int GLViewer::mesh_first_idx;

unsigned int GLViewer::num_frames;

string GLViewer::image_prefix;
string GLViewer::image_suffix;
unsigned int GLViewer::image_first_idx;

/* MAIN VARIABLES */
MyMesh GLViewer::mesh;

unsigned int GLViewer::frame_idx;

OpenMesh::IO::Options GLViewer::mesh_read_opt;

GLubyte* GLViewer::pixel_data;

int GLViewer::window_id = 0;

int GLViewer::sh_order = -1;
vector<float> GLViewer::sh_coeff;

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

	use_gl_light = false;

	if (params.use_gl_light[0])
	{
		/* Enable a single OpenGL light. */
		glLightfv(GL_LIGHT0, GL_POSITION, light_position[0].data());
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity[0].data());
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular_intensity[0].data());
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient[0].data());
		glEnable(GL_LIGHT0);

		use_gl_light = true;
	}

	if (params.use_gl_light[1])
	{
		/* Enable a single OpenGL light. */
		glLightfv(GL_LIGHT1, GL_POSITION, light_position[1].data());
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_intensity[1].data());
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular_intensity[1].data());
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient[1].data());
		glEnable(GL_LIGHT1);

		use_gl_light = true;
	}

	if (use_gl_light)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		// Disable opengl lighting and illuminate using SH
		glDisable(GL_LIGHTING);
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, params.material_specular_intensity.data());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, params.material_specular_shininess);

	glEnable(GL_COLOR_MATERIAL);

	glClearColor(params.background_colour[0], params.background_colour[1],
		params.background_colour[2], params.background_colour[3]);

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
	}

	if (frame_idx == num_frames)
	{
		glutDestroyWindow(window_id);
	}
	else
	{
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

		modifyLight(GL_LIGHT0, 0);
		modifyLight(GL_LIGHT1, 1);

		glPushMatrix();
		drawModel();
		glPopMatrix();

		glutSwapBuffers();

		glutPostRedisplay();
	}
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
void GLViewer::modifyLight(const GLenum _light, const size_t _light_idx)
{
	if (params.use_gl_light[_light_idx])
	{
		light_intensity[_light_idx][0] += light_increment[_light_idx];

		if (light_intensity[_light_idx][0] > 1 
			|| light_intensity[_light_idx][0] < 0)
		{
			light_increment[_light_idx] *= -1;
			light_intensity[_light_idx][0] += 2 * light_increment[_light_idx];
		}

		light_intensity[_light_idx][1] += light_increment[_light_idx];
		light_intensity[_light_idx][2] += light_increment[_light_idx];

		glLightfv(_light, GL_DIFFUSE, light_intensity[_light_idx].data());
		glLightfv(_light, GL_SPECULAR, light_intensity[_light_idx].data());

		if (params.light_rotation[_light_idx] > 0 
			|| params.light_rotation[_light_idx] < 0)
		{
			light_position[_light_idx][0] = 
				cos(M_PI - frame_idx * params.light_rotation[_light_idx]);
			light_position[_light_idx][1] = 
				sin(frame_idx * params.light_rotation[_light_idx]);
			light_position[_light_idx][2] = -1;

			glLightfv(_light, GL_POSITION, light_position[_light_idx].data());
		}
	}
}
//=============================================================================
void GLViewer::saveRenderedImage()
{
	glReadPixels(0, 0, image_width, image_height, GL_BGR, GL_UNSIGNED_BYTE,
		pixel_data);

	cv::Mat img = cv::Mat(image_height, image_width, CV_8UC3, (void*)pixel_data);
	cv::flip(img, img, 0);

	string idx = cvtIntToString(image_first_idx + frame_idx - 1, 4);

	string path = image_prefix + idx + image_suffix;
	cv::imwrite(path.c_str(), img);
}
//=============================================================================
void GLViewer::initialize(int *argc, char **argv)
{
	initParameters(argv[1]);

	initGLUT(argc, argv);

	mesh.request_vertex_colors();
	mesh.request_face_normals();
	mesh.request_vertex_normals();

	mesh_read_opt += OpenMesh::IO::Options::VertexColor;
	mesh_read_opt += OpenMesh::IO::Options::VertexNormal;
	//mesh_read_opt += OpenMesh::IO::Options::Binary;
	//mesh_read_opt += OpenMesh::IO::Options::LSB;
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
	vector<float> sh_function;

	if (sh_order > -1)
	{
		sh_function.push_back(1.f);
	}

	float x, y, z;
	if (sh_order > 0)
	{
		x = _normal[0];
		y =	_normal[1];
		z =	_normal[2];

		sh_function.push_back(x);
		sh_function.push_back(y);
		sh_function.push_back(z);
	}

	float x2, y2, z2;
	float xy, xz, yz;
	if (sh_order > 1)
	{
		x2 = _normal[0] * _normal[0];
		y2 = _normal[1] * _normal[1];
		z2 = _normal[2] * _normal[2];

		xy = _normal[0] * _normal[1];
		xz = _normal[0] * _normal[2];
		yz = _normal[1] * _normal[2];

		sh_function.push_back(xy);
		sh_function.push_back(xz);
		sh_function.push_back(yz);
		sh_function.push_back( x2 - y2 );
		sh_function.push_back( 3.f * z2 - 1 );
	}

	if (sh_order > 2)
	{
		sh_function.push_back((3 * x2 - y2) * y);	// (3 * x^2 - y^2) * y 
		sh_function.push_back(xy * z);				// x * y * z
		sh_function.push_back((5 * z2 - 1) * y);	// (5 * z^2 - 1) * y
		sh_function.push_back((5 * z2 - 3) * z);	// (5 * z^2 - 3) * z
		sh_function.push_back((5 * z2 - 1) * x);	// (5 * z^2 - 1) * x
		sh_function.push_back((x2 - y2) * z);		// (x^2 - y^2) * z
		sh_function.push_back((x2 - 3 * y2) * x);	// (x^2 - 3 * y^2) * x
	}

	if (sh_order > 3)
	{
		sh_function.push_back((x2 - y2) * xy);							// (x^2 - y^2) * x * y
		sh_function.push_back((3 * x2 - y2) * yz);						// (3 * x^2 - y^2) * yz
		sh_function.push_back((7 * z2 - 1) * xy);						// (7 * z^2 - 1) * x * y
		sh_function.push_back((7 * z2 - 3) * yz);						// (7 * z^2 - 3) * y * z
		sh_function.push_back(3 - 30 * z2 + 35 * z2 * z2);				// 3 - 30 * z^2 + 35 * z^4
		sh_function.push_back((7 * z2 - 3) * xz);						// (7 * z^2 - 3) * x * z
		sh_function.push_back((7 * z2 - 1) * (x2 - y2));				// (7 * z^2 - 1) * (x^2 - y^2)
		sh_function.push_back((x2 - 3 * y2) * xz);						// (x^2 - 3 * y^2) * x * z
		sh_function.push_back((x2 - 3 * y2) * x2 - (3 * x2 - y2) * y2);	// (x^2 - 3 * y^2) * x^2 - (3 * x^2 - y^2) * y^2 
	}

	float intensity = 0.f;
	for (int i = 0; i < sh_coeff.size(); i++)
	{
		intensity += sh_coeff[i] * sh_function[i];
	}

	return intensity;
}
//=============================================================================
void GLViewer::initParameters(const char* _filename)
{
	// Load parameters from yml file
	params.load(_filename);

	/* IMAGE SIZE */
	image_width = params.image_width;
	image_height = params.image_height;

	/* PERSPECTIVE PROPERTIES */
	frustum_left = params.frustum_left;
	frustum_right = params.frustum_right;
	frustum_bottom = params.frustum_bottom;
	frustum_top = params.frustum_top;
	frustum_near = params.frustum_near;
	frustum_far = params.frustum_far;
	eye = params.eye;

	/* LIGHT PARAMETERS */
	light_ambient = params.light_ambient;
	light_position = params.light_position;
	light_intensity = params.light_intensity;
	light_specular_intensity = params.light_specular_intensity;
	background_colour = params.background_colour;

	light_increment = params.light_increment;

	use_gl_light = false;
	use_sh_light = params.use_sh_light;

	/* FILENAME */
	mesh_prefix = params.mesh_prefix;
	mesh_suffix = params.mesh_suffix;
	mesh_first_idx = params.mesh_first_idx;

	num_frames = params.num_frames;

	image_prefix = params.image_prefix;
	image_suffix = params.image_suffix;
	image_first_idx = params.image_first_idx;

	if (!use_gl_light && use_sh_light)
	{
		readFloatVector(params.sh_coeff_filename.c_str(), sh_coeff);
		sh_order = sqrt(sh_coeff.size()) - 1;
	}

	pixel_data = (GLubyte*)malloc(3 * image_width * image_height);
}
//=============================================================================