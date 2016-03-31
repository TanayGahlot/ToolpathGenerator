//oggpnosn 
//hkhr 

//heightmap to toolpath generator
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std;


int calculateMaxZ(HeightMap heightMap){
		int globalMaximum=0, localMaximum=0;		
		for(int i=0; i<heightMap.second.first; i++){
			localMaximum = *max_element(heightMap.first[i].begin(), heightMap.first[i].end());
			if(localMaximum > globalMaximum){
				globalMaximum = localMaximum;
			}  
		}
		return globalMaximum;
}

list<Point_2D> findNeighbour(int a, int b, int N, int M){
	list<Point_2D> neighbour;

	if(a+1<N){
		neighbour.push_back(make_pair(a+1, b));
	}
	if(a-1>=0){
		neighbour.push_back(make_pair(a-1, b));	
	}
	if(b-1>=0){
		neighbour.push_back(make_pair(a, b-1));	
	}
	if(b+1 < M){
		neighbour.push_back(make_pair(a, b+1));	
	}
	return neighbour;
}

void insertEdge(Graph &graph, int r1, int r2, int h1, int h2){
	try{
		AdjList iter = graph.at(r1).second;
		AdjList::iterator it = find(iter.begin(), iter.end(), r2);
		if(it == iter.end()){
			//add r1, r2
			graph[r1].second.push_back(r2);
			insertEdge(graph, r2, r1, h2, h1);
		}
	}
	catch(out_of_range){
		AdjList *adj = new AdjList();
		adj->push_back(r2);
		graph[r1] = make_pair(h1, *adj);
		insertEdge(graph, r2, r1, h2, h1);
	}

}

Graph toGraph(HeightMap heightMap, ToolConfig toolConfig, RegionMap &regionMap){
	
	/* Initiating regionMap */
	regionMap.first = Matrix(heightMap.second.first, vector<int >(heightMap.second.second, 0));
	regionMap.second.first = heightMap.second.first;
	regionMap.second.second = heightMap.second.second;
	
	
	
	Graph graph;
	Stack globalStack, localStack;
	int regionNumber = 0;
	
	/* Keep this for future reference */
	//heightmap = makeHeightmapMachinable(heightmap, toolRadius, toolLength);
	
	globalStack.push(make_pair(0, 0));

	while(!globalStack.empty()){
		Point_2D currentPoint = globalStack.top(); 
		globalStack.pop();
		
		if(regionMap.first[currentPoint.first][currentPoint.second] == 0){
			regionNumber += 1;
			localStack = Stack();
			localStack.push(make_pair(currentPoint.first, currentPoint.second));

			while(!localStack.empty()){
				currentPoint = localStack.top(); 
				localStack.pop();
				
				if(regionMap.first[currentPoint.first][currentPoint.second] == 0){
					regionMap.first[currentPoint.first][currentPoint.second] = regionNumber;

					list<Point_2D> neighbour = findNeighbour(currentPoint.first, currentPoint.second, regionMap.second.first, regionMap.second.second);
					
					for(list<Point_2D>::iterator it = neighbour.begin(); it != neighbour.end(); it++){
						Point_2D currentNeighbour = (*it);

						if(heightMap.first[currentPoint.first][currentPoint.second] == heightMap.first[currentNeighbour.first][currentNeighbour.second]){
							if(regionMap.first[currentPoint.first][currentPoint.second] == 0){
								localStack.push(make_pair(currentPoint.first, currentPoint.second));
							}
						}
						else{
							if(regionMap.first[currentNeighbour.first][currentNeighbour.second] == 0){
								globalStack.push(make_pair(currentNeighbour.first, currentNeighbour.second));
							}
							else{
								insertEdge(graph, regionMap.first[currentNeighbour.first][currentNeighbour.second], regionMap.first[currentPoint.first][currentPoint.second], heightMap.first[currentNeighbour.first][currentNeighbour.second], heightMap.first[currentPoint.first][currentPoint.second]);
							}
						}
					}
				}
			}
		}
	}
	for(int i=0; i<regionMap.second.first; i++){
		cout<<"\n";
		for(int j=0; j<regionMap.second.second; j++)
			cout<<regionMap.first[i][j]<<" ";
	}
	
	return graph;
}

