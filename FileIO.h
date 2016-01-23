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
int writeVector(const std::vector<int> &_v, const char* _filename);
//=============================================================================
int readVector(const char* _filename, float* _v, int &_size);
//=============================================================================
int readFloatVector(const char* _filename, std::vector<float> &_v);
//=============================================================================