#include <vector>

#include "structures.h"


class VolumetricModel{
	public:

		int xmax, ymax, zmax;
		int xmin, ymin, zmin;
		int volume;
		vector<vector<vector<int> > > model;
		
		VolumetricModel(vector<vector<vector<int> > > voxels, int lMax, int bMax, int hMax);
		int getVolume();
		ll calculateMachinableVolume(string orientation);
		int fillMachinableVolume(Orientation orientation, HeightMap heightmap);
		HeightMap toHeightmap(string orientation);
		void print(ofstream &fileStream);
};

void VolumetricModel::print(ofstream &fileStream){
	/*input: None
	  purpose: prints voxels for debugging purpose 
	  output: None
	 */
	
	int x, y,z;
	for(z=zmin; z<=zmax; z++){
		for(x=xmin; x<=xmax; x++){
			for(y=ymin; y<=ymax; y++){
				if(model[x][y][z] == true){
					fileStream << std::hex << 0x1;	
				}
				else if(model[x][y][z] == false){
					fileStream << std::hex << 0x0;	
				}
				else{
					fileStream << std::hex << 0x2;	
				}
			}	
		}		
	}
}

VolumetricModel::VolumetricModel(vector<vector<vector<int> > > voxels, int lMax, int bMax, int hMax){
	xmin =0; xmax = lMax-1;
	ymin =0; ymax = bMax-1;
	zmin =0; zmax = hMax-1;

	model = voxels;
	
	volume = getVolume();
}

int VolumetricModel::getVolume(){
	int objVolume = 0;
	
	/* Calculate Empty Space in the Volume */
	for(int zIter = zmin; zIter<=zmax; zIter++){
		for(int xIter = xmin; xIter<=xmax; xIter++){
			for(int yIter = ymin; yIter<=ymax; yIter++){
				if(model[xIter][yIter][zIter] == 1)
					objVolume += 1;				
			}	
		}
	}
	
	return objVolume;
}
	
ll VolumetricModel::calculateMachinableVolume(Orientation orientation){
	
	ll machinableVolume = 0;
	
	if(orientation == "xy+"){
		for(int x=xmin; x<=xmax; x++){
			for(int y=ymin; y<=ymax; y++){
				for(int z=zmax; z>=zmin; z--){
					if(model[x][y][z] == true)
						break;
					else if(model[x][y][z] == false)
						machinableVolume += 1;
				}
				
			}
		}
	}
	else if(orientation == "xy-"){
		for(int x=xmin; x<=xmax; x++){
			for(int y=ymin; y<=ymax; y++){
				for(int z=zmin; z<=zmax; z++){
					if(model[x][y][z] == true){
						break;
					}
					else if(model[x][y][z] == false)
						machinableVolume += 1;
				}
			}
		}	
	}
	else if(orientation == "yz+"){
		for(int y=ymin; y<=ymax; y++){
			for(int z=zmin; z<=zmax; z++){
				for(int x= xmax; x>=xmin; x--){
					if(model[x][y][z] == true)
						break;	
					else if(model[x][y][z] == false)
						machinableVolume += 1;
				}		
				
			}
		}
	} 
	else if(orientation == "yz-"){
		for(int y=ymin; y<=ymax; y++){
			for(int z=zmin; z<=zmax; z++){
				for(int x= xmin; x<=xmax; x++){
					if(model[x][y][z] == true){
						break;	
					}
					else if(model[x][y][z] == false)
						machinableVolume += 1;
				}		
				
			}
		}
	}
	else if(orientation == "xz+"){
		for(int x=xmin; x<=xmax; x++){
			for(int z=zmin; z<=zmax; z++){
				for(int y=ymax; y>=ymin; y--){
					if(model[x][y][z] == true)
						break;	
					else if(model[x][y][z] == false)
						machinableVolume += 1;
				}
				
			}
		}
	}
	else if(orientation == "xz-"){
		for(int x=xmin; x<=xmax; x++){
			for(int z=zmin; z<=zmax; z++){
				for(int y=ymin; y<=ymax; y++){
					if(model[x][y][z] == true)
						break;	
					else if(model[x][y][z] == false)
						machinableVolume += 1;
				}
				
			}
		}
	}

	return machinableVolume;
}