//it converts heightmap to depthmap by subtracting maximum height with all the heights 
void toDepthGraph(Graph &graph, int maxHeight){
	
	/* This loop basically converts height to depth and then smokes weed */
	for(Graph::iterator it = graph.begin(); it != graph.end(); it++){
		((it->second).first) = maxHeight - ((it->second).first);
	}

}

//picks the minimum depth node 
int pickMin(Graph &graph){
	Graph::iterator it = graph.begin();
	int u = it->first;
	int minElement = (it->second).first;
	for(it = graph.begin(); it != graph.end(); it++){
		if((it->second).first < minElement){
			minElement = (it->second).first;
			u = it->first;
		}
	}
	return u;
}

//finds all the nodes connected to "u" 
AdjList connected(Graph &graph, int u){
	IntStack stack;
	AdjList vlist;
	BoolDict explored;
	AdjList::iterator neighbour;
	Graph::iterator it;
	int ui;

	stack.push(u);
	
	for(it = graph.begin(); it != graph.end(); it++){
		explored[it->first] = false;
	}
	
	while(stack.size() != 0){
		ui = stack.top(); stack.pop();
		if(!explored[ui]){
			vlist.push_back(ui);
			explored[ui] = true;
			for(neighbour = graph[ui].second.begin(); neighbour != graph[ui].second.end(); neighbour++){
				stack.push(*neighbour);
			}
		}		
	}
	return vlist;
}

//writes gcode by taking path position 
void writeGcode(bool &cutting, BoolDict &isInList, Matrix &regionmap, string &toolpath, int safeHeight, int regionCurrentHeight, int i , int j, int dep, int iprev, int jprev, int delta){
	if(cutting){
		if(isInList[regionmap[i][j]]){
			//toolpath += ("G1 X" + to_string(i) + " Y" + to_string(j) +"\n"); 
		}
		else{
			toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
			toolpath += ("G1 Z2\n");
			toolpath += ("G0 ");
			cutting = false; 	
		}
	}
	else{
		if(isInList[regionmap[i][j]]){
			toolpath += ( "X" + to_string(i) + " Y" + to_string(j) +"\n"); 
			toolpath += ("G1 Z" + to_string(regionCurrentHeight-dep-safeHeight) + "\n");
			cutting = true;
		}
	}
}

// a  hack to optimize code 
void endCut(bool &cutting, BoolDict &isInList, Matrix &regionmap, string &toolpath, int safeHeight, int regionCurrentHeight, int i , int j, int dep, int iprev, int jprev){
	toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
	//cout<<"iprev: "<<iprev<<", jprev: "<<jprev<<"i: "<<i<<", j= "<<j<<"\n";
	toolpath += ("G1 Z2\n");
	toolpath += ("G0 ");
}

//call it magic...
bool callItMagic(VolumetricModel &model, int i, int j, int dep, string orientation, int safeHeight, int delta){
	//cout<<i<<", "<<j<<","<<dep<<"\n";
	if(orientation ==  "xy+"){
		if(model.model[i][j][safeHeight-delta-dep-1] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xy-"){
		if(model.model[i][j][dep] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xz+"){
		if(model.model[i][safeHeight-delta-dep-1][j] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xz-"){
		if(model.model[i][dep][j] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "yz+"){

		if(model.model[safeHeight-delta-dep-1][i][j] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "yz-"){
		if(model.model[dep][i][j] == 2)
			return false;
		else
			return true;
	}
}

//#include "contour_tracing.h"

//machines connected regions 
string machine(VolumetricModel &model, string orientation, AdjList vlist, Matrix regionmap, int depth, int noOfRegion, int regionCurrentHeight, int safeHeight, int maxHeight, int TOOL_DIA, int depthPerPass){
	int dep, i, j, iprev=0, jprev=0,  m = regionmap.size(), n = regionmap[0].size();
	string toolpath = "G0 ";
	int delta = safeHeight - maxHeight;
	bool cutting;
	BoolDict isInList;
	AdjList::iterator it;

	for(i=1; i<= noOfRegion; i++){
		it = find(vlist.begin(), vlist.end(), i);
		if(it != vlist.end())
			isInList[i] = true;
		else
			isInList[i] = false;
	}

	
	for(dep = 0; dep<depth; dep+=depthPerPass){
		//toolpath += "G1 Z" + to_string(safeHeight-dep) + "\n";
		
		if(dep%2 == 0){
			for(i=0; i<m; i+=TOOL_DIA){
				cutting = false;
				if((i/TOOL_DIA)%2 == 0){
					for(j=0; j<n; j++){
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta)){
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
							iprev = i;jprev =j;
						}
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2 \n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}	
					}	
				}
				else{
					for(j=n-1; j>=0; j--){
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta)){
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
							iprev = i;jprev =j;
						}
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2\n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}
						
					}
					
				}
				if(cutting)
					endCut(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev);
			}
		}
		else{
			for(i=m-1; i>=0; i-= TOOL_DIA){
				cutting = false;
				if((i/TOOL_DIA)%2 == 0){
					for(j=0; j<n; j++){
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta)){
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
							iprev = i;jprev =j;
						}
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2\n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}
					}
					
				}
				else{
					for(j=n-1; j>=0; j--){
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta)){
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
							iprev = i;jprev =j;	
						}
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2\n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}
					}
					
				}
				if(cutting)
					endCut(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev);
			}
		}
	}

	//quick fix
	int length = toolpath.size();
	if(toolpath.substr(length-3, 3) == "G0 ")
		toolpath = toolpath.substr(0, length-3);
	toolpath += "G1 Z2\n"; 
	return toolpath;
}

