//oggpnosn 
//hkhr 

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>
#include <string>

#include "model/utility.h"
#include "model/structures.h"
#include "model/ToolConfig.h"
#include "model/VolumetricModel.h"
#include "voxelizer.h"


//heightmap generator 
using namespace std;

string SET_OF_ORIENTATIONS[] =  {"xy+", "xy-", "xz+", "xz-", "yz+", "yz-"};

pair<ll, Matrix> getHeightmapAndMachinableVolume(cvmlcpp::Matrix<int, 3u> &model, string orientation){
	
	ll machinableVolume = 0;
	tr1::array<std::size_t, 3>::const_iterator iter = model.extents();
	int xmax = (*iter); iter++; int ymax = (*iter); iter++; int zmax = (*iter);
	int xmin =0, ymin =0, zmin=0;
	xmax-=1; ymax-=1; zmax-=1;

	Matrix heightmap;
	
	if(orientation == "xy+"){
		heightmap = *(new Matrix(xmax-xmin+1, vector<int>(ymax-ymin+1, 0)));
		for(int x=xmin; x<=xmax; x++){
			for(int y=ymin; y<=ymax; y++){
				for(int z=zmax; z>=zmin; z--){
					if(model[x][y][z] == 1){
						heightmap[x][y] = z - zmin + 1;
						break;
					}
					else if(model[x][y][z] == 0){
						machinableVolume += 1;
						model[x][y][z] = 11;
					}

				}
				
			}
		}
	}
	else if(orientation == "xy-"){
		heightmap = *(new Matrix(xmax-xmin+1, vector<int>(ymax-ymin+1, 0)));
		for(int x=xmin; x<=xmax; x++){
			for(int y=ymin; y<=ymax; y++){
				for(int z=zmin; z<=zmax; z++){
					if(model[x][y][z] == 1){
						heightmap[x][y] = zmax - z +1;
						break;
					}
					else if(model[x][y][z] == 0){
						machinableVolume += 1;
						model[x][y][z] = 12;
					}
				}
			}
		}	
	}
	else if(orientation == "yz+"){
		heightmap = *(new Matrix(ymax-ymin+1, vector<int>(zmax-zmin+1, 0)));
		for(int y=ymin; y<=ymax; y++){
			for(int z=zmin; z<=zmax; z++){
				for(int x= xmax; x>=xmin; x--){
					if(model[x][y][z] == 1){
						heightmap[y][z] = x- xmin +1;
						break;	
					}
					else if(model[x][y][z] == 0){
						machinableVolume += 1;
						model[x][y][z] = 13;
					}
				}		
				
			}
		}
	} 
	else if(orientation == "yz-"){
		heightmap = *(new Matrix(ymax-ymin+1, vector<int>(zmax-zmin+1, 0)));
		for(int y=ymin; y<=ymax; y++){
			for(int z=zmin; z<=zmax; z++){
				for(int x= xmin; x<=xmax; x++){
					if(model[x][y][z] == 1){
						heightmap[y][z] = xmax - x +1;
						break;	
					}
					else if(model[x][y][z] == 0){
						machinableVolume += 1;
						model[x][y][z] = 14;
					}
				}		
				
			}
		}
	}
	else if(orientation == "xz+"){
		heightmap = *(new Matrix(xmax-xmin+1, vector<int>(zmax-zmin+1, 0)));
		for(int x=xmin; x<=xmax; x++){
			for(int z=zmin; z<=zmax; z++){
				for(int y=ymax; y>=ymin; y--){
					if(model[x][y][z] == 1){
						heightmap[x][z] = y - ymin +1;
						break;	
					}
					else if(model[x][y][z] == 0){
						machinableVolume += 1;
						model[x][y][z] = 15;
					}
				}
				
			}
		}
	}
	else if(orientation == "xz-"){
		heightmap = *(new Matrix(xmax-xmin+1, vector<int>(zmax-zmin+1, 0)));
		for(int x=xmin; x<=xmax; x++){
			for(int z=zmin; z<=zmax; z++){
				for(int y=ymin; y<=ymax; y++){
					if(model[x][y][z] == 1){
						heightmap[x][z] = ymax -y +1;
						break;	
					}
					else if(model[x][y][z] == 0){
						machinableVolume += 1;
						model[x][y][z] = 16;
					}
				}
				
			}
		}
	}
	return make_pair(machinableVolume, heightmap);
}

void print(Matrix heightmap){
	int i, j, n= heightmap.size(), m = heightmap[0].size();
	for(i=0; i<n; i++){
		for(j=0; j<m; j++){
			cout<<heightmap[i][j] <<" ";
		}
		cout<<"\n";
	}  
}

void print(cvmlcpp::Matrix<int, 3u> model){

	tr1::array<std::size_t, 3>::const_iterator iter = model.extents();
	int xmax = (*iter); iter++; int ymax = (*iter); iter++; int zmax = (*iter);
	int xmin =0, ymin =0, zmin=0;
	xmax-=1; ymax-=1; zmax-=1;
	for(int z=zmin; z<=zmax; z++){
		for(int x=xmin; x<=xmax; x++){
			for(int y=ymin; y<=ymax; y++){
					cout<<model[x][y][z]<<" ";
			}
			cout<<"\n";
		}
		cout<<"\n";
	}
}

int main(int argc, char **argv){
	
	int scale;
	//voxelizing input stl file 
	//why am i using cvmlcpp? coz its fast and we need to come up with a heightmap creator which could convert heightmap and estimate volume at once 
	cvmlcpp::Matrix<int, 3u> model =  getVoxelizedMatrixFromFile(argv[1], &scale);

	//getting heightmap for each orientation 
	int i;
	for(i=0; i<NO_OF_ORIENTATIONS; i++){
		pair<ll, Matrix> volumeAndHeightmap = getHeightmapAndMachinableVolume(model, SET_OF_ORIENTATIONS[i]);
		ll machinableVolume = volumeAndHeightmap.first;
		Matrix heightmap = volumeAndHeightmap.second;

		if(machinableVolume > 0){
			cout<< SET_OF_ORIENTATIONS[i]<<":"<<machinableVolume<<"\n";
			print(heightmap);
		}
	}
	//print(model);

	return 0;
}