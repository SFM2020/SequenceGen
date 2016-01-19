#include "GLViewer.h"
//=============================================================================
/**** CONSTANTS****/

/* WINDOW PARAMETERS */
const char* GLViewer::WINDOW_TITLE = "GLViewer";
const int GLViewer::WINDOW_WIDTH = 1920;
const int GLViewer::WINDOW_HEIGHT = 1088;

/* PERSPECTIVE PROPERTIES */
const GLfloat GLViewer::FRUSTUM_LEFT = -960;
const GLfloat GLViewer::FRUSTUM_RIGHT = 960;
const GLfloat GLViewer::FRUSTUM_BOTTOM = -544;
const GLfloat GLViewer::FRUSTUM_TOP = 544;
const GLfloat GLViewer::FRUSTUM_NEAR = 10;
const GLfloat GLViewer::FRUSTUM_FAR = 3000;

const GLfloat GLViewer::EYE_DISTANCE = 1.0f;
const GLfloat GLViewer::ZOOM_INCR = 5.f;

/* VIEWER COLOURS */
const GLfloat GLViewer::LIGHT_AMBIENT[4] = { 0.5f, 0.5f, 0.5f, 1.f };
const GLfloat GLViewer::LIGHT_POSITION[4] = { .5f, .5f, 1.f, 0.f };		// { 0.4f, 0.01f, 0.15f, 0.f };
const GLfloat GLViewer::BACKGROUND_COLOUR[4] = { 0.46f, 0.46f, 0.54f, 1.f };
//const GLfloat GLViewer::BACKGROUND_COLOUR[4] = { 0.7f, 0.7f, 0.7f, 1.f };

/* FILENAME */
const char* GLViewer::FILENAME_PREFIX 
= "C:/Users/Qi/Documents/GitHub/_data/valgaerts_siggraphasia2012/meshes/refined_reconstruction_0";
const char* GLViewer::FILENAME_SUFFIX = ".ply";
const unsigned int GLViewer::FIRST_IDX = 180;
const unsigned int GLViewer::NUM_FRAMES = 200;

//=============================================================================
/**** VARIABLES ****/

/* MAIN VARIABLES */
MyMesh GLViewer::mesh;

unsigned int GLViewer::frame_idx;

OpenMesh::IO::Options GLViewer::mesh_read_opt;

GLubyte* GLViewer::pixel_data = (GLubyte*)malloc(3 * WINDOW_WIDTH * WINDOW_HEIGHT);

/* VIEW VARIABLES */
GLfloat GLViewer::eye[3] = { 0.f, 0.f, 0.f };
GLfloat GLViewer::aspectRatio = 1.f;

//=============================================================================
void GLViewer::initGLUT(int *argc, char **argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WINDOW_WIDTH) / 2,
		10);

	// Uncomment to enable transparencies
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);

	glutDisplayFunc(display);

	glewInit();

	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glClearColor(BACKGROUND_COLOUR[0], BACKGROUND_COLOUR[1],
		BACKGROUND_COLOUR[2], BACKGROUND_COLOUR[3]);

	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Anti-aliasing*/
	glEnable(GL_MULTISAMPLE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}
//=============================================================================
void GLViewer::display(void)
{
	if (frame_idx > 0 && frame_idx <= NUM_FRAMES)
	{
		glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE,
			pixel_data);

		cv::Mat img = cv::Mat(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3, (void*) pixel_data);

		int i = 0;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(FRUSTUM_LEFT, FRUSTUM_RIGHT, FRUSTUM_BOTTOM, FRUSTUM_TOP,
		FRUSTUM_NEAR, FRUSTUM_FAR);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2],	/* eye */
		0.f, 0.f, 1.f,	/* center */
		0.f, 1.f, 0.f);	/* up is in positive Y direction */

	glPushMatrix();
	drawModel();
	glPopMatrix();

	glutSwapBuffers();
}
//=============================================================================
void GLViewer::drawModel()
{
	if (frame_idx < NUM_FRAMES)
	{
		string path = FILENAME_PREFIX + to_string(FIRST_IDX + frame_idx) + FILENAME_SUFFIX;

		readMesh(mesh, path.c_str(), mesh_read_opt);

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

			//MyMesh::Normal n = mesh.normal(*fv_it);
			//float normal[3] {n[0], n[1], n[2]};

			MyMesh::Color c = mesh.color(*fv_it);
			glColor3ui(c[0], c[1], c[2]);

			//RowVectorXf n = normals2draw->row(v_idx);
			//float normal[3] {n(0), n(1), n(2)};

			//RowVectorXf c = colours2draw->row(v_idx);
			//glColor3f(c(0), c(1), c(2));

			//glNormal3fv(normal);
			glVertex3fv(point);
		}
	}
	glEnd();
}
//=============================================================================
void GLViewer::initialize(int *argc, char **argv)
{
	initGLUT(argc, argv);

	mesh.request_vertex_colors();
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
