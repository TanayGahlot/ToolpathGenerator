//oggpnson 
//hkhr 

#include<iostream>
#include<stdint.h>
#include<list>
#include<vector>
#include<map>
#include<stack>
#include<algorithm>
#include <fstream>

//#define ll long long int
#define NO_OF_ORIENTATIONS 6
#define GENERATE_MODELS 1

using namespace std; 

Orientation ORIENTATIONS[] = {"xy+", "xy-", "yz+", "yz-", "xz+", "xz-"};

int MAXIMUM_SIZE_OF_GENERATION = 10;
double P = .5;
int N = 6;

bool compareMachinableVolumes(pair<Orientation, int> v1, pair<Orientation, int> v2){
	return v1.second > v2.second;
}

void generateFilledModels(VolumetricModel &model, Orientation currentOrientation, string folderName){
	ofstream myfile;
	myfile.open ("./" + folderName + "/" + currentOrientation + ".interim.raw", ios::binary );
	model.print(myfile);
}

list<pair<Orientation, HeightMap>> generateSequence(VolumetricModel model, string folderName){
	
	vector<pair<Orientation, int>> orientationList(NO_OF_ORIENTATIONS);
	int machinedVolume = 0;
	
	/* Find Machinable volume for each  orientation */
	for(int i=0; i<NO_OF_ORIENTATIONS; i++){
		orientationList[i] = make_pair(ORIENTATIONS[i], model.calculateMachinableVolume(ORIENTATIONS[i]));
	}

	/* Sort the orientations */
	sort(orientationList.begin(), orientationList.end(), compareMachinableVolumes);

	list<pair<Orientation, HeightMap> > sequence;

	for(int i=0; i<NO_OF_ORIENTATIONS; i++){
		
		if(model.calculateMachinableVolume(orientationList[i].first) != 0){
			
			HeightMap heightMap = model.toHeightmap(orientationList[i].first);
					
			int machinedVolumeInOrientation = model.fillMachinableVolume(orientationList[i].first, heightMap);
			
			if(machinedVolumeInOrientation){
				
				if(GENERATE_MODELS){
					generateFilledModels(model, orientationList[i].first, folderName);
				}
				
				sequence.push_back(make_pair(orientationList[i].first, heightMap));
			}
			
			machinedVolume += machinedVolumeInOrientation;
		}
		
	}
	
	if(machinedVolume != ((model.xmax+1)*(model.ymax+1)*(model.zmax+1) - model.volume)){
		cout<<"model volume:"<<model.volume<<"\n";
		cout<<"model total volume:"<<(model.xmax+1)*(model.ymax+1)*(model.zmax+1)<<"\n";
		cout<<"Volume to be machined:"<<(model.xmax+1)*(model.ymax+1)*(model.zmax+1) - model.volume<<"\n";
		cout<<"Machined Volume: "<< machinedVolume<<"\n";
		cout<<"Sorry , the object cannot be machined\n";
	}
	return sequence;
}

void writeSequenceToFile(string folderName, list<pair<Orientation, HeightMap> > &sequence){


	ofstream myfile;

	myfile.open("./" + folderName + "/sequence.txt");

	for(list<pair<Orientation, HeightMap> >::iterator it = sequence.begin(); it != sequence.end(); it++){
		myfile << ((*it).first + " > ");
	}

	myfile.close();
}
