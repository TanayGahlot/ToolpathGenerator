/* 	
	This file deals about the contour generation strategy for Toolpath generation.
	The tool basically traces the boundary of the uncarved region methodically.
	
	Things to be implemented: 
		[x] Contours for 3D
		[x] Integrating this file with source.(This looks like a hell of a job right now!).
		[x] Clean the code
		[x] Rerouting tool for efficiency in cutting.
		[x] Bring callItMagic to its knees!!!!
		[ ]Once merged, also consider replacing vector<vector<int>> with Matrix class in the source. But for that, source needs to be cleaned first to avoid incomatibilities in header files.

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

// Sleep time for visualisation
#define SLEEP_TIME 10

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

		//Checks if given point is neighbour
		bool isNeighbour(Point p){
			if(p.x-x >=-1 && p.x-x <=1 && p.y-y >=-1 && p.y-y <=1){
				return true;
			}
			else
				return false;
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
bool exists(int x, int y, vector<vector<int> > &regionMap){
	if(x>=0 && x<regionMap.size() && y>=0 && y<regionMap[0].size()){
		return true;
	}
	return false;
}


// Utility function to calculate slope. Basically returns an arctan value since real slope value can be infinity practically. Also will help if gradiet approach is thought of in future.
float calculate_slope(Point p1, Point p2){
	return atan(float(p2.y - p1.y)/(p2.x - p1.x));
}


// Function for debugging, also can be used as fancy way to visualise contour process.
void display(vector<vector<int> > &regionMap, vector<vector<int> > &traverseMap){
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
int get_highest_value(vector<vector<int> > &heightMap){
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
int get_lowest_value(vector<vector<int> > &heightMap){
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
	else if(action == "move around x y")
		return "G0 X" + to_string(x) + "\n" + "G0 Y" + to_string(y) + "\n";
	else if(action == "move around y x")
		return "G0 Y" + to_string(y) + "\n" + "G0 X" + to_string(x) + "\n";
	else if(action == "move raised")
		return "G1 Z" + to_string(z) + "\n" + "G0 X" + to_string(x) + " Y" + to_string(y) + "\n";
	return "";
}





/******************************************Higher Utility Functions******************************************/

// Combines all the checks so higher abstract functions can call hassle free single function.
bool check_feasibility(Point P, vector<vector<int> > regionMap, vector<vector<int> > traverseMap){

	if(exists(P.x, P.y, regionMap) && regionMap[P.x][P.y] == 0 && traverseMap[P.x][P.y] == 0){
		return true;
	}
	return false;
}