void displayGraph(Graph &graph){
	Graph::iterator it;
	for(it = graph.begin(); it != graph.end(); it++){
		cout<< it->first<<": ";
	
		AdjList::iterator itt;
		for(itt = (it->second).second.begin(); itt!= (it->second).second.end(); itt++)
			cout<<*itt<<" ";
		cout<<"\n";
	}
}

//it updates the graph depth info after machining 
void processGraph(Graph &graph , AdjList &vlist, int depth){
	AdjList::iterator it; 
	AdjList::iterator itt, neighbour, toDelete;
	AdjList zeroList;

	for(it = vlist.begin(); it!= vlist.end(); it ++){
		graph[*it].first = graph[*it].first - depth; 
		if(graph[*it].first == 0){
			zeroList.push_back(*it);
		}
	}

	for(itt = zeroList.begin(); itt!= zeroList.end(); itt++){
		for(neighbour = graph[*itt].second.begin(); neighbour != graph[*itt].second.end(); neighbour++){
			toDelete = find(graph[*neighbour].second.begin(), graph[*neighbour].second.end(), *itt);
			graph[*neighbour].second.erase(toDelete);
		}
		graph.erase(*itt);
	}
	
}

//converts graph to toolpath using connected component and zigzag path generation strategy
string toToolpath(VolumetricModel &model, ToolConfig toolConfig, Orientation orientation, Graph &graph, RegionMap &regionMap, int maxHeight, HeightMap heightMap){
	
	string toolpath = "G21\nG90\nF" + to_string(toolConfig.feedRate) + "\n" ;
	int u;
	
	int noOfRegion = graph.size();
	int currentHeight = maxHeight;
	
	IntMap regionCurrentHeights;
	for(int i=1; i<=noOfRegion; i++){
		regionCurrentHeights[i] = maxHeight;
	}

	toDepthGraph(graph, maxHeight);

	toolpath += "G1 Z2\n";
	while(graph.size() != 0){
		u = pickMin(graph);

		AdjList vlist = connected(graph, u);
		//toolpath += generate_toolpath_with_compatibility(model, orientation, vlist, regionMap.first, graph[u].first, noOfRegion, regionCurrentHeights[u], toolConfig.safeHeight,  maxHeight, heightMap.first, toolConfig.toolDiameter, toolConfig.stepSize);
		toolpath += machine(model, orientation, vlist, regionMap.first, graph[u].first, noOfRegion, regionCurrentHeights[u], toolConfig.safeHeight,  maxHeight, toolConfig.toolDiameter, toolConfig.stepSize);

		AdjList::iterator it;
		for(it = vlist.begin(); it!= vlist.end(); it++){
			regionCurrentHeights[*it] -= graph[u].first;
		}

		processGraph(graph, vlist, graph[u].first); 
		
	}	
	return toolpath;
}


