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

typedef map<int, pair<int, list<int> > > Graph;

typedef stack<pair<int, int > > Stack;
typedef list<pair<int, int> > Lop;
typedef list<int> AdjList;
typedef map<int , bool> BoolDict;
typedef stack<int> IntStack;
typedef map<int, int> IntMap;

Orientation ORIENTATIONS[] = {"xy+", "xy-", "yz+", "yz-", "xz+", "xz-"};

int MAXIMUM_SIZE_OF_GENERATION = 10;
double P = .5;
int N = 6;


int calculateMaxZ(vector<vector<int> > heightmap){
		int i, j, m= heightmap.size();
		int globalMaximum=0, localMaximum=0;		
		for(i=0; i<m; i++){
			localMaximum = *max_element(heightmap[i].begin(), heightmap[i].end());
			if(localMaximum > globalMaximum){
				globalMaximum = localMaximum;
			}  
		}
		return globalMaximum;
}

bool compareMachinableVolumes(pair<Orientation, int> v1, pair<Orientation, int> v2){
	return v1.second > v2.second;
}

list<string> generateTentativeSequence(VolumetricModel &model){
	
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




void writeSequenceToFile(string folderName, list<string> sequence){
	list<string>::iterator it;

	ofstream myfile;

	myfile.open("./" + folderName + "/sequence.txt");

	for(it = sequence.begin(); it != sequence.end(); it++){
		myfile << (*it + "->");
	}

	myfile.close();
}