int VolumetricModel::fillMachinableVolume(Orientation orientation, HeightMap heightMap){
	//Optimization: this function isnt required since this job can be done while calculating machinable volume 

	int x, y, z;
	int machinedVolume=0;
	
	if(orientation == "xy+"){
		for(x=xmin; x<=xmax; x++){
			for(y=ymin; y<=ymax; y++){
				for(z=zmax; z>=heightMap.first[x][y]; z--){
					if(model[x][y][z] == false){
						machinedVolume +=1;
					}
					model[x][y][z] = 2;
					
				}
				//machinedVolume += (zmax-heightmap[x][y]-1);
			}
		}
	}
	else if(orientation == "xy-"){
		for(x=xmin; x<=xmax; x++){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax-heightMap.first[x][y]; z++){
					if(model[x][y][z] == false){
						machinedVolume +=1;
					}
					model[x][y][z] = 2;
				}
				// machinedVolume += (heightmap[x][y]-1);
			}
		}	
	}
	else if(orientation == "yz+"){
		for(y=ymin; y<=ymax; y++){
			for(z=zmin; z<=zmax; z++){
				for(x= xmax; x>=heightMap.first[y][z]; x--){
					if(model[x][y][z] == false){
						machinedVolume +=1;
					}
					model[x][y][z] = 2;
				}	
				// machinedVolume += (xmax-heightmap[y][z]-1);	
			}
		}
	} 
	else if(orientation == "yz-"){
		for(y=ymin; y<=ymax; y++){
			for(z=zmin; z<=zmax; z++){
				//cout<<"("<<y<<","<<z<<"): ";
				for(x= xmin; x<=xmax- heightMap.first[y][z]; x++){
					if(model[x][y][z] == false){
						machinedVolume +=1;
					}
					
					model[x][y][z] = 2;
				}		
				//cout<<"\n";
				// machinedVolume += (heightmap[y][z]-1);
			}

		}
	}
	else if(orientation == "xz+"){
		for(x=xmin; x<=xmax; x++){
			for(z=zmin; z<=zmax; z++){
				for(y=ymax; y>=heightMap.first[x][z]; y--){	
					if(model[x][y][z] == false){
						machinedVolume +=1;
					}
					model[x][y][z] = 2;
				}
				// machinedVolume += (ymax-heightmap[x][z]-1);
			}
		}
	}
	else if(orientation == "xz-"){
		for(x=xmin; x<=xmax; x++){
			for(z=zmin; z<=zmax; z++){
				for(y=ymin; y<=ymax- heightMap.first[x][z]; y++){
					if(model[x][y][z] == false){
						machinedVolume +=1;
					}
					model[x][y][z] = 2;
				}
				// machinedVolume += (heightmap[x][z]-1);		
			}
		}
	}
	return machinedVolume;
}

HeightMap VolumetricModel::toHeightmap(Orientation orientation){
	//Optimization: this function isnt required since this job can be done while calculating machinable volume 
	/*
		input: orientation in which heightmap is to be created 
		purpose: it converts the give  volumetric model to heightmap in an orientation 
		output: heightmap
	*/
	int x, y, z;
	Matrix HM;
	
	if(orientation == "xy+"){
		HM = *(new Matrix(xmax-xmin+1, vector<int>(ymax-ymin+1, 0)));
		for(x=xmin; x<=xmax; x++){
			for(y=ymin; y<=ymax; y++){
				for(z=zmax; z>=zmin; z--){
					if(model[x][y][z] == true){
						HM[x][y] = z - zmin + 1;
						break;
					}
				}
			}
		}
	}
	else if(orientation == "xy-"){
		HM = *(new Matrix(xmax-xmin+1, vector<int>(ymax-ymin+1, 0)));
		for(x=xmin; x<=xmax; x++){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					if(model[x][y][z] == true){
						HM[x][y] = zmax - z +1;
						break;
					}
					
				}
			}
		}	
	}
	else if(orientation == "yz+"){
		HM = *(new Matrix(ymax-ymin+1, vector<int>(zmax-zmin+1, 0)));
		for(y=ymin; y<=ymax; y++){
			for(z=zmin; z<=zmax; z++){
				for(x= xmax; x>=xmin; x--){
					if(model[x][y][z] == true){
						HM[y][z] = x - xmin +1;
						break;	
					}
					
				}		
			}
		}
	} 
	else if(orientation == "yz-"){
		HM = *(new Matrix(ymax-ymin+1, vector<int>(zmax-zmin+1, 0)));
		for(y=ymin; y<=ymax; y++){
			for(z=zmin; z<=zmax; z++){
				for(x= xmin; x<=xmax; x++){
					if(model[x][y][z] == true){
						HM[y][z] = xmax + 1 - x;
						break;	
					}
				}		
			}
		}
	}
	else if(orientation == "xz+"){
		HM = *(new Matrix(xmax-xmin+1, vector<int>(zmax-zmin+1, 0)));
		for(x=xmin; x<=xmax; x++){
			for(z=zmin; z<=zmax; z++){
				for(y=ymax; y>=ymin; y--){
					if(model[x][y][z] == true){
						HM[x][z] = y - ymin + 1;
						break;	
					}
				}
			}
		}
	}
	else if(orientation == "xz-"){
		HM = *(new Matrix(xmax-xmin+1, vector<int>(zmax-zmin+1, 0)));
		for(x=xmin; x<=xmax; x++){
			for(z=zmin; z<=zmax; z++){
				for(y=ymin; y<=ymax; y++){
					if(model[x][y][z] == true){
						HM[x][z] = ymax -y +1;
						break;		
					}
				}
			}
		}
	}
	
	HeightMap heightMap;
	heightMap.first = HM;
	heightMap.second.first = HM.size();
	heightMap.second.second = HM[0].size();
	
	return heightMap;
}