//generate toolpath for sequence 
pair<list<string>, list<string> > planOperation(VolumetricModel &model, ToolConfig toolConfig, list<Orientation> tentativeSequence, string folderName, bool printVolume){
	
	int xmax = model.xmax, ymax = model.ymax, zmax = model.zmax;


	list<Orientation> trueSequence;

	list<string> toolpathSet;

	int machinedVolume=0; 
	
	for(list<Orientation>::iterator it = tentativeSequence.begin(); it != tentativeSequence.end(); it++){
		
		Orientation currentOrientation = (*it);
		int machinableVolume = model.calculateMachinableVolume(currentOrientation);
		int machinedVolumeInOrientation = 0;
		
		if(machinableVolume != 0){
			
			/* Generate HeightMap */
			HeightMap heightMap = model.toHeightmap(currentOrientation);
		
			int i, j;
			
			RegionMap regionMap;
			
			Graph graph = toGraph(heightMap, toolConfig, regionMap);
			
			string toolpath;
			
			if(*it == "xy+" || *it == "xy-"){
				toolConfig.safeHeight = model.zmax +2;
				toolpath = toToolpath(model, toolConfig, currentOrientation, graph, regionMap, model.zmax + 1, heightMap);
			}
			else if(*it == "yz+" || *it == "yz-"){
				toolConfig.safeHeight = model.xmax +2;
				toolpath = toToolpath(model, toolConfig, currentOrientation, graph, regionMap, model.xmax + 1, heightMap);	
			}
			else if(*it == "xz+" || *it == "xz-"){
				toolConfig.safeHeight = model.ymax +2;
				toolpath = toToolpath(model, toolConfig, currentOrientation, graph, regionMap, model.ymax + 1, heightMap);
			}	
			
			toolpathSet.push_back(toolpath);

			/* Filling machined volume so that it would be outta consideration in next orientation */
			machinedVolumeInOrientation = model.fillMachinableVolume(currentOrientation, heightMap);
			
			if(machinedVolumeInOrientation){
				if(printVolume){
					ofstream myfile;
			 		myfile.open ("./" + folderName + "/" + currentOrientation + ".interim.raw", ios::binary );
			 		model.print(myfile);
				}
				trueSequence.push_back(currentOrientation);
			}

			machinedVolume += machinedVolumeInOrientation;
			
		}		
	}
	if(machinedVolume != ((xmax+1)*(ymax+1)*(zmax+1) - model.volume)){
		cout<<"xmax:"<<xmax<<"\n";
		cout<<"ymax:"<<ymax<<"\n";
		cout<<"zmax:"<<zmax<<"\n";
		cout<<"model volume:"<<model.volume<<"\n";
		cout<<"model total volume:"<<(xmax+1)*(ymax+1)*(zmax+1)<<"\n";
		cout<<"Volume to be machined:"<<(xmax+1)*(ymax+1)*(zmax+1) - model.volume<<"\n";
		cout<<"Machined Volume: "<< machinedVolume<<"\n";
		cout<<"Sorry , the object cannot be machined\n";
	}
	
	return make_pair(trueSequence, toolpathSet);
}



/*
Matrix calculateMachinability(Matrix heightmap, int toolRadius, int toolLength){
	int N = heightmap.size(), M = heightmap[0].size(); 
	int n, m, x, y;
	Matrix machinable(N, vector<int>(M, true));

	for(n=0; n<N; n++){
		for(m=0; m<M; m++){
			for(x = n - toolRadius; x<= n + toolRadius; x++){
				if(machinable[n][m]){
					for(y = m - toolRadius; y<= m + toolRadius; y++){
						//tocheck if the point is valid i.e within boundry of heightmap
						if(x>=0 && y>=0 && x<N && y<M){
							//to check if the point is within circle of interest
							if((x-n)*(x-n) + (y-m)*(y-m) <= toolRadius*toolRadius){
								//check if (x,y) satisfy height constraint 
								if((heightmap[n][m] < heightmap[x][y]) && ( heightmap[x][y] - heightmap[n][m] >= toolLength)){
									machinable[n][m] = false;
									break;
								}
							}	
						}
					}
				}
				else{
					break;
				}		
			}
		}
	}
	return machinable;
}
*/

