//#include "FileIO.h"
#include "GLViewer.h"
//=============================================================================
int main(int argc, char **argv)
{

	////const char* mesh_filename = "C:/Users/Qi/Desktop/output_simple.ply";

	//const char* mesh_filename
	//	= "C:/Users/Qi/Documents/GitHub/_data/valgaerts_siggraphasia2012/meshes/refined_reconstruction_0180.ply";

	//const char* mesh_filename_preffix
	//	= "C:/Users/Qi/Documents/GitHub/_data/valgaerts_siggraphasia2012/meshes/refined_reconstruction_0"; 
	//const char* mesh_filename_suffix = ".ply";

	////const char* mesh_filename
	////	= "C:/Users/Qi/Desktop/output_order.ply";

	//bool _read_vertex_normals = false;
	//bool _read_vertex_colors = true;
	//bool _binary = true;
	//bool _little_endian = true;

	//OpenMesh::IO::Options ropt;
	//MyMesh mesh;

	//if (_read_vertex_normals)
	//{
	//	mesh.request_vertex_normals();
	//	ropt += OpenMesh::IO::Options::VertexNormal;
	//}

	//if (_read_vertex_colors)
	//{
	//	mesh.request_vertex_colors();
	//	ropt += OpenMesh::IO::Options::VertexColor;
	//}

	//if (_binary)
	//{
	//	ropt += OpenMesh::IO::Options::Binary;
	//
	//	if (_little_endian)
	//	{
	//		ropt += OpenMesh::IO::Options::LSB;
	//	}
	//}

	//readMesh(mesh, mesh_filename, ropt);

	////const char* albedo_filename = "C:/Users/Qi/Desktop/albedo_occlusion_simple_4.txt";

	////Eigen::MatrixXi albedo;
	////readMatrixXi(albedo, albedo_filename);

	////updateColors(mesh, albedo);

	//unsigned char color[3];

	//color[0] = 0;
	//color[1] = 0;
	//color[2] = 0;

	//std::vector<int> deleted_v_idxs;
	//deleteVerticesByColor(mesh, color, deleted_v_idxs);

	////const char* deleted_filename = "C:/Users/Qi/Desktop/deleted_v_idxs.txt";
	////writeVector(deleted_v_idxs, deleted_filename);

	//updateColorsOrder(mesh);

	//OpenMesh::IO::Options wopt;
	//wopt += OpenMesh::IO::Options::VertexColor;

	//const char* output_mesh_filename = "C:/Users/Qi/Desktop/output_order_simple.ply";

	//writeMesh(mesh, output_mesh_filename, wopt);

	GLViewer glViewer;
	glViewer.initialize(&argc, argv);
	glViewer.run();
	glViewer.destroy();

	return 0;
}
//=============================================================================