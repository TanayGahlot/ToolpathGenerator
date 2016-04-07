#include <iostream>
#include <vector>

#include "lib/cvmlcpp/base/Matrix"
#include "lib/cvmlcpp/volume/Geometry"
#include "lib/cvmlcpp/volume/VolumeIO"
#include "lib/cvmlcpp/volume/Voxelizer"
#include "./model/structures.h"

using namespace std;

// Generic functions for using cvmlcpp library
//scale is passed by reference so that it can be decided 
cvmlcpp::Matrix<int, 3u> getVoxelizedMatrixFromFile(char *stlFile, int *scale){
	cvmlcpp::Matrix<int, 3u> voxels;
	cvmlcpp::Geometry<float> geometry;

	// cout<<endl<<"Reading File...";

	
	bool isVoxelized = cvmlcpp::readFileSTL(geometry, stlFile);  // need to put a try catch block here 
	
	if(!isVoxelized){
		cout<<"Error: Cannot read the file, kindly fix it ";exit(-1);
	}
	//cvmlcpp::fixGeometry();

	int boundX = (int)(geometry.max(0) - geometry.min(0));
	int boundY = (int)(geometry.max(1) - geometry.min(1));
	int boundZ = (int)(geometry.max(2) - geometry.min(2));
	float maxDim = max(boundX, max(boundY, boundZ));
	// if(maxDim < 200)
	// 	*scale = 10;
	// else if(maxDim < 500)
	// 	*scale = 5;
	// else if(maxDim < 1200)
	// 	*scale = 2;
	// else{
	// 	cout<<"Error: Given STL file is too big for this generator";
	// 	exit(-1);
	// }
	*scale = 1;
	if(TURN_ON_SCALE){
		geometry.scaleTo(SCALE_BY);
	}
	
	if(TURN_ON_RESOLUTION)
		cvmlcpp::voxelize(geometry, voxels, 1.0/(*scale));//RESOLUTION);
	else
		cvmlcpp::voxelize(geometry, voxels);

	return voxels;
}

// // This function is used with integrated voxelizer
// Voxels convert_to_voxels(char *stlFile){
	
// 	cvmlcpp::Matrix<int, 3u> voxels = getVoxelizedMatrixFromFile(stlFile);
	
// 	tr1::array<std::size_t, 3>::const_iterator i = voxels.extents();
// 	int xMax = (*i);
// 	i++;
// 	int yMax = (*i);
// 	i++;
// 	int zMax = (*i);
// 	int l=0;

// 	int lMax = yMax;
// 	int bMax = zMax;
// 	int hMax = xMax;

// 	Voxels vox(lMax, vector<vector<int> >(bMax, vector<int>(hMax, false)));
	
// 	cvmlcpp::Matrix<int,3u>::iterator iter = voxels.begin();

// 	int total = (xMax)*(yMax)*(zMax);
// 	for(int hIter = 0; hIter<hMax; hIter++){
// 		for(int lIter = 0; lIter<lMax; lIter++){
// 			for(int bIter = 0; bIter<bMax; bIter++){
// 				if((*iter) == 1)
// 					//This fix was made to resolve mirror image error
// 					vox[lIter][bIter][hMax - hIter-1] = true;
// 				else
// 					vox[lIter][bIter][hMax - hIter-1] = false;
// 				iter++; l++;
// 			}	
// 		}
// 	}	
	
// 	return vox;
	
// }