/*
void calculateNewHeightForPoint(Matrix &heightmap, Matrix &heightmapNew, Stack &Qnew, int n, int m, int toolRadius, int toolLength){
	int N = heightmap.size(), M = heightmap[0].size(); 
	int x, y;
	int maxDifference=-1;
	int xmax=n, ymax=m;

	for(x = n - toolRadius; x<= n + toolRadius; x++){
		for(y = m - toolRadius; y<= m + toolRadius; y++){
			//tocheck if the point is valid i.e within boundry of heightmap
			if(x>=0 && y>=0 && x<N && y<M){
				//to check if the point is within circle of interest
				if((x-n)*(x-n) + (y-m)*(y-m) <= toolRadius*toolRadius){
					//check if (x,y) satisfy height constraint 
					if( heightmap[x][y] - heightmap[n][m] >= toolLength){
						int difference = heightmap[x][y] - heightmap[n][m];
						if(difference > maxDifference){
							xmax = x;
							ymax = y;
							maxDifference = difference;
						}
					}
				}	
			}
		}
	}
	if(xmax != n || ymax != m){
		heightmapNew[n][m] = heightmap[xmax][ymax] - toolLength +1;
		Qnew.push(make_pair(n,m));
	}	
	
}
*/

/*
Matrix makeHeightmapMachinable(Matrix heightmap, int toolRadius, int toolLength){
	int N = heightmap.size(), M = heightmap[0].size(); 
	int n, m, x, y;
	Stack Q;
	Matrix heightmapNew(N, vector<int>(M));
	heightmapNew = heightmap;

	// cout<<"Orignal saabji!!-------\n";
	// for(n=0; n<N; n++){
	// 		for(m=0; m<M; m++){
	// 			cout<<heightmap[n][m]<<" ";
	// 		}
	// 		cout<<"\n";
	// 	}



	for(n=0; n<N; n++){
		for(m=0; m<M; m++){
			int maxDifference=-1;
			int xmax=n, ymax=m;
			for(x = n - toolRadius; x<= n + toolRadius; x++){
				for(y = m - toolRadius; y<= m + toolRadius; y++){
					//tocheck if the point is valid i.e within boundry of heightmap
					if(x>=0 && y>=0 && x<N && y<M){
						//to check if the point is within circle of interest
						if((x-n)*(x-n) + (y-m)*(y-m) <= toolRadius*toolRadius){
							//check if (x,y) satisfy height constraint 
							if(heightmap[x][y] - heightmap[n][m] >= toolLength){
								int difference = heightmap[x][y] - heightmap[n][m];
								if(difference > maxDifference){
									xmax = x;
									ymax = y;
									maxDifference = difference;
								}
							}
						}
					}
				}
			}
			if(xmax != n || ymax != m){
				heightmapNew[n][m] = heightmap[xmax][ymax] - toolLength +1;
				Q.push(make_pair(n, m));
			}
		}
	}

	
	while(!Q.empty()){
		Stack Qnew;
		heightmap = heightmapNew;
		// for(n=0; n<N; n++){
		// 	for(m=0; m<M; m++){
		// 		cout<<heightmap[n][m]<<" ";
		// 	}
		// 	cout<<"\n";
		// }
		// cout<<"\n";
		// Matrix machinable = calculateMachinability(heightmap, toolRadius, toolLength);
		// for(n=0; n<N; n++){
		// 	cout<<"m ";
		// 	for(m=0; m<M; m++){
		// 		cout<<machinable[n][m]<<" ";
		// 	}
		// 	cout<<"\n";
		// }
		// cout<<"\n";
		while(!Q.empty()){

			pair<int, int > pixel = Q.top(); Q.pop();
			n = pixel.first;
			m = pixel.second;
			for(x = n - toolRadius; x<= n + toolRadius; x++){
				for(y = m - toolRadius; y<= m + toolRadius; y++){
					//tocheck if the point is valid i.e within boundry of heightmap
					if(x>=0 && y>=0 && x<N && y<M){
						//to check if the point is within circle of interest
						if((x-n)*(x-n) + (y-m)*(y-m) <= toolRadius*toolRadius){
							calculateNewHeightForPoint(heightmap, heightmapNew, Qnew, x, y, toolRadius, toolLength);
						}
					}
				}
			}	
		}
		Q = Qnew;
	}
	return heightmapNew;	
}
*/

