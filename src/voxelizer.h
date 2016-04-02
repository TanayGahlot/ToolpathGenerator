#include <iostream>
#include <vector>

#include "lib/cvmlcpp/base/Matrix"
#include "lib/cvmlcpp/volume/Geometry"
#include "lib/cvmlcpp/volume/VolumeIO"
#include "lib/cvmlcpp/volume/Voxelizer"

#define TURN_ON_SCALE 0
#define SCALE_BY 100
#define TURN_ON_RESOLUTION 0
#define RESOLUTION 0.1
using namespace std;

// Generic functions for using cvmlcpp library
cvmlcpp::Matrix<int, 3u> getVoxelizedMatrixFromFile(char *stlFile){
	cvmlcpp::Matrix<int, 3u> voxels;
	cvmlcpp::Geometry<float> geometry;

	cout<<endl<<"Reading File...";

	cvmlcpp::readFileSTL(geometry, stlFile);

	if(TURN_ON_SCALE){
		geometry.scaleTo(SCALE_BY);
	}
	cout<<endl<<"Voxelizing...";
	if(TURN_ON_RESOLUTION)
		cvmlcpp::voxelize(geometry, voxels, RESOLUTION);
	else
		cvmlcpp::voxelize(geometry, voxels);

	return voxels;
}

// This function is used with integrated voxelizer
vector<vector<vector<int>>> convert_to_voxels(char *stlFile){
	
	cvmlcpp::Matrix<int, 3u> voxels = getVoxelizedMatrixFromFile(stlFile);
	//return voxels;
	
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
	cout<<endl<<"Dimensions : L="<<lMax<<" B="<<hMax<<" H="<<xMax<<endl;
	int total = (xMax)*(yMax)*(zMax);
	for(int hIter = 0; hIter<hMax; hIter++){
		for(int lIter = 0; lIter<lMax; lIter++){
			for(int bIter = 0; bIter<bMax; bIter++){
				cout<<"\nWriting voxel no. "<<l<<"("<<lIter<<","<<bIter<<","<<hMax - hIter-1<<")"<<" of "<<total;
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
