#include "MyMesh.h"
//=============================================================================
int updateColors(MyMesh &_mesh, const unsigned char *_color)
{
	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();

	MyMesh::Color color{ _color[0], _color[1], _color[2] };
	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		_mesh.set_color(v_it, color);
	}

	return 0;
}
//=============================================================================
int updateColors(MyMesh &_mesh, const Eigen::MatrixXi _colors)
{
	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();
	int i = 0;
	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		MyMesh::Color color{ (unsigned char)_colors(i, 0),
			(unsigned char)_colors(i, 1), (unsigned char)_colors(i, 2) };
		_mesh.set_color(v_it, color);
		i++;
	}

	return 0;
}
//=============================================================================
int updateColorsOrder(MyMesh &_mesh)
{
	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();
	int i = 0;
	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		MyMesh::Color color{ (unsigned char)((i / (256 * 256)) % 256),
			(unsigned char) ((i / 256) % 256), (unsigned char) (i % 256) };
		_mesh.set_color(v_it, color);
		i++;
	}

	return 0;
}
//=============================================================================
int deleteVerticesByColor(MyMesh &_mesh, const unsigned char *_color, 
	std::vector<int> &_deleted_v_idxs)
{
	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();

	int i = 0;
	_deleted_v_idxs.clear();
	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		MyMesh::Color v_color = _mesh.color(v_it);
		
		//MyMesh::Color new_color{ (unsigned char)((i / (256 * 256)) % 256),
		//	(unsigned char)((i / 256) % 256), (unsigned char)(i % 256) };
		//_mesh.set_color(v_it, new_color);
		
		if (v_color[0] == _color[0] 
			&& v_color[1] == _color[1]
			&& v_color[2] == _color[2])
		{
			_mesh.delete_vertex(v_it, true);

			_deleted_v_idxs.push_back(v_it.handle().idx());
		}
		i++;
	}

	_mesh.garbage_collection();

	return 0;
}
//=============================================================================
int deleteVerticesByIdx(MyMesh &_mesh, const std::vector<int> &_v_idxs)
{
	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();

	int i = 0;
	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		MyMesh::Color v_color = _mesh.color(v_it);

		int v_idx = v_it.handle().idx();

		if (i < _v_idxs.size() && v_idx == _v_idxs[i])
		{
			_mesh.delete_vertex(v_it, true);

			i++;
		}
	}

	_mesh.garbage_collection();

	return 0;
}
//=============================================================================
int deleteVerticesByNormal(MyMesh &_mesh)
{
	_mesh.request_face_normals();
	_mesh.request_vertex_normals();
	_mesh.update_face_normals();
	_mesh.update_vertex_normals();

	MyMesh::ConstVertexIter v_it;
	MyMesh::ConstVertexIter v_end = _mesh.vertices_end();

	for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it)
	{
		MyMesh::Normal normal = _mesh.normal(v_it);

		// Remove vectices normal direction pointing away from the camera
		if (normal[2] > 0.f)
		{
			_mesh.delete_vertex(v_it, true);
		}
	}

	_mesh.garbage_collection();

	return 0;
}
//=============================================================================
int updateVertexNormals(MyMesh &_mesh)
{
	_mesh.request_face_normals();
	_mesh.request_vertex_normals();
	_mesh.update_face_normals();
	_mesh.update_vertex_normals();

	return 0;
}
//=============================================================================
