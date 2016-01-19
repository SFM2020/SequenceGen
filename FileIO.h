#pragma once

#include "MyMesh.h"
//=============================================================================
int readMesh(MyMesh& _mesh, const char* _filename,
	OpenMesh::IO::Options _ropt);
//=============================================================================
int writeMesh(MyMesh& _mesh, const char* _filename,
	OpenMesh::IO::Options _wopt);
//=============================================================================
int readMatrixXi(Eigen::MatrixXi &_matrix, const char* _filename);
//=============================================================================
//template <typename T>
int writeVector(const std::vector<int> &_v, const char* _filename);
//=============================================================================
