//oggpnson 
//hkhr 

#include<iostream>
#include<list>
#include<vector>
#include<map>
#include<stack>
#include<algorithm>
#include "toolpathGenerator.h"
#include <fstream>

using namespace std; 

#define ll long long int

typedef map<int, pair<int, list<int> > > Graph;
typedef vector< vector<int> > Matrix;
typedef stack<pair<int, int > > Stack;
typedef list<pair<int, int> > Lop;
typedef list<int> AdjList;
typedef map<int , bool> BoolDict;
typedef stack<int> IntStack;
typedef map<int, int> IntMap;

//state variable that can be configured to experiment 
string ORIENTATION[] = {"xy+", "xy-", "yz+", "yz-", "xz+", "xz-"};
int NOOFORIENTATION = 6;
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

//VolumetricModel class handles volumetric representation



//supportive class type chromosome that repersents a set of orientation 
class Chromosome{
public:
	int currentGeneration;
	VolumetricModel model;
	string currentOrientation;
	list<string> orientationLeft;
	list<string> sequence;
	list<ll> volumes;
	ll quality;

	Chromosome(VolumetricModel model, string orientation):model(model), currentOrientation(orientation){
		int i;
		currentGeneration = 6;
		quality =0;
		for(i=0; i<6; i++){
			if(ORIENTATION[i] != orientation)
				orientationLeft.push_back(ORIENTATION[i]);
		}
		//adding orientation to sequence list 
		sequence.push_back(orientation);
		
	}

	Chromosome(VolumetricModel model, string orientation, list<string> orientationToConsider, int currentGeneration, list<string> sequencePreviousGeneration, int quality, list<ll> volumes):model(model), currentGeneration(currentGeneration), quality(quality), volumes(volumes){
		orientationLeft = orientationToConsider;
		
		//removing orientation from orientation to consider 
		list<string>::iterator it = find(orientationLeft.begin(), orientationLeft.end(), orientation);
		orientationLeft.erase(it);
		
		//assigning current orientation to orientation 
		currentOrientation = orientation;

		//adding new orientation to sequence
		sequence = sequencePreviousGeneration;
		sequence.push_back(orientation);
	}

	list<Chromosome> generateOffsprings(){
		int i;
		list<Chromosome> offspring;
		string orientation;
		list<string>::iterator it;
		for(it=orientationLeft.begin(); it!=orientationLeft.end(); it++){
			Chromosome *child = new Chromosome(model, *it, orientationLeft, currentGeneration-1, sequence, quality, volumes);
			offspring.push_back(*child);
		}
		
		return offspring; 
	}	

	ll calculateMachinableVolume(){
		/*input: None
		   purpose: calculate volume that can be machined 
		   output: volume that can be machined
		*/
		ll volume = model.calculateMachinableVolume(currentOrientation);
		return volume;
	}

	void fillMachinableVolume(){
		/*input: None
		   purpose: fills the machinable volume in the direction of current orientation  
		   output: None  
		*/
		model.fillMachinableVolume(currentOrientation);
	}

};

void operator<<(ostream &out, Chromosome &organism){
	list<string>::iterator it;
	list<ll>::iterator itt = organism.volumes.begin();

	for(it= organism.sequence.begin(); it!= organism.sequence.end(); it++){
		if(*itt != 0)
			out<<*it<<"("<<*itt<<")"<<" -> ";
		itt++;
	}

	out<<"\n";
}

bool compareChromosomes(Chromosome c1, Chromosome c2){
	//this sorts in descending order 
	return c1.quality>c2.quality;	
}



//implementing mutation function that generates organization 
list<Chromosome> mutation(list<Chromosome> &generation){
	list<Chromosome> *newGeneration = new list<Chromosome>();
	list<Chromosome>::iterator it;
	for(it = generation.begin(); it != generation.end(); it++){
		//generating offsppring forn current generation 
		list<Chromosome> offsprings =  it->generateOffsprings();
		list<Chromosome>::iterator nit;
		for(nit = offsprings.begin(); nit!=offsprings.end(); nit++){
			newGeneration->push_back(*nit);
		}
	}
	return *newGeneration;
}


