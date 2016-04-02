#include <iostream>
#include <vector>

#include "lib/cvmlcpp/base/Matrix"
#include "lib/cvmlcpp/volume/Geometry"
#include "lib/cvmlcpp/volume/VolumeIO"
#include "lib/cvmlcpp/volume/Voxelizer"

using namespace std;

// Generic functions for using cvmlcpp library
cvmlcpp::Matrix<int, 3u> getVoxelizedMatrixFromFile(char *stlFile){
	cvmlcpp::Matrix<int, 3u> voxels;
	cvmlcpp::Geometry<float> geometry;

	cvmlcpp::readFileSTL(geometry, stlFile);

	if(TURN_ON_SCALE){
		geometry.scaleTo(SCALE_BY);
	}
	
	if(TURN_ON_RESOLUTION)
		cvmlcpp::voxelize(geometry, voxels, RESOLUTION);
	else
		cvmlcpp::voxelize(geometry, voxels);

	return voxels;
}

// This function is used with integrated voxelizer
vector<vector<vector<int>>> convert_to_voxels(char *stlFile){
	
	cvmlcpp::Matrix<int, 3u> voxels = getVoxelizedMatrixFromFile(stlFile);
	
	tr1::array<std::size_t, 3>::const_iterator i = voxels.extents();
	int xMax = (*i);
	i++;
	int yMax = (*i);
	i++;
	int zMax = (*i);
	int l=0;

	int lMax = yMax;
	int bMax = zMax;
	int hMax = xMax;

	vector<vector<vector<int>>> vox(lMax, vector<vector<int> >(bMax, vector<int>(hMax, false)));
	
	cvmlcpp::Matrix<int,3u>::iterator iter = voxels.begin();

	int total = (xMax)*(yMax)*(zMax);
	for(int hIter = 0; hIter<hMax; hIter++){
		for(int lIter = 0; lIter<lMax; lIter++){
			for(int bIter = 0; bIter<bMax; bIter++){
				if((*iter) == 1)
					//This fix was made to resolve mirror image error
					vox[lIter][bIter][hMax - hIter-1] = true;
				else
					vox[lIter][bIter][hMax - hIter-1] = false;
				iter++; l++;
			}	
		}
	}	
	
	return vox;
	
}
