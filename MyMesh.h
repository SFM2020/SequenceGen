#pragma once
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
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
	typedef OpenMesh::Vec2uc  TexCoord2D;
	/// The default 3D texture coordinate type is OpenMesh::Vec3f.
	typedef OpenMesh::Vec3uc  TexCoord3D;

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