void volumeEstimator(list<Chromosome> &generation){
	/*input: list of Chromosomes
	   purpose: for each chromosome calculate volume that can be machined 
	   output: each chromosome will be assigned a machinable volume and quality score will be calculated 
	*/

	list<Chromosome>::iterator organismIterator;
	ll volume;

	for(organismIterator = generation.begin(); organismIterator != generation.end(); organismIterator++){

		//calculate the volume that can be machined 
		volume = organismIterator->calculateMachinableVolume();

		//update the list of volumes with the current volume
		(organismIterator->volumes).push_back(volume);

		//update the quality score for assessment 
		organismIterator->quality += (organismIterator->currentGeneration*volume);
	}

}


void fillVolume(list<Chromosome> &generation){
	/*
		input: generation of chromosomes 
		purpose: to mutate the model to fill the volume that being machined 
		output: Mutated model object in chromosomes 
	*/
	list<Chromosome>::iterator it;

	for(it = generation.begin(); it != generation.end(); it++){
		it->fillMachinableVolume();
	}

}
//compares volume of pair, used for sorting 
bool compareVolume(pair<int, string> v1, pair<int, string> v2){
	return v1.first > v2.first;
}

//fitness function using quality methds not quality rank as mentioned in the paper
list<Chromosome> selection(list<Chromosome> &generation){
	/*
		input: generation of chromosome
		purpose: to select fittest of chromosome 
		output: list of chromosome that made it to next generation 
	*/

	int count =0;
	list<Chromosome>::iterator it; 
	list<Chromosome> newGeneration;
	
	//rank chromosome according to their fitness values in descending order 
	generation.sort(compareChromosomes);
	
	//only select the chromosomes that rank high in terms of quality 
	for(it = generation.begin(); it != generation.end(); it++){
		count+=1;
		newGeneration.push_back(*it);
		if(count >= MAXIMUM_SIZE_OF_GENERATION)
			break;
	}

	return newGeneration;
}	

//genetic algorithm implementation for toolpath sequence generation 
list<string> generateSequence(VolumetricModel model){
	/*
		input: model of the object to be made 
		purpose: to generate the optimal sequence of operation that can carve the given model using 3 axis cnc 
		output: optimal sequence of orientation that can build model using a 3-axis cnc machine 
	*/

	vector<pair<int, string> > volumes(NOOFORIENTATION);
	int i;

	//generating intital generation of chromosome with six possible degrees of rotation 
	for(i=0; i<NOOFORIENTATION; i++){
		volumes[i] = make_pair(model.calculateMachinableVolume(ORIENTATION[i]), ORIENTATION[i]);
	}
	sort(volumes.begin(), volumes.end(), compareVolume );

	list<string> optimalSet;

	for(i=0; i<NOOFORIENTATION; i++){
		optimalSet.push_back(volumes[i].second);
	}

	return optimalSet;

}

