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
MyMesh GLViewer::template_mesh;
MyMesh GLViewer::mesh;
MyMesh GLViewer::mesh2;

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

	if (use_gl_light)
	{
		/* Enable a single OpenGL light. */
		glLightfv(GL_LIGHT0, GL_POSITION, params.light_position.data());
		glLightfv(GL_LIGHT0, GL_DIFFUSE, params.light_intensity.data());
		glLightfv(GL_LIGHT0, GL_SPECULAR, params.light_specular_intensity.data());
		glLightfv(GL_LIGHT0, GL_AMBIENT, params.light_ambient.data());
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, params.material_specular_intensity.data());
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, params.material_specular_shininess);
	}
	else
	{
		// Disable opengl lighting and illuminate using SH
		glDisable(GL_LIGHTING);
	}

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
		int curr_idx = mesh_first_idx + frame_idx;

		string idx = cvtIntToString(curr_idx , 
			params.num_digits > 0 ? params.num_digits : numDigits(curr_idx));

		string path = mesh_prefix + idx + mesh_suffix;

		readMesh(mesh, path.c_str(), mesh_read_opt);

		string path2 = params.mesh_prefix2 + idx + params.mesh_suffix2;

		readMesh(mesh2, path2.c_str(), mesh_read_opt);

		//for (MyMesh::VertexIter v_it = template_mesh.vertices_begin(); 
		//	v_it != template_mesh.vertices_end(); ++v_it)
		//{
		//	int v_idx = v_it->idx();

		//	MyMesh::VertexHandle vh = mesh2.vertex_handle(v_idx);

		//	template_mesh.set_point(*v_it, mesh2.point(vh));

		//}

		//template_mesh.update_normals();

		frame_idx++;
	}

	drawModel(mesh, params.point_cloud, params.color, params.radius);
	drawModel(mesh2, params.point_cloud2, params.color2, params.radius2);
}
//=============================================================================
void GLViewer::drawModel(MyMesh &mesh, bool point_cloud, 
	const vector<float> &color, float radius)
{
	if (point_cloud)
	{
		for (MyMesh::ConstVertexIter v_it = mesh.vertices_begin();
			v_it != mesh.vertices_end(); ++v_it)
		{
			MyMesh::Point p = mesh.point(*v_it);

			glPushMatrix();
			glTranslatef(p[0], p[1], p[2]);
			glutSolidSphere(radius, 10, 10);
			glColor3f(color[0], color[1], color[2]);
			glPopMatrix();
		}
	}
	else
	{
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
}
//=============================================================================
void GLViewer::saveRenderedImage()
{
	glReadPixels(0, 0, image_width, image_height, GL_BGR, GL_UNSIGNED_BYTE,
		pixel_data);

	cv::Mat img = cv::Mat(image_height, image_width, CV_8UC3, (void*)pixel_data);
	cv::flip(img, img, 0);

	int curr_idx = image_first_idx + frame_idx - 1;

	string idx = cvtIntToString(curr_idx, 
		params.num_digits > 0 ? params.num_digits : numDigits(curr_idx));

	string path = image_prefix + idx + image_suffix;
	cv::imwrite(path.c_str(), img);
}
//=============================================================================
void GLViewer::initialize(int *argc, char **argv)
{
	initParameters(argv[1]);

	initGLUT(argc, argv);

	mesh_read_opt += OpenMesh::IO::Options::VertexColor;
	mesh_read_opt += OpenMesh::IO::Options::FaceColor;
	mesh_read_opt += OpenMesh::IO::Options::VertexNormal;

	template_mesh.request_vertex_colors();
	template_mesh.request_face_colors();
	template_mesh.request_face_normals();
	template_mesh.request_vertex_normals();

	readMesh(template_mesh, params.template_path.c_str(), mesh_read_opt);

	mesh.request_vertex_colors();
	mesh.request_face_colors();
	mesh.request_face_normals();
	mesh.request_vertex_normals();

	mesh2.request_vertex_colors();
	mesh2.request_face_colors();
	mesh2.request_face_normals();
	mesh2.request_vertex_normals();


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
	if (_number==0)
	{
		return 1;
	}

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

	use_gl_light = params.use_gl_light;
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