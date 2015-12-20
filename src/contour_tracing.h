/* 	
	This file deals about the contour generation strategy for Toolpath generation.
	The tool basically traces the boundary of the uncarved region methodically.
	
	Things to be implemented: Contours for 3D, Integrating this file with source.(This looks like a hell of a job right now!). 
	Once merged, also consider replacing vector<vector<int>> with Matrix class in the source. But for that, source needs to be cleaned first to avoid incomatibilities in header files.

	And ya...screw you callItMagic!
	
	Author: Amey Kamat
*/

#include<iostream>

#include<string>
#include<vector>

#include<math.h>
#include<stdio.h>
#include<ctype.h>

#include<thread>
#include<chrono>

#include <deque>
#include <list>
#include <queue>

// The safe height is to be taken from the source eventually. For now this macro works as a stub.
#define SAFE_HEIGHT 20

// Sleep time for visualisation
#define SLEEP_TIME 100

using namespace std;

/******************************************Class Declarations******************************************/

// Class to club x and y co-ordinates of a point for convinience. Consider merging this with equivalent class in the source if such class exists n the future.
class Point{
	public:
		int x, y;
	
		Point(int x1, int y1){
			x = x1; y = y1;
		}
		Point operator=(Point p){
			x = p.x;
			y = p.y;
		}
		Point(){}

		// This constructor was specifically designed to set a point as null.
		Point(void * ptr){
			if(ptr == NULL){
				x = -1; y = -1;
			}
		}
	
		// Checks if the point is null based on abouve constructor
		bool isNull(){
			if(x == -1 && y == -1){
				return true;
			}
			return false;
		}

		// Function for debugging
		void display(){
			cout<<x<<" "<<y;
		}
};

// This class is wrapper for all tool specs. Consider replacing this class with equivalent source class once such class exists.
class ToolSpecs{
	public:
		int toolLength;
		int toolDiameter;
		int spindleDiameter;
		int depthPerPass;
		int safeHeight;
		int feedRate;
		ToolSpecs(int l, int d, int sD, int dpp, int sh, int fr){
			toolLength = l;
			toolDiameter = d;
			spindleDiameter = sD;
			safeHeight = sh;
			depthPerPass = dpp;
			feedRate = fr;
		}
		ToolSpecs(){}
};









/******************************************Lower Utility Functions******************************************/

// Ensures if the point exists to avoid segmentation faults. This can be replaced by equivalent function in the source (if such function exists!)
bool exists(int x, int y, vector<vector<int>> &regionMap){
	if(x>=0 && x<regionMap.size() && y>=0 && y<regionMap[0].size()){
		return true;
	}
	return false;
}


// Utility function to calculate slope. Basically returns an arctan value since real slope value can be infinity practically.
float calculate_slope(Point p1, Point p2){
	return atan(float(p2.y - p1.y)/(p2.x - p1.x));
}


// Function for debugging, also can be used as fancy way to visualise contour process.
void display(vector<vector<int>> &regionMap, vector<vector<int>> &traverseMap){
	for(int i=0; i<regionMap.size(); i++){
		cout<<"\n";
		for(int j=0; j<regionMap[0].size(); j++){
			if(traverseMap[i][j] == 1){			
				cout<<"o ";
			}
			else{
				cout<<regionMap[i][j]<<" ";
			}
		}
	}
	cout<<"\n\n\n";
}


// Gets highest value from height map. Can be replaced with equivalent function from source.
int get_highest_value(vector<vector<int>> &heightMap){
	int high = heightMap[0][0];
	for (int i=0; i<heightMap[0].size(); i++){
		for(int j=0; j<heightMap.size(); j++){
			if(high < heightMap[j][i]){
				high = heightMap[j][i];
			}
		}
	}
	return high;
}


// Gets lowest value from height map. Can be replaced with equivalent function from source.
int get_lowest_value(vector<vector<int>> &heightMap){
	int low = heightMap[0][0];
	for (int i=0; i<heightMap[0].size(); i++){
		for(int j=0; j<heightMap.size(); j++){
			if(low > heightMap[j][i]){
				low = heightMap[j][i];
			}
		}
	}
	return low;
}


// Utility function to generate gcode to clean up the code.
// Currently implements "move": "G0", "cut": "G1" and "raise": "G1 Z"
string write_gcode(string action, int x=0, int y=0, int z=0){
	if(action == "move")
		return "G0 X" + to_string(x) + " Y" + to_string(y) + "\n";
	else if(action == "cut")
		return "G1 X" + to_string(x) + " Y" + to_string(y) + "\n";
	else if(action == "raise")
		return "G1 Z" + to_string(z) + "\n";
	return "";
}