//generate toolpath for sequence 
pair<list<string>, list<string> > toolpathGeneratorForSequence(list<string> sequence, VolumetricModel model, int TOOL_DIA, int lMax, int bMax, int hMax, string folderName, bool printVolume){
	

	//iterator to access sequence of orientation 
	list<string>::iterator it;

	list<string> machiningSequence;

	list<string> toolpaths; 

	for(it = sequence.begin(); it != sequence.end(); it++){
					//printing the model for debugging purpose 
			if(printVolume){
				ofstream myfile;
		 		myfile.open ("./" + folderName + "/" + *it + ".interim.raw", ios::binary );
		 		model.print(myfile);
		 		// const char *voxelsStream = voxels.c_str();
		 		// myfile.write(voxelsStream, lMax*hMax*bMax);
			}
			//converting the model to heightmap in a given orientation 
			Matrix heightmap = model.toHeightmap(*it);
			int machinedVolume = model.calculateMachinableVolume(*it);
			if(machinedVolume != 0){
				int i, j;
				machiningSequence.push_back(*it);

				//converting heightmap to graph and regionmap for toolpath generation
				pair<Graph, Matrix> graphRegionmap = toGraph(heightmap);
				Graph graph = graphRegionmap.first;
				Matrix regionmap = graphRegionmap.second;

				//calculating toolpath for the given model and orientation
				
				string toolpath;// = toToolpath(model, *it, graph, regionmap, safeHeight, maxHeight, TOOL_DIA);
				if(*it == "xy+" || *it == "xy-"){
					int safeHeight = hMax +2;
					toolpath = toToolpath(model, *it, graph, regionmap, safeHeight, hMax, TOOL_DIA);
				}
				else if(*it == "yz+" || *it == "yz-"){
					int safeHeight = lMax +2;
					toolpath = toToolpath(model, *it, graph, regionmap, safeHeight, lMax, TOOL_DIA);	
				}
				else if(*it == "xz+" || *it == "xz-"){
					int safeHeight = bMax +2;
					toolpath = toToolpath(model, *it, graph, regionmap, safeHeight, bMax, TOOL_DIA);		
				}	

				toolpaths.push_back(toolpath);

				//filling machined volume so that it would be outta consideration in next orientation
				model.fillMachinableVolume(*it);
			}
		
	}

	return make_pair(machiningSequence, toolpaths);
}




void writeSequenceToFile(string folderName, Chromosome optimalOrganism){
	list<string> sequence = optimalOrganism.sequence;
	list<ll> volumes = optimalOrganism.volumes;
	list<string>::iterator it;
	list<ll>::iterator itt = volumes.begin();
	ofstream myfile;

	myfile.open("./" + folderName + "/sequence.txt");

	for(it = sequence.begin(); it != sequence.end(); it++){
		if(*itt != 0)
			myfile << (*it + "->");
		else
			break;
		itt++;
	}

	myfile.close();
}

int main(int argc, char *argv[]){

	//length, breadth, height of the model along with the corresponding iterator 
	int lMax, bMax, hMax, lIter, bIter, hIter;
	cin>>lMax>>bMax>>hMax; 
	string folderName = static_cast<string>(argv[1]);

	//data model for storing voxels 
	vector<vector<vector<int> > > voxels(lMax, vector<vector<int> >(bMax, vector<int>(hMax, false)));
	char boolVal; 
	int TOOL_DIA = 3;

	//taking voxel input  
	for(hIter = 0; hIter<hMax; hIter++){
		for(lIter = 0; lIter<lMax; lIter++){
			for(bIter = 0; bIter<bMax; bIter++){
				cin>>boolVal;
				if(boolVal == '1')
					voxels[lIter][bIter][hIter] = true;
				else
					voxels[lIter][bIter][hIter] = false;
			}	
		}
	}

	//internal representation of the model 
	VolumetricModel model(voxels, lMax, bMax, hMax);

	//generating sequence using genetic algorithm 
	list<string> sequence = generateSequence(model);
	list<string>::iterator it;

	
	
	//generating toolpath for the optimal sequence thats produced by sequence generator 
	pair<list<string>, list<string> > skk = toolpathGeneratorForSequence(sequence, model, TOOL_DIA,lMax,bMax, hMax,folderName, false);
	list<string> sk = skk.second;
	sequence = skk.first;

	for(it = sequence.begin(); it != sequence.end(); it++)
		cout<< *it<<"\n";
	list<string>::iterator itt;int count = 1;
	itt = sequence.begin();

	//saving toolpath in folder "OrientationOutput"
	
 	for(it = sk.begin(); it != sk.end(); it++){
 		ofstream myfile;
 		myfile.open ("./" + folderName + "/" + *itt + ".gcode" );
 		myfile<<*it;
 		count++;
 		itt++;
 		myfile.close();
 	}

	// // list<string> sk;
	// // sk.push_back("xz+");
	// // cout<< toolpathGeneratorForSequence(sk, model, TOOL_DIA, lMax, bMax, hMax);


	return 0;
}