#pragma once
#include <OpenMesh\Core\IO\MeshIO.hh>
#include <OpenMesh\Core\Mesh\PolyMesh_ArrayKernelT.hh>
#include <Eigen\Dense>
//=============================================================================
struct Traits
{
	/// The default coordinate type is OpenMesh::Vec3f.
	typedef OpenMesh::Vec3f  Point;

	/// The default normal type is OpenMesh::Vec3f.
	typedef OpenMesh::Vec3f  Normal;

	/// The default 1D texture coordinate type is float.
	typedef float  TexCoord1D;
	/// The default 2D texture coordinate type is OpenMesh::Vec2f.
	typedef OpenMesh::Vec2f  TexCoord2D;
	/// The default 3D texture coordinate type is OpenMesh::Vec3f.
	typedef OpenMesh::Vec3f  TexCoord3D;

	/// The default texture index type
	typedef int TextureIndex;

	/// The default color type is OpenMesh::Vec3uc.
	typedef OpenMesh::Vec3uc Color;

#ifndef DOXY_IGNORE_THIS
	VertexTraits{};
	HalfedgeTraits{};
	EdgeTraits{};
	FaceTraits{};
#endif

	VertexAttributes(OpenMesh::Attributes::Status);
	HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
	EdgeAttributes(OpenMesh::Attributes::Status);
	FaceAttributes(OpenMesh::Attributes::Status);
};
//=============================================================================
typedef OpenMesh::PolyMesh_ArrayKernelT<Traits> MyMesh;
//=============================================================================
int updateColors(MyMesh &_mesh, const unsigned char *_color);
//=============================================================================
int updateColors(MyMesh &_mesh, const Eigen::MatrixXi _colors);
//=============================================================================
int updateColorsOrder(MyMesh &_mesh);
//=============================================================================
int deleteVerticesByColor(MyMesh &_mesh, const unsigned char *color,
	std::vector<int> &_deleted_v_idxs);
//=============================================================================
int deleteVerticesByIdx(MyMesh &_mesh, const std::vector<int> &_v_idxs);
//=============================================================================
