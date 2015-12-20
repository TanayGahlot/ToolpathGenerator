#include <iostream>
#include "voxelizer.h"

#define BUFF_L 2048

using namespace std;

int main(int argc, char **argv){

	string inputFormat = string(argv[1]);
	string outputFormat = string(argv[2]);
	
	std::transform(inputFormat.begin(), inputFormat.end(), inputFormat.begin(), ::tolower);
	std::transform(outputFormat.begin(), outputFormat.end(), outputFormat.begin(), ::tolower);

	cout<<endl<<"Input Format: "<<inputFormat;
	cout<<endl<<"Output Format: "<<outputFormat;

	if(inputFormat == "file"){
		string voxels;
		if(outputFormat == "json"){
			voxels = getVoxelizedJSONString(argv[3], inputFormat);
		}
		else if(outputFormat == "string"){
			voxels = getVoxelizedString(argv[3], inputFormat);
		}
		
		ofstream myfile (argv[4]);
		if (myfile.is_open()){
    			myfile << voxels;
    			myfile.close();
  		}
  		else cout << "Unable to open file";
	}
	else if(inputFormat == "string"){
		//The input has to be in ASCII encoding though
		char buffer[BUFF_L];
		string stl = "";
		string voxels;

		while(fgets(buffer, BUFF_L, stdin)){
			stl = stl + string(buffer);
			//cout<<"\n"<<sizeof(stl)<<" "<<buffer;
		}

		//Basically converting to c_string to avoid weird errors	
		char *stl_char = (char *)stl.c_str();

		if(outputFormat == "json"){
			voxels = getVoxelizedJSONString(stl_char, inputFormat);
		}
		else if(outputFormat == "string"){
			voxels = getVoxelizedString(stl_char, inputFormat);
		}		

		ofstream myfile (argv[4]);
		if (myfile.is_open()){
    			myfile << voxels;
    			myfile.close();
  		}
  		else cout << "Unable to open file";
	}
		
  	return 0;
	
}