/******************************************Higher Utility Functions******************************************/

// Combines all the checks so higher abstract functions can call assle free single function. Though the arguments are screwed up because of screwed up callItMagic function.
bool check_feasibility(Point P, vector<vector<int>> regionMap, vector<vector<int>> traverseMap){

	if(exists(P.x, P.y, regionMap) && regionMap[P.x][P.y] == 0 && traverseMap[P.x][P.y] == 0){
		return true;
	}
	return false;
}

// This function is set of if else rules to classify locality of point into known scenarios. Iteratively checks all neighbours for existance, region check and traversal to set correct bits in bitMap.
int get_position_scenario(Point p, vector<vector<int>> &regionMap, vector<vector<int>> &traverseMap){

	int currentRegion = regionMap[p.x][p.y];
	int positionScenario = 0;

	Point a(p.x-1, p.y);

	if(!check_feasibility(Point(p.x-1,p.y), regionMap, traverseMap)){
		positionScenario += 8;
	}
	if(!check_feasibility(Point(p.x,p.y-1), regionMap, traverseMap)){
		positionScenario += 4;
	}
	if(!check_feasibility(Point(p.x+1,p.y), regionMap, traverseMap)){
		positionScenario += 2;
	}
	if(!check_feasibility(Point(p.x,p.y+1), regionMap, traverseMap)){
		positionScenario += 1;
	}

	return positionScenario;
}


// Function to debug code by visualisation
void visualise(vector<vector<int>> &regionMap, vector<vector<int>> &traverseMap){
	std::chrono::milliseconds timespan(SLEEP_TIME);
	std::this_thread::sleep_for(timespan);
	display(regionMap, traverseMap);
}





/******************************************Seeding Functions******************************************/


// This function scan the entire region map to find the first point it encounters as a starting point for contour generation algorithm.
Point get_start_point(vector<vector<int>> &regionMap, vector<vector<int>> &traverseMap){
	
	for(int i=0; i<regionMap[0].size(); i++){
		for(int j=0; j<regionMap.size(); j++){
			if(check_feasibility(Point(j,i), regionMap, traverseMap)){
				return Point(j,i);
			}
		}
	}
	return Point(NULL);
}


// Implements BFS. Given a point, this function is capable of returning an un carved point with same region and closest to the seed point.
Point get_seed_point(Point seed_point, vector<vector<int>> &regionMap, vector<vector<int>> &traverseMap){
	
	
	Point currentPoint;
	std::queue<Point> Q;
	Q.push(seed_point);
	//seed_point.display(); cout<<"\n";
	// QMap is a data structure to barr already considered point to be entering again in the queue.
	vector<vector<int>> QMap(regionMap.size(), vector<int>(regionMap[0].size(), 0)); 

	while(!Q.empty()){
		currentPoint = Q.front();
		Q.pop();
		if(traverseMap[currentPoint.x][currentPoint.y] != 1){
			return currentPoint;
		}
		for(int i=-1; i<=1; i++)
			for(int j=-1;j<=1;j++){
				if(i != 0 || j != 0){
					if(exists(currentPoint.x+i, currentPoint.y+j, regionMap) 
								&& regionMap[currentPoint.x+i][currentPoint.y+j] == 0
								&& QMap[currentPoint.x+i][currentPoint.y+j] != 1){
							Q.push(Point(currentPoint.x+i, currentPoint.y+j));
							QMap[currentPoint.x+i][currentPoint.y+j] = 1;
					}

				}
			}
	}
	return Point(NULL);
}


/******************************************Lookup Functions******************************************/

// This function handles complexity of special case. It resolves a bug in which a point is surrounded by boundaries on all sides and hence tool cannot move further. It sets certain rules in the locality of point in such cases to know in which case it fits. Then decides on the next point to be considered. Does not tell if the point exists or not.
Point escape_from_closed_region(Point p, vector<vector<int>> &regionMap, vector<vector<int>> &traverseMap){
	
	int currentRegion = regionMap[p.x][p.y];
	int closedScenario=0;

	if(!check_feasibility(Point(p.x-1,p.y), regionMap, traverseMap)){
		closedScenario += 8;
	}
	if(!check_feasibility(Point(p.x,p.y-1), regionMap, traverseMap)){
		closedScenario += 4;
	}
	if(!check_feasibility(Point(p.x+1,p.y), regionMap, traverseMap)){
		closedScenario += 2;
	}
	if(!check_feasibility(Point(p.x,p.y+1), regionMap, traverseMap)){
		closedScenario += 1;
	}

	switch(closedScenario){
		case  7: return Point(p.x-1,p.y+1);
		case 11: return Point(p.x-1,p.y-1);
		case 13: return Point(p.x+1,p.y-1);
		case 14: return Point(p.x+1,p.y+1);	
		default: return Point(NULL);
	}
}



