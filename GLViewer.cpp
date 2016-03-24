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
vector<float> GLViewer::eye_center;

/* LIGHT PARAMETERS */
vector<float> GLViewer::light_ambient;
vector<float> GLViewer::light_position;
vector<float> GLViewer::light_intensity;
vector<float> GLViewer::background_colour;

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

unsigned int GLViewer::frame_idx = 0;
unsigned int GLViewer::angle_idx = 0;

OpenMesh::IO::Options GLViewer::mesh_read_opt;

GLubyte* GLViewer::pixel_data;

int GLViewer::window_id = 0;

int GLViewer::sh_order = -1;
vector<float> GLViewer::sh_coeff;

Matrix3f GLViewer::KK;
Vector3f GLViewer::translation;
MatrixXf GLViewer::vertices;
MatrixXf GLViewer::centred_vertices;
MatrixXf GLViewer::points2d;
MatrixXf GLViewer::face_normals;
vector<vector<int>> GLViewer::faces;

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
		createDepthMap(vertices, points2d);
		angle_idx++;
	}

	if (frame_idx == num_frames 
		&& (angle_idx == params.camera_rotation_x.size()) )
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

		////glPushMatrix();
		//if (!params.camera_rotation_x.empty())
		//{
		//	rotateEye(eye, eye_center, params.camera_rotation_x[angle_idx],
		//		params.camera_rotation_y[angle_idx],
		//		params.camera_rotation_z[angle_idx]);

		//	//glTranslatef(-eye_center[0], -eye_center[1], -eye_center[2]);
		//	//glRotatef(0, 1, 0, 0);
		//	//glTranslatef(eye_center[0], eye_center[1], eye_center[2]);

		//}

		gluLookAt(eye[0], eye[1], eye[2],	/* eye */
			eye_center[0], eye_center[1], eye_center[2],	/* center */
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
	if (angle_idx == 0 || angle_idx == params.camera_rotation_x.size())
	{
		string idx = cvtIntToString(mesh_first_idx + frame_idx, 4);

		string path = mesh_prefix + idx + mesh_suffix;

		readMesh(mesh, path.c_str(), mesh_read_opt);

		mesh.update_normals();

		copyMeshFaces(mesh, faces);
		copyMeshFaceNormals(mesh, face_normals);
		copyMeshVertices(mesh, centred_vertices);
		centred_vertices -= translation.replicate(1, centred_vertices.cols());

		angle_idx = 0;
		frame_idx++;
	}

	if (!params.camera_rotation_x.empty())
	{
		rotateMesh(params.camera_rotation_x[angle_idx], mesh);
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
	cv::flip(img, img, 0);

	string idx = cvtIntToString(image_first_idx + angle_idx, 4);

	string path = image_prefix + idx;

	path += image_suffix;

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

	initIntrinsicMatrix();

	translation = Vector3f(eye_center[0], eye_center[1], eye_center[2]);
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
	eye_center = params.eye_center;

	/* LIGHT PARAMETERS */
	light_ambient = params.light_ambient;
	light_position = params.light_position;
	light_intensity = params.light_intensity;
	background_colour = params.background_colour;

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
		sh_order = (int)sqrt(sh_coeff.size()) - 1;
	}

	pixel_data = (GLubyte*)malloc(3 * image_width * image_height);
}
//=============================================================================
void GLViewer::rotateEye(vector<float> &_eye, const vector<float> &_center, 
	const float _angle_x, const float _angle_y, const float _angle_z)
{
	float angle_x = _angle_x * float(M_PI) / 180.0f;
	float angle_y = _angle_y * float(M_PI) / 180.0f;
	float angle_z = _angle_z * float(M_PI) / 180.0f;

	Eigen::Matrix3f rot_x;
	rot_x << 1, 0, 0,
		0, cos(angle_x), -sin(angle_x),
		0, sin(angle_x), cos(angle_x);

	Eigen::Matrix3f rot_y;
	rot_y << cos(angle_y), 0, sin(_angle_y),
		0, 1, 0,
		-sin(angle_y), 0, cos(_angle_y);

	Eigen::Matrix3f rot_z;
	rot_z << cos(_angle_z), -sin(angle_z), 0,
		sin(angle_z), cos(angle_z), 0,
		0, 0, 1;

	Eigen::Vector3f v;
	v << -_center[0], -_center[1], -_center[2];

	Eigen::Vector3f rot_v = rot_z * rot_y * rot_x * v;

	_eye[0] = rot_v(0) + _center[0];
	_eye[1] = rot_v(1) + _center[1];
	_eye[2] = rot_v(2) + _center[2];
}
//=============================================================================
void GLViewer::initIntrinsicMatrix()
{
	KK = Matrix3f::Identity();
	KK(0, 0) = params.frustum_near * 10;
	KK(1, 1) = params.frustum_near * 10;
	KK(0, 2) = params.frustum_right * 10;
	KK(1, 2) = params.frustum_top * 10;
}
//=============================================================================
void GLViewer::copyMeshVertices(const MyMesh &_mesh, MatrixXf &_vertices)
{
	_vertices.resize(3, _mesh.n_vertices());

	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();
	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		int v_idx = v_it->idx();
		MyMesh::Point p = _mesh.point(*v_it);
		_vertices(0, v_idx) = p[0];
		_vertices(1, v_idx) = p[1];
		_vertices(2, v_idx) = p[2];
	}
}
//=============================================================================
void GLViewer::copyMeshFaces(const MyMesh &_mesh, vector<vector<int>> &_faces)
{
	vector<int> zeros(3, 0);
	_faces.resize(_mesh.n_faces(), zeros);

	MyMesh::ConstFaceIter f_it;
	MyMesh::ConstFaceIter f_end = _mesh.faces_end();
	for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
	{
		int faceInd = f_it->idx();

		MyMesh::ConstFaceVertexIter fv_it;
		MyMesh::ConstFaceVertexIter fv_end = mesh.fv_end(*f_it);
		int i = 0;
		for (fv_it = mesh.fv_begin(*f_it); fv_it != fv_end; ++fv_it)
		{
			_faces[faceInd][i] = fv_it->idx();
			i++;
		}
	}
}
//=============================================================================
void GLViewer::copyMeshFaceNormals(MyMesh &_mesh, MatrixXf &_face_normals)
{
	_face_normals.resize(3, _mesh.n_faces());

	MyMesh::ConstFaceIter f_it;
	MyMesh::ConstFaceIter f_end = _mesh.faces_end();
	for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
	{
		int faceInd = f_it->idx();

		MyMesh::Normal n = _mesh.normal(*f_it);

		_face_normals(0, faceInd) = n[0];
		_face_normals(1, faceInd) = n[1];
		_face_normals(2, faceInd) = n[2];
	}
}
//=============================================================================
void GLViewer::computeImageProjection(const MatrixXf &_vertices, 
	const Matrix3f &_KK, MatrixXf &_image_projection)
{
	_image_projection.resize(2, _vertices.cols());

	MatrixXf homogeneus = _KK * _vertices;
	
	_image_projection.row(0) = homogeneus.row(0).cwiseProduct(
		homogeneus.row(2).cwiseInverse()
		);
	_image_projection.row(1) = homogeneus.row(1).cwiseProduct(
		homogeneus.row(2).cwiseInverse()
		);
}
//=============================================================================
void GLViewer::updateVisibility(vector<bool> &visibility)
{
	int width = params.image_width;
	int height = params.image_height;

	int n_vertices = (int)vertices.cols();

	static vector< vector<unsigned int> > visibilityFacesTest;

	// visibility mask, used for speed up old rendering code
	// this will set up in the first call
	// will not do anything during the second time
	visibilityFacesTest.resize(width*height);
	for (int i = 0; i < width*height; ++i)
		visibilityFacesTest[i].reserve(5);   // 5 reserved faces for each pixel.

	// image width and height
	int m_nWidth, m_nHeight;
	m_nWidth = width; m_nHeight = height;

	// reset the visibility mask to true first.
	visibility.resize(n_vertices, true);

	// update the visibility mask using previous mesh
	// assign faces to visibilityFacesTest according to 2d projections
	float x1, x2, x3, y1, y2, y3;
	float xmin, xmax, ymin, ymax;
	int xminI, xmaxI, yminI, ymaxI;
	int xx, yy;
	int vertex1, vertex2, vertex3;
	MatrixXf faceCenters(3, mesh.n_faces());

	for (int faceInd = 0; faceInd < faces.size(); ++faceInd)
	{
		vertex1 = faces[faceInd][0];
		vertex2 = faces[faceInd][1];
		vertex3 = faces[faceInd][2];

		x1 = points2d(0, vertex1);
		x2 = points2d(0, vertex2);
		x3 = points2d(0, vertex3);
		y1 = points2d(1, vertex1);
		y2 = points2d(1, vertex2);
		y3 = points2d(1, vertex3);
		xmin = min(min(x1, x2), x3);
		xmax = max(max(x1, x2), x3);
		ymin = min(min(y1, y2), y3);
		ymax = max(max(y1, y2), y3);
		// if the projection of the face is outside the image, we give up
		if (xmin > m_nWidth + 1 || ymin > m_nHeight + 1 ||
			xmax < 0 || ymax < 0)
			continue;

		xminI = max(1, int(floor(xmin)));
		yminI = max(1, int(floor(ymin)));
		xmaxI = min(int(ceil(xmax)), m_nWidth);
		ymaxI = min(int(ceil(ymax)), m_nHeight);
		for (xx = xminI - 1; xx < xmaxI; ++xx)
		{
			for (yy = yminI - 1; yy < ymaxI; ++yy)
			{
				visibilityFacesTest[yy*m_nWidth + xx].push_back(faceInd);
			}
		}

		Vector3f v1 = vertices.col(vertex1);
		Vector3f v2 = vertices.col(vertex2);
		Vector3f v3 = vertices.col(vertex3);

		// prepare the face center and face normals for the purpose of gettin
		// intersection between faces and back-projected ray.
		faceCenters.col(faceInd) = (v1 + v2 + v3) / 3;

		//compnorm(&meshVertices[vertex1][0], &meshVertices[vertex2][0],
		//	&meshVertices[vertex3][0], &faceNormals[3 * faceInd], 1);

	}

	// // check if the projection of all faces looks reasonable
	// // PixelType* pMaskBuffer = new PixelType[m_nHeight*m_nWidth];
	// // IntensityImageType maskImage(m_nHeight,m_nWidth,pMaskBuffer);
	// IntensityImageType maskImage(m_nHeight,m_nWidth);
	// for(int i = 0; i < m_nHeight; ++i)
	// {
	//     for(int j = 0; j < m_nWidth; ++j)
	//     {
	//         if(visibilityFacesTest[i*m_nWidth+j].size() > 0)
	//         maskImage(i,j) = 255;
	//         else
	//         maskImage(i,j) = 0;
	//     }
	// }
	// // cv::imshow("maskImage",maskImage);
	// // cv::waitKey(0);
	// cv::imwrite("visMaskImage.png",maskImage);
	// // delete[] pMaskBuffer;

	// loop over vertices to find the intersection between backprojected race
	// and the possible faces of the mesh
	float projX, projY;
	int projXminI, projYminI;
	int projXmaxI, projYmaxI;
	int position, faceInd;

	for (int vertexInd = 0; vertexInd < n_vertices;
		++vertexInd)
	{
		// if the projection is outside the image, it is not visible

		if (points2d(0, vertexInd) < 0 || points2d(0, vertexInd) > m_nWidth ||
			points2d(1, vertexInd) < 0 || points2d(1, vertexInd) > m_nHeight)
		{
			visibility[vertexInd] = false;
			continue;
		}

		projX = points2d(0, vertexInd);
		projY = points2d(1, vertexInd);

		projXminI = max(1, int(floor(projX)));
		projYminI = max(1, int(floor(projY)));

		projXmaxI = min(int(ceil(projX)), m_nWidth);
		projYmaxI = min(int(ceil(projY)), m_nHeight);

		bool done = false;

		for (xx = projXminI - 1; xx < projXmaxI; ++xx)
		{
			for (yy = projYminI - 1; yy < projYmaxI; ++yy)
			{
				// loop over all the faces here
				position = yy*m_nWidth + xx;
				int facesNum = (int)visibilityFacesTest[position].size();
				for (int k = 0; k < facesNum; ++k)
				{
					faceInd = visibilityFacesTest[position][k];

					vertex1 = faces[faceInd][0];
					vertex2 = faces[faceInd][1];
					vertex3 = faces[faceInd][2];
					// check if the vertex belongs to this face
					if (vertex1 == vertexInd ||
						vertex2 == vertexInd ||
						vertex3 == vertexInd)
						continue;
					// First test if the projection of the point is inside the triangle,
					// If not, we continue to the next face
					if (pointInTriangleTest2(points2d.col(vertexInd),
						points2d.col(vertex1),
						points2d.col(vertex2),
						points2d.col(vertex3)))
					{
						Vector3f v0 = vertices.col(vertexInd);
						Vector3f v1 = vertices.col(vertex1);
						Vector3f v2 = vertices.col(vertex2);
						Vector3f v3 = vertices.col(vertex3);
						Vector3f fn = face_normals.col(faceInd);

						// compute the intersection between the
						// backtraced ray of vertexInd and the face faceInd
						// point, center, normal, point1, point2, point3
						visibility[vertexInd] = visibilityTest(v0,
							faceCenters.col(faceInd),
							fn,
							v1,
							v2,
							v3);
					}
					else
						continue;

					// check if the vertex is already occluded or not
					if (visibility[vertexInd] == false)
					{
						done = true;
						break;
					}
				}

				if (done)
					break;

			}

			if (done)
				break;

		}

		//cout << "Vertex " << vertexInd << ": " << visibility[vertexInd] << endl;

	}

	for (int i = 0; i < visibilityFacesTest.size(); ++i)
		visibilityFacesTest[i].clear();   // 5 reserved faces for each pixel, capacity doesn't change

	// print out how many points are visible and how many points are occluded
	int visible_num = 0;
	int occluded_num = 0;
	for (int i = 0; i < visibility.size(); ++i)
		if (visibility[i])
			visible_num++;
		else
			occluded_num++;

	cout << "visible num: " << visible_num << endl;
	cout << "occluded num: " << occluded_num << endl;

}
//=============================================================================
bool GLViewer::pointInTriangleTest2(const Vector2f &pointP,
	const Vector2f &pointA, const Vector2f &pointB, const Vector2f &pointC)
{
	float A, B, D, E, C, F;
	A = pointA(0) - pointC(0);
	B = pointB(0) - pointC(0);
	D = pointA(1) - pointC(1);
	E = pointB(1) - pointC(1);

	C = pointC(0) - pointP(0);
	F = pointC(1) - pointP(1);

	float alpha, beta, gamma;
	alpha = (B*(F)-C*(E)) / (A*(E)-B*(D));
	beta = (A*(F)-C*(D)) / (B*(D)-A*(E));
	gamma = 1 - alpha - beta;

	return (alpha >= 0 && beta >= 0 && gamma >= 0);

}
//=============================================================================
bool GLViewer::visibilityTest(const Vector3f &vertex, const Vector3f &center,
	const Vector3f & normal, const Vector3f & vertex1,
	const Vector3f & vertex2, const Vector3f & vertex3)
{
	// tell if a point is in front of a triangle face
	float faceDist, pointDist, scale;
	faceDist = center.dot(normal);
	pointDist = vertex.dot(normal);
	scale = faceDist / pointDist;

	Vector3f intersection = scale * vertex;

	// if the intersection is in front,
	// then the test vertex is occluded by
	// the front face, we give up
	if (intersection(2) < vertex(2))
		return false;
	else
		return true;

}
//=============================================================================
void GLViewer::rotateMesh(const float &_angle, MyMesh &_mesh)
{
	float angle = _angle * float(M_PI) / 180.0f;

	Matrix3f rot_matrix;
	rot_matrix << 1, 0, 0,
		0, cos(angle), -sin(angle),
		0, sin(angle), cos(angle);

	vertices = rot_matrix * centred_vertices;
	vertices += translation.replicate(1, vertices.cols());

	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();
	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		int v_idx = v_it->idx();
		MyMesh::Point p(
			vertices(0, v_idx), vertices(1, v_idx), vertices(2, v_idx));
		_mesh.set_point(*v_it, p);
	}

	_mesh.update_normals();

	copyMeshVertices(mesh, vertices);
	copyMeshFaceNormals(mesh, face_normals);
	computeImageProjection(vertices, KK, points2d);
}
//=============================================================================
void GLViewer::createDepthMap(const MatrixXf &_vertices, const MatrixXf &_points2d)
{
	int img_height = params.image_height;
	int img_width = params.image_width;

	vector<bool> visibility;
	updateVisibility(visibility);

	// For each pixels stores pairs of weights and depth of vertices
	// The weight is based ob its distance to the pixel
	static vector<vector<pair<float, float>>> depth_pixel;
	depth_pixel.resize(img_width * img_height);

	float sqrt_2 = sqrt(2.0f);
	for (int i = 0; i < _vertices.cols(); i++)
	{
		if (visibility[i])
		{
			float x = _points2d(0, i);
			float y = _points2d(1, i);
			int x_min = (int)floor(x);
			int x_max = (int)ceil(x);
			int y_min = (int)floor(y);
			int y_max = (int)ceil(y);

			float depth = _vertices(2, i) * 1e-3f;
			// The reprojection of a vertex can lie between four different pixels
			float distance;
			int pixel_idx;
			distance = sqrt(pow(x - float(x_min), 2) + pow(y - float(y_min), 2));
			pixel_idx = x_min * img_height + y_min;
			depth_pixel[pixel_idx].push_back(pair<float, float>(sqrt_2 - distance, depth));

			if (x_min != x_max)
			{
				distance = sqrt(pow(x - float(x_max), 2) + pow(y - float(y_min), 2));
				pixel_idx = x_max * img_height + y_min;
				depth_pixel[pixel_idx].push_back(pair<float, float>(sqrt_2 - distance, depth));
			}

			if (y_min != y_max)
			{
				distance = sqrt(pow(x - float(x_min), 2) + pow(y - float(y_max), 2));
				pixel_idx = x_min * img_height + y_max;
				depth_pixel[pixel_idx].push_back(pair<float, float>(sqrt_2 - distance, depth));
			}

			if (x_min != x_max && y_min != y_max)
			{
				distance = sqrt(pow(x - float(x_max), 2) + pow(y - float(y_max), 2));
				pixel_idx = x_max * img_height + y_max;
				depth_pixel[pixel_idx].push_back(pair<float, float>(sqrt_2 - distance, depth));
			}
		}
	}


	cv::Mat depth_img = cv::Mat(img_height, img_width, CV_32FC1);
	for (int i = 0; i < img_height * img_width; i++)
	{
		int y = i % img_height;
		int x = i / img_height;

		float depth = 0.0f;
		if (!depth_pixel[i].empty())
		{
			float sum_weights = 0;
			for (int j = 0; j < depth_pixel[i].size(); j++)
			{
				depth += depth_pixel[i][j].first * depth_pixel[i][j].second;
				sum_weights += depth_pixel[i][j].first;
			}
			depth /= sum_weights;
		}

		depth_img.at<float>(cv::Point(x, y)) = depth;

		depth_pixel[i].clear();
	}

	string idx = cvtIntToString(image_first_idx + angle_idx, 4);

	string path = params.depth_prefix + idx + params.depth_suffix;

	cv::imwrite(path.c_str(), depth_img);
}
//=============================================================================