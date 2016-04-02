//oggpnson 
//hkhr 

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>

#include "lib/cvmlcpp/base/Matrix"
#include "model/structures.h"
#include "model/ToolConfig.h"
#include "model/VolumetricModel.h"
#include "voxelizer.h"
#include "sequenceGenerator.h"
#include "toolpathGenerator.h"


int main(int argc, char **argv){
 
	string folderName = static_cast<string>(argv[2]);
	
	/* Arguement 1 has stl file address which is converted to voxels. */
	vector<vector<vector<int>>> voxels = convert_to_voxels(argv[1]);
	
	int lMax = voxels.size();
	int bMax = voxels[0].size();
	int hMax = voxels[0][0].size();

	ToolConfig toolConfig;
	
	char boolVal; 
	bool printVolume = true;
	int objectsVolume=0;
	list<string> toolpaths;

	/* Initialise Volumetric Model */
	VolumetricModel model(voxels, lMax, bMax, hMax);
	
	/* Generate Tentative Sequence of Orientations */
	list<pair<Orientation, HeightMap> > sequence = generateSequence(model, folderName);
	
	/* Operation Planning for each Orientation */
	for(list<pair<Orientation, HeightMap>>::iterator it = sequence.begin(); it != sequence.end(); it++){
		
		string toolpath = planOperation(model, toolConfig, (*it).first, (*it).second);
		toolpaths.push_back(toolpath);
	}
	
	//writing sequence to the test file 
	writeSequenceToFile(folderName, sequence);

	//saving toolpath in folder "OrientationOutput"
	list<pair<Orientation, HeightMap> >::iterator seqIter = sequence.begin();
 	for(list<string>::iterator toolpathIter = toolpaths.begin(); toolpathIter != toolpaths.end(); toolpathIter++){
 		ofstream myfile;
 		cout<<(*seqIter).first<<" ";
 		myfile.open ("./" + folderName + "/" + (*seqIter).first + ".gcode" );
 		myfile<<*toolpathIter;
 		seqIter++;
 		myfile.close();
	}

	return 0;
}