// Implements a lookup table. gets bitMap and looks into table for next points to be considered around the current point. If no such point exists, returns null point.
Point get_next_point(Point p, Point prev_p, vector<vector<int>> &regionMap, vector<vector<int>> &traverseMap){
	
	int currentRegion = regionMap[p.x][p.y];
	int positionScenario = get_position_scenario(p, regionMap, traverseMap);
	int x1, x2, y1, y2;

	Point escape_point;
	
	switch(positionScenario){
		case  0: return Point(NULL); break;
		case  1: x1 = -1; y1 = +1; x2 = -1; y2 =  0; break;
		case  2: x1 = +1; y1 = +1; x2 =  0; y2 = +1; break;
		case  3: x1 = -1; y1 = +1; x2 = -1; y2 =  0; break;
		case  4: x1 = +1; y1 = -1; x2 = +1; y2 =  0; break;
		case  5: x1 = p.x-prev_p.x; y1 = prev_p.x-p.x; x2 = p.x-prev_p.x; y2 = 0; break;
		case  6: x1 = +1; y1 = +1; x2 =  0; y2 = +1; break;
		case  7: x1 = -1; y1 = +1; x2 = -1; y2 =  0; break;
		case  8: x1 = -1; y1 = -1; x2 =  0; y2 = -1; break;
		case  9: x1 = -1; y1 = -1; x2 =  0; y2 = -1; break;
		case 10: x1 = prev_p.y-p.y; y1 = p.y-prev_p.y; x2 = 0; y2 = p.y-prev_p.y;; break;
		case 11: x1 = -1; y1 = -1; x2 =  0; y2 = -1; break;
		case 12: x1 = +1; y1 = -1; x2 = +1; y2 =  0; break;
		case 13: x1 = +1; y1 = -1; x2 = +1; y2 =  0; break;
		case 14: x1 = +1; y1 = +1; x2 =  0; y2 = +1; break;
		case 15: escape_point = escape_from_closed_region(p, regionMap, traverseMap); 
			 if(check_feasibility(escape_point, regionMap, traverseMap))
			 	return escape_point;
			 else{
				return Point(NULL);
			 }
			 break;
		default: return Point(NULL); break;
	}
	
	if(check_feasibility(Point(p.x+x1,p.y+y1), regionMap, traverseMap)){
		return Point(p.x+x1, p.y+y1);
	}
	else if(check_feasibility(Point(p.x+x2,p.y+y2), regionMap, traverseMap)){
		return Point(p.x+x2, p.y+y2);
	}
	else{
		return Point(NULL);
	}
}


/******************************************Main Algorithm Logic******************************************/

// Main function where the body of algorithm lies.
string generate_2D_contours(vector<vector<int>> &regionMap, ToolSpecs &tool, int depth){

	Point new_point, prev_point;
	float slope, prev_slope;
	string gcode = "";

	vector<vector<int>> traverseMap(regionMap.size(), vector<int>(regionMap[0].size(), 0));
	
	// This is done in case required in future. This loop goes until sequence of regions is exhausted. Currently only takes matrix of 0s ad 1s so this loop only runs once.
	
	Point st_point = get_start_point(regionMap, traverseMap);
	Point point = st_point;

	// Reson behind this is simple. It is assumed that tool will move in -> direction initially hence because of the way we are scanning the region map for the first point, the prev_point can only be the point exactly behind the current point. If no such point exists, prev_point equals current point.
	if(exists(point.x-1, point.y, regionMap))		
		prev_point = Point(point.x-1, point.y);
	else
		prev_point = point;

	// code for gcode generation
	gcode = gcode + write_gcode("raise",0,0,tool.safeHeight);
	gcode = gcode + write_gcode("move",point.x,point.y,0);
	gcode = gcode + write_gcode("raise",0,0,depth);

	// This only happens when no point remains to be carved in the region.
	while(!st_point.isNull()){
		//mark the point as traversed.
		traverseMap[point.x][point.y] = 1;
			
		
			// Uncomment this for contour visualisation
			//visualise(regionMap, traverseMap);
		

		// Fetch new point based on known facts
		new_point = get_next_point(point, prev_point, regionMap, traverseMap);

		// No point remains around the point to be carved
		if(new_point.isNull()){

			// code for gcode generation
			gcode = gcode + write_gcode("cut",point.x,point.y,0);

			// Get new seed point and get going!
			st_point = get_seed_point(point, regionMap, traverseMap);
			prev_point = point;
			point = st_point;
			continue;
		}
		else{
			prev_slope = slope;
			slope = calculate_slope(point, new_point);
		
			//Until slope remains same, do not write gcode
			if(prev_slope == slope){
				prev_point = point;
				point = new_point;
			}
			else{
				//code for gcode generation
				gcode = gcode + write_gcode("cut",point.x,point.y,0);
				st_point = new_point;
				prev_point =point;
				point = new_point;

					
				new_point = Point(NULL);
			}
		}
	}
	return gcode;
	
}

