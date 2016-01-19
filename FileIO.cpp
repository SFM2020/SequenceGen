#include "FileIO.h"
//=============================================================================
int readMesh(MyMesh& _mesh, const char* _filename,
	OpenMesh::IO::Options _ropt)
{
	if (!OpenMesh::IO::read_mesh(_mesh, _filename, _ropt))
	{
		std::cout << "Could not read file: " << _filename << std::endl << std::endl;

		return -1;
	}

	return 0;
}
//=============================================================================
int writeMesh(MyMesh& _mesh, const char* _filename,
	OpenMesh::IO::Options _wopt)
{
	if (!OpenMesh::IO::write_mesh(_mesh, _filename, _wopt))
	{
		std::cout << "Could not write file: " << _filename << std::endl << std::endl;

		return -1;
	}

	return 0;
}
//=============================================================================
int readMatrixXi(Eigen::MatrixXi &_matrix, const char* _filename)
{
	std::ifstream ifs(_filename);

	int rows;
	ifs >> rows;

	int cols;
	ifs >> cols;

	if (rows < 1 || cols < 1)
	{
		std::cout << "Invalid matrix size: (" << rows << ", " << cols << ")." 
			<< std::endl;

		return -1;
	}

	_matrix.resize(rows, cols);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
		{
			ifs >> _matrix(i, j);
		}
	}

	return 0;
}
//=============================================================================
//template <typename T>
int writeVector(const std::vector<int> &_v, const char* _filename)
{
	std::ofstream ofs;
	ofs.open(_filename, std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < _v.size(); i++)
	{
		ofs << _v[i] << std::endl;
	}
	ofs.close();

	return 0;
}
//=============================================================================