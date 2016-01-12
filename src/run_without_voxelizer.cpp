//oggpnson 
//hkhr 

#include<iostream>
#include<list>
#include<vector>
#include<map>
#include<stack>
#include<algorithm>
#include "toolpathGenerator.h"
#include "sequenceGenerator.h"
#include <fstream>

using namespace std;

int main(int argc, char *argv[]){

	//length, breadth, height of the model along with the corresponding iterator 
	int lMax, bMax, hMax, lIter, bIter, hIter;
	cin>>lMax>>bMax>>hMax; 
	string folderName = static_cast<string>(argv[1]);


	//data model for storing voxels 
	vector<vector<vector<int> > > voxels(lMax, vector<vector<int> >(bMax, vector<int>(hMax, false)));
	char boolVal; 
	int TOOL_DIA = 1;
	int feedrate = 500;
	int depthPerPass=1;
	bool printVolume = true;
	int toolRadius = 10;
	int toolLength = 100;
	long long int objectsVolume=0;
	
	//taking voxel input  
	for(hIter = 0; hIter<hMax; hIter++){
		for(lIter = 0; lIter<lMax; lIter++){
			for(bIter = 0; bIter<bMax; bIter++){
				cin>>boolVal;
				if(boolVal == '1'){
					voxels[lIter][bIter][hIter] = true;
					objectsVolume += 1;
				}
				else
					voxels[lIter][bIter][hIter] = false;
			}	
		}
	}

	

	//taking voxel input  
	//for(hIter = 0; hIter<hMax; hIter++){
	//	for(lIter = 0; lIter<lMax; lIter++){
	//		for(bIter = 0; bIter<bMax; bIter++){
	//			cin>>boolVal;
	//			if(boolVal == '1')
	//				voxels[lIter][bIter][hIter] = true;
	//			else
	//				voxels[lIter][bIter][hIter] = false;
	//		}	
	//	}
	//}

	//cout<<voxels[25][6][13]<<" *"<<voxels[23][6][13]<<"Yaha dekho!!\n";
	//internal representation of the model 
	VolumetricModel model(voxels, lMax, bMax, hMax);

	//generating sequence using genetic algorithm 
	list<string> sequence = generateSequence(model);
	// list<string> sequence;
	// int i;
	// for(i=0; i<NOOFORIENTATION; i++)
	// 	sequence.push_back(ORIENTATION[i]);

	list<string>::iterator it;

	
	//generating toolpath for the optimal sequence thats produced by sequence generator 
	pair<list<string>, list<string> > skk = toolpathGeneratorForSequence(sequence, model, TOOL_DIA,lMax,bMax, hMax,folderName, printVolume, feedrate, depthPerPass, toolRadius, toolLength, objectsVolume);
	list<string> sk = skk.second;
	sequence = skk.first;

	list<string>::iterator itt;
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

	// // list<string> sk;
	// // sk.push_back("xz+");
	// // cout<< toolpathGeneratorForSequence(sk, model, TOOL_DIA, lMax, bMax, hMax);


	return 0;
}