/******************************************Highest Abstraction Functions******************************************/

vector<vector<int>> get_modified_regionMap(VolumetricModel &model, string orientation, vector<vector<int>> regionMap, BoolDict isInList, ToolSpecs &tool, int depth, int delta){

	vector<vector<int>> modifiedRegionMap(regionMap.size(), vector<int>(regionMap[0].size(), 0));
	for(int i=0;i<regionMap[0].size();i++){
		for(int j=0; j<regionMap.size(); j++){
			//cout<<"\nPoint"<<j<<" "<<i<<" "<<depth;
			if(isInList[regionMap[j][i]] && callItMagic(model, j, i, depth, orientation, tool.safeHeight, delta)){
				modifiedRegionMap[j][i] = 0;
			}
			else{
				modifiedRegionMap[j][i] = 1;
			}
		}

	}
	
	return modifiedRegionMap;
}

// Generates 3D contours iterating over certain values of heightMap.
string get_3D_contours(VolumetricModel &model, string orientation, vector<vector<int>> &regionMap, BoolDict &isInList, ToolSpecs &tool, pair<int, int> height_dimensions){

	int hMax = height_dimensions.first;
	int hMin = height_dimensions.second;

	int delta = tool.safeHeight - hMax;

	vector<vector<int>> modifiedRegionMap(regionMap.size(), vector<int>(regionMap[0].size(), 0));
	
	string gcode = "";
	
	for(int h = hMax; h>hMin; h--){
		// raise the tool to height
		
		//cout<<"\n"<<hMin<<" "<<hMax<<" "<<h;
		
		write_gcode("raise", 0, 0, h-hMax);
		//To be implemeted: generate modified regionMap based on isInList and ether map.
		modifiedRegionMap = get_modified_regionMap(model, orientation, regionMap, isInList, tool, hMax-h, delta);
		
		// Later this function also has to consider the ether, so one more arguement expected
		gcode = gcode + generate_2D_contours(modifiedRegionMap, tool, h-hMax);
	}
	return gcode;
}


// This function has been written to make this file compatible with source. Once source is cleaned and restructured, this function would stand obselete.
string generate_toolpath_with_compatibility(VolumetricModel &model, string orientation, AdjList vlist, Matrix regionMap, int depth, int noOfRegion, int regionCurrentHeight, int safeHeight, int maxHeight, vector<vector<int>>heightMap, int TOOL_DIA, int depthPerPass){
	
	AdjList::iterator it;
	BoolDict isInList;
	for(int i=1; i<= noOfRegion; i++){
		it = find(vlist.begin(), vlist.end(), i);
		if(it != vlist.end())
			isInList[i] = true;
		else
			isInList[i] = false;

		cout<<isInList[i]<<" ";
	}
	cout<<" depth: "<<depth<<" regionCurrentHeight: "<<regionCurrentHeight<<" maxHeight: "<<maxHeight<<"\n\n";

	ToolSpecs tool(0,TOOL_DIA,0,depthPerPass,0,0);

	int hMax = get_highest_value(heightMap);
	int hMin = get_lowest_value(heightMap);

	//cout<<hMax<<" "<<hMin<<"\n\n";
	
	return get_3D_contours(model, orientation, regionMap, isInList, tool, make_pair(maxHeight, regionCurrentHeight - depth));

	return "";

	
}

/*
int main(){
	
	int xDim, yDim;
	int noOfRegions;
	string gcode;
	
	cin>>xDim>>yDim>>noOfRegions;

	vector<vector<int>> HeightMap(xDim, vector<int>(yDim, 0));
	vector<vector<int>> traversalMap(xDim, vector<int>(yDim, 0));
	vector<int> sequence(noOfRegions, 0);
	
	for(int i=0; i<noOfRegions; i++){
		cin>>sequence[i];
	}

	for(int i=0; i<yDim; i++){
		for(int j=0; j<xDim; j++){
			cin>>HeightMap[j][i];
		}
	}
	
	generate_modified_regionMap
	
	gcode = generate_contours(regionMap, traversalMap, sequence);
	cout<<gcode;
	
	
	
	return 0;
}
*/
