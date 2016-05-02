//oggpnosn 
//hkhr 

//generate sequence of setup orientation in 
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



using namespace std;

string SET_OF_ORIENTATIONS[] =  {"xy+", "xy-", "yz+", "yz-", "xz+", "xz-"};

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

string print(Matrix heightmap){
	int i, j, n= heightmap.size(), m = heightmap[0].size();
	string heightmapString ="";
	for(i=0; i<n; i++){
		for(j=0; j<m; j++){
			heightmapString += to_string(heightmap[i][j]) + " ";
		}
		// cout<<"\n";
	}  
	return heightmapString;
}

string print(cvmlcpp::Matrix<int, 3u> model){

	tr1::array<std::size_t, 3>::const_iterator iter = model.extents();
	string modelString = "";
	int xmax = (*iter); iter++; int ymax = (*iter); iter++; int zmax = (*iter);
	int xmin =0, ymin =0, zmin=0;
	xmax-=1; ymax-=1; zmax-=1;
	
	for(int x=xmin; x<=xmax; x++){
		for(int y=ymin; y<=ymax; y++){
			for(int z=zmin; z<=zmax; z++){
				modelString += to_string(model[x][y][z]) + " ";
		}
		// cout<<"\n";
	}
		// cout<<"\n";
	}
	return modelString;
}

int main(int argc, char **argv){
	
	int scale;
	//voxelizing input stl file 
	//why am i using cvmlcpp? coz its fast and we need to come up with a heightmap creator which could convert heightmap and estimate volume at once 
	cvmlcpp::Matrix<int, 3u> model =  getVoxelizedMatrixFromFile(argv[1], &scale);
	tr1::array<std::size_t, 3>::const_iterator iter = model.extents();
	int length = (*iter); iter++; int width = (*iter); iter++; int height = (*iter);

	//letting the user know what the scale of model
	string jsonOutput = "";

	jsonOutput += "{\"scale\":" + to_string(scale) + ", \"heightmaps\":[";

	//getting heightmap for each orientation 
	int i;
	for(i=0; i<NO_OF_ORIENTATIONS; i++){
		pair<ll, Matrix> volumeAndHeightmap = getHeightmapAndMachinableVolume(model, SET_OF_ORIENTATIONS[i]);
		ll machinableVolume = volumeAndHeightmap.first;
		Matrix heightmap = volumeAndHeightmap.second;

		if(machinableVolume > 0){
			
			jsonOutput += "{\"orientation\": \"" + SET_OF_ORIENTATIONS[i] + "\",\"volume\": " + to_string(machinableVolume) + ", ";

			int length = heightmap.size();
			int width = heightmap[0].size();
			jsonOutput +=  "\"length\":" + to_string(length) + ", \"width\":" + to_string(width) + ",";
			jsonOutput += "\"raw\":\"";
			jsonOutput += print(heightmap);
			jsonOutput += "\"},";
		}
	}
	jsonOutput.pop_back();
	jsonOutput += "], \"model\":{";
	jsonOutput += "\"length\":" + to_string(length) + ", \"width\":" + to_string(width) + "," + "\"height\":" + to_string(height) + ",\"raw\": \"" ;
	jsonOutput += print(model);
	jsonOutput += "\"}";
	jsonOutput += "}";
	cout<<jsonOutput;
	return 0;
}
