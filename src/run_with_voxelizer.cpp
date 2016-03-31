//oggpnson 
//hkhr 

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>

#include "model/structures.h"
#include "model/ToolConfig.h"
#include "model/VolumetricModel.h"
#include "voxelizer.h"
#include "sequenceGenerator.h"
#include "toolpathGenerator.h"


int main(int argc, char **argv){
 
	string folderName = static_cast<string>(argv[2]);
	
	/* Arguement 1 has stl file address which is converted to voxels. */
	vector<vector<vector<int> > > voxels = convert_to_voxels(argv[1]);
	int lMax = voxels.size();
	int bMax = voxels[0].size();
	int hMax = voxels[0][0].size();

	ToolConfig toolConfig;
	
	char boolVal; 
	bool printVolume = true;
	int objectsVolume=0;

	/* Initialise Volumetric Model */
	VolumetricModel model(voxels, lMax, bMax, hMax);
	
	/* Generate Tentative Sequence of Orientations */
	list<string> tentativeSequence = generateTentativeSequence(model);
	
	list<string>::iterator it;
	
	/* Operation Planning for each Orientation */
	pair<list<Orientation>, list<string> > skk = planOperation(model, toolConfig, tentativeSequence, folderName, printVolume);
	list<string> sk = skk.second;
	list<Orientation> sequence = skk.first;
	
	list<Orientation>::iterator itt;
	itt = sequence.begin();

	//writing sequence to the test file 
	writeSequenceToFile(folderName, sequence);

	//saving toolpath in folder "OrientationOutput"
 	for(it = sk.begin(); it != sk.end(); it++){
 		ofstream myfile;
 		myfile.open ("./" + folderName + "/" + *itt + ".gcode" );
 		myfile<<*it;
 		itt++;
 		myfile.close();
	}

	return 0;
}
