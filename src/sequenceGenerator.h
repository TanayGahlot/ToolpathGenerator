//oggpnson 
//hkhr 

#include <iostream>
#include <stdint.h>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std; 

Orientation ORIENTATIONS[] = {"xy+", "xy-", "yz+", "yz-", "xz+", "xz-"};

/* Comparator function for sorting. Compares two orientation based on volumes */
bool compareMachinableVolumes(pair<Orientation, int> v1, pair<Orientation, int> v2){
	return v1.second > v2.second;
}

/* Debug function for Qvox visualization. Generates Qvox raw files */
void generateFilledModels(VolumetricModel &model, Orientation currentOrientation, string folderName){
	ofstream myfile;
	myfile.open ("./" + folderName + "/" + currentOrientation + ".interim.raw", ios::binary );
	model.print(myfile);
}

/* Returns sequence of orientations for the given Volumetric model. */
/* Note that model is passed by value so that ether filling does not reflect back */
list<pair<Orientation, HeightMap>> generateSequence(VolumetricModel model, string folderName){
	
	vector<pair<Orientation, int>> orientationList(NO_OF_ORIENTATIONS);
	int machinedVolume = 0;
	
	/* Find Machinable volume for each  orientation */
	for(int i=0; i<NO_OF_ORIENTATIONS; i++){
		orientationList[i] = make_pair(ORIENTATIONS[i], model.calculateMachinableVolume(ORIENTATIONS[i]));
	}

	/* Sort the orientations based on machinable volume */
	sort(orientationList.begin(), orientationList.end(), compareMachinableVolumes);

	list<pair<Orientation, HeightMap> > sequence;

	for(int i=0; i<NO_OF_ORIENTATIONS; i++){
		
		/* Check if volume is left to be machined in current orientation */
		if(model.calculateMachinableVolume(orientationList[i].first) != 0){
			
			/* Generate height map for the current orientation. Relocated here for future parallelization */
			HeightMap heightMap = model.toHeightmap(orientationList[i].first);
			
			/* Ether filling. Note that this ether filling does not affect original model */
			int machinedVolumeInOrientation = model.fillMachinableVolume(orientationList[i].first, heightMap);
			
			
			if(machinedVolumeInOrientation){
				
				if(GENERATE_MODELS){
					generateFilledModels(model, orientationList[i].first, folderName);
				}
				
				/* If volume is machined in the orientation, push this orientation in sequence for further processing */
				sequence.push_back(make_pair(orientationList[i].first, heightMap));
			}
			
			machinedVolume += machinedVolumeInOrientation;
		}
		
	}
	
	if(machinedVolume != ((model.xmax+1)*(model.ymax+1)*(model.zmax+1) - model.volume)){
		cerr<<"\n"<<float(machinedVolume)/((model.xmax+1)*(model.ymax+1)*(model.zmax+1) - model.volume)<<"% model non machinable.";
	}
	return sequence;
}

/* Write Sequence to the file */
void writeSequenceToFile(string folderName, list<pair<Orientation, HeightMap> > &sequence){
	ofstream myfile;

	myfile.open("./" + folderName + "/sequence.txt");

	for(list<pair<Orientation, HeightMap> >::iterator it = sequence.begin(); it != sequence.end(); it++){
		myfile << ((*it).first + " > ");
	}

	myfile.close();
}
