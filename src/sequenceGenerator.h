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

using namespace std; 

Orientation ORIENTATIONS[] = {"xy+", "xy-", "yz+", "yz-", "xz+", "xz-"};

int MAXIMUM_SIZE_OF_GENERATION = 10;
double P = .5;
int N = 6;

bool compareMachinableVolumes(pair<Orientation, int> v1, pair<Orientation, int> v2){
	return v1.second > v2.second;
}

list<Orientation> generateTentativeSequence(VolumetricModel &model){
	
	vector<pair<Orientation, int>> orientationList(NO_OF_ORIENTATIONS);
	
	/* Find Machinable volume for each  orientation */
	for(int i=0; i<NO_OF_ORIENTATIONS; i++){
		orientationList[i] = make_pair(ORIENTATIONS[i], model.calculateMachinableVolume(ORIENTATIONS[i]));
	}

	/* Sort the orientations */
	sort(orientationList.begin(), orientationList.end(), compareMachinableVolumes);

	list<Orientation> tentativeSequence;

	for(int i=0; i<NO_OF_ORIENTATIONS; i++){
		tentativeSequence.push_back(orientationList[i].first);
	}

	return tentativeSequence;
}

void writeSequenceToFile(string folderName, list<Orientation> sequence){
	list<string>::iterator it;

	ofstream myfile;

	myfile.open("./" + folderName + "/sequence.txt");

	for(it = sequence.begin(); it != sequence.end(); it++){
		myfile << (*it + "->");
	}

	myfile.close();
}
