#include <iostream>
#include<vector>
#include "cvmlcpp/base/Matrix"
#include "cvmlcpp/volume/Geometry"
#include "cvmlcpp/volume/VolumeIO"
#include "cvmlcpp/volume/Voxelizer"

#define TURN_ON_SCALE 0
#define SCALE_BY 100
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
	cvmlcpp::voxelize(geometry, voxels);

	return voxels;
}

cvmlcpp::Matrix<int, 3u> getVoxelizedMatrixFromString(char *data){
	cvmlcpp::Matrix<int, 3u> voxels;
	cvmlcpp::Geometry<float> geometry;

	//cvmlcpp library modified to use build this feature
	cvmlcpp::readStringSTL(geometry, data);
	if(TURN_ON_SCALE){
		geometry.scaleTo(SCALE_BY);
	}
	cout<<"\nVoxelizing...";
	cvmlcpp::voxelize(geometry, voxels);

	return voxels;
}

// This function returns voxel string for standalone voxelizer
string getVoxelizedString(char *data, string inputFormat){
	
	cvmlcpp::Matrix<int, 3u> voxels; 
	if(inputFormat == "file"){
		voxels = getVoxelizedMatrixFromFile(data);
	}
	else if(inputFormat == "string"){
		voxels = getVoxelizedMatrixFromString(data);
	}
	

	string vox_string ="";
	
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
	
	
	cout<<endl<<"Dimensions : L="<<lMax<<" B="<<hMax<<" H="<<xMax<<endl;

	string frame = "";
	for(cvmlcpp::Matrix<int,3u>::iterator iter = voxels.begin(); iter != voxels.end();l++){
				//This fix was made to resolve mirror image error
				vox_string = "\n\n\n" + frame + vox_string;
				frame = "";
				for(int m =0; m<yMax; m++){	
						frame = frame + "\n";
						for(int n = 0; n<zMax; n++){								
								cout<<"\rWriting voxel no. "<<l<<" of "<<(xMax)*(yMax)*(zMax);
								l++;
								frame = frame + to_string((*iter));
								iter++;
						}
				}
		
	}
	vox_string = to_string(lMax) + " " + to_string(bMax) + " " + to_string(hMax) + vox_string;
	
	return vox_string;
}

// This function returns voxel string for standalone voxelizer in JSON format
string getVoxelizedJSONString(char *data, string inputFormat){
	
	cvmlcpp::Matrix<int, 3u> voxels; 
	if(inputFormat == "file"){
		voxels = getVoxelizedMatrixFromFile(data);
	}
	else if(inputFormat == "string"){
		voxels = getVoxelizedMatrixFromString(data);
	}

	string vox_string ="";
	
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
	
	
	cout<<endl<<"Dimensions : L="<<lMax<<" B="<<hMax<<" H="<<xMax<<endl;

	//writes string in JSON Format as required by the web interface

	vox_string = "{ \n \
			\"length\" : \"" + to_string(lMax) + "\",\n \
			\"breadth\" : \"" + to_string(bMax) + "\",\n \
			\"height\" : \"" + to_string(hMax) + "\",\n \
			\"stream\" : \"";

	string stream = "";
	string frame = "";
	for(cvmlcpp::Matrix<int,3u>::iterator iter = voxels.begin(); iter != voxels.end();l++){
				//This fix was made to resolve mirror image error
				stream = frame + stream;
				frame = "";
				for(int m =0; m<yMax; m++){
						for(int n = 0; n<zMax; n++){								
								cout<<"\rWriting voxel no. "<<l<<" of "<<(xMax)*(yMax)*(zMax);
								l++;
								frame = frame + to_string((*iter));
								iter++;
						}
				}
		
	}
	vox_string = vox_string + stream + "\"\n }";
	
	return vox_string;
}


// This function is used with integrated voxelizer
std::vector<vector<vector<int>>> convert_to_voxels(char *stlFile)
{
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