// This function is set of if else rules to classify locality of point into known scenarios. Iteratively checks all neighbours for existance, region check and traversal to set correct bits in bitMap.
int get_position_scenario(Point p, vector<vector<int> > &regionMap, vector<vector<int> > &traverseMap){

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
void visualise(vector<vector<int> > &regionMap, vector<vector<int> > &traverseMap){
	std::chrono::milliseconds timespan(SLEEP_TIME);
	std::this_thread::sleep_for(timespan);
	display(regionMap, traverseMap);
}





/******************************************Seeding Functions******************************************/

// Implements BFS. Given a point, this function is capable of returning an un carved point with same region and closest to the seed point.
Point get_seed_point(Point seed_point, vector<vector<int> > &regionMap, vector<vector<int> > &traverseMap){
	
	
	Point currentPoint;
	std::queue<Point> Q;

	
	// QMap is a data structure to barr already considered point to be entering again in the queue.
	vector<vector<int> > QMap(regionMap.size(), vector<int>(regionMap[0].size(), 0)); 
	if(exists(seed_point.x, seed_point.y, regionMap))
		QMap[seed_point.x][seed_point.y] = 1;
	else{
		seed_point = Point(0,0);
		QMap[seed_point.x][seed_point.y] = 1;		
	}
	seed_point.display(); cout<<"<<<\n";
	Q.push(seed_point);

	while(!Q.empty()){
		currentPoint = Q.front();
		Q.pop();
		// The second condition was incorporated for rare case of regions divided into various chunks on region map.
		if(check_feasibility(currentPoint, regionMap, traverseMap)){
			//cout<<">>"; currentPoint.display(); cout<<"\n";
			return currentPoint;
		}
		for(int i=-1; i<=1; i++)
			for(int j=-1;j<=1;j++){
				if(i != 0 || j != 0){
					if(exists(currentPoint.x+i, currentPoint.y+j, regionMap) 
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
// [ ] Needs to also incorporate what happens if point is surrounded by traversed point. For now tool just relocates itself by means of get_seed_point function
Point escape_from_closed_region(Point p, vector<vector<int> > &regionMap, vector<vector<int> > &traverseMap){
	
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
Point get_next_point(Point p, Point prev_p, vector<vector<int> > &regionMap, vector<vector<int> > &traverseMap){
	
	int currentRegion = regionMap[p.x][p.y];
	int positionScenario = get_position_scenario(p, regionMap, traverseMap);
	int x1, x2, y1, y2;

	Point escape_point;
	
	switch(positionScenario){
		case  0: x1 = -1; y1 = +1; x2 = -1; y2 =  0; break;
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

//This function decides on the path to take in case of disjoint movement
pair<string, int> get_relevent_path(Point start, Point end, vector<vector<int> > &regionMap, vector<vector<int> > &heightMap){
	int releventPathFlag = 1;
	int height;	
	
	if(start.x == end.x && start.y == end.y){
		// See if it is same fucking point and rejoice!!!
		return make_pair("",0);
	}
	/*else{
		// Use DDA algorithm
		float currentX, currentY;
		int dx = end.x - start.x;
		int dy = end.y - start.y;
		int steps = (abs(dx)>abs(dy)?abs(dx):abs(dy));
		currentX = start.x; currentY = start.y;
		height = heightMap[currentX][currentY];
		float xIncr = dx/(float)steps;
		float yIncr = dy/(float)steps;

		for(int i=0; i<heightMap.size(); i++){
			cout<<"\n";
			for(int j=0; j<heightMap[0].size(); j++){
					cout<<heightMap[i][j]<<" ";
			}
		}
		cout<<"\n\n\n";

		for(int currentStep = 0; currentStep < steps; currentStep++)
		{
			currentX += xIncr;
			currentY += yIncr;
			if(regionMap[currentX][currentY] == 1){
				if(height < heightMap[currentX][currentY]){
					height = heightMap[currentX][currentY];
					cout<<height<<" "<<currentX<<currentY<<"Dead may rise\n";
				}
				releventPathFlag = 0;
			}
		}
		if(releventPathFlag == 1){
			return make_pair("move",0);
		}

	}
	
	*/
	/*
	releventPathFactor = 1;
	
	for(currentY = start.y, currentX = start.x; currentY != end.y; currentY += signFactorY){
		if(regionMap[currentX][currentY] == 1){
			releventPathFlag = 0;
			break;
		}
	}

	for(currentX = start.x; currentX != end.x && releventPathFactor != 0; currentX += signFactorX){
		if(regionMap[currentX][currentY] == 1){
			releventPathFlag = 0;
			break;
		}
	}

	if(releventFactor == 1){
		return "move around y z"
	}
	*/
	return make_pair("move raised",0);
}


/******************************************Main Algorithm Logic******************************************/

// Main function where the body of algorithm lies.
pair<string, Point> generate_2D_contours(vector<vector<int> > &regionMap, ToolSpecs &tool, vector<vector<int> > &heightMap, Point seed_point, int depth){

	Point new_point, prev_point;
	float slope, prev_slope;
	string gcode = "";
	pair<string, int> pathAndHeight;

	vector<vector<int> > traverseMap(regionMap.size(), vector<int>(regionMap[0].size(), 0));

	Point st_point = get_seed_point(seed_point, regionMap, traverseMap);
	Point point = st_point;

	// Reson behind this is simple. It is assumed that tool will move in -> direction initially hence because of the way we are scanning the region map for the first point, the prev_point can only be the point exactly behind the current point. If no such point exists, prev_point equals current point.
	if(exists(point.x-1, point.y, regionMap))		
		prev_point = Point(point.x-1, point.y);
	else
		prev_point = point;

	if(!exists(seed_point.x, seed_point.y, regionMap)){
		seed_point = new Point(0,0);
	}

	// code for gcode generation only if point exists to start with in first place. otherwise tool just fools around randomly at constant x y positions at different depths
	if(!st_point.isNull()){
		
		pathAndHeight = get_relevent_path(seed_point,point,regionMap,heightMap);
		gcode = gcode + write_gcode(pathAndHeight.first, point.x, point.y,  0);
		gcode = gcode + write_gcode("raise",0,0,depth);
	}

	// This only happens when no point remains to be carved in the region.
	while(!st_point.isNull()){
		//mark the point as traversed.
		traverseMap[point.x][point.y] = 1;

		if(!point.isNeighbour(prev_point)){
			pathAndHeight = get_relevent_path(prev_point,point,regionMap,heightMap);
			gcode = gcode + write_gcode(pathAndHeight.first,point.x,point.y,0);
			gcode = gcode + write_gcode("raise",0,0,depth);
		}
			
		
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
	//prev_point.display(); cout<<"\n";
	return make_pair(gcode,prev_point);
	
}

/******************************************Highest Abstraction Functions******************************************/

// Generates modified height map by considering current position in region graph, ether(#callItMagic) and regionMap. One of the biggest inefficiencies in code. Needs to be taken care in future.
vector<vector<int> > get_modified_regionMap(VolumetricModel &model, string orientation, vector<vector<int> > regionMap, BoolDict isInList, ToolSpecs &tool, int depth, int delta){

	vector<vector<int> > modifiedRegionMap(regionMap.size(), vector<int>(regionMap[0].size(), 0));
	for(int i=0;i<regionMap[0].size();i++){
		for(int j=0; j<regionMap.size(); j++){
			if(isInList[regionMap[j][i]] && callItMagic(model, j, i, depth, orientation, tool.safeHeight, delta)){
				modifiedRegionMap[j][i] = 0;
			}
			else{
				modifiedRegionMap[j][i] = 1;
			}
		}

	}

	for(int i=0; i<modifiedRegionMap.size(); i++){
		cout<<"\n";
		for(int j=0; j<modifiedRegionMap[0].size(); j++){
				cout<<modifiedRegionMap[i][j]<<" ";
		}
	}
	cout<<"\n\n\n";
	
	return modifiedRegionMap;
}

// Generates 3D contours iterating over certain values of heightMap.
string get_3D_contours(VolumetricModel &model, string orientation, vector<vector<int> > &regionMap, vector<vector<int> > &heightMap, BoolDict &isInList, ToolSpecs &tool, pair<int, int> height_dimensions){

	int hRegionMax = height_dimensions.first;
	int hRegionMin = height_dimensions.second;
	static Point seed_point = Point(0,0);;
	static string last_orientation = "";
	
	if(last_orientation != orientation){
		seed_point = Point(0,0);
	}
	
	last_orientation = orientation;

	int delta = 0;

	vector<vector<int> > modifiedRegionMap(regionMap.size(), vector<int>(regionMap[0].size(), 0));
	
	pair<string, Point> contourOutput;
	string gcode = "";
	
	for(int h = hRegionMax; h>hRegionMin; h-=tool.depthPerPass){

		//Generates modified regionMap based on isInList and ether map.
		modifiedRegionMap = get_modified_regionMap(model, orientation, regionMap, isInList, tool, tool.safeHeight-h, delta);
		
		contourOutput = generate_2D_contours(modifiedRegionMap, tool, heightMap, seed_point, h - tool.safeHeight);
		gcode += contourOutput.first;
		seed_point = contourOutput.second;
	}
	return gcode;
}


// This function has been written to make this file compatible with source. Once source is cleaned and restructured, this function would stand obselete.
string generate_toolpath_with_compatibility(VolumetricModel &model, string orientation, AdjList vlist, Matrix regionMap, int depth, int noOfRegion, int regionCurrentHeight, int safeHeight, int maxHeight, vector<vector<int> >heightMap, int TOOL_DIA, int depthPerPass){
	
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
	
	cout<<" depth: "<<depth<<" regionCurrentHeight: "<<regionCurrentHeight<<" maxHeight: "<<maxHeight<<"orientation: "<<orientation<<"\n\n";

	ToolSpecs tool(0,TOOL_DIA,0,depthPerPass,maxHeight,0);

	//int hMax = get_highest_value(heightMap);
	//int hMin = get_lowest_value(heightMap);
	
	return get_3D_contours(model, orientation, regionMap, heightMap, isInList, tool, make_pair(regionCurrentHeight, regionCurrentHeight - depth));

}


// Main Function used for debugging
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
