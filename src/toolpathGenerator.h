//oggpnosn 
//hkhr 

#include <iostream>
#include <iomanip>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std;

/* Maximum height in the height map */
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

/* Find the valid neighboour points on height map for a point */
list<Point_2D> findNeighbour(Point_2D point, int N, int M){
	list<Point_2D> neighbour;

	if(point.first+1<N){
		neighbour.push_back(make_pair(point.first+1, point.second));
	}
	if(point.first-1>=0){
		neighbour.push_back(make_pair(point.first-1, point.second));	
	}
	if(point.second-1>=0){
		neighbour.push_back(make_pair(point.first, point.second-1));	
	}
	if(point.second+1 < M){
		neighbour.push_back(make_pair(point.first, point.second+1));	
	}
	return neighbour;
}

/* Inserts edge in region graph */
void insertEdge(Graph &graph, int region1, int region2, int height1, int height2){
	try{
		/* Get the relevent adjacency list */
		AdjList iter = graph.at(region1).second;
		
		/* Find region2 in the adjacency list */
		AdjList::iterator it = find(iter.begin(), iter.end(), region2);
		
		/* if found */
		if(it == iter.end()){
			/* add r1, r2 */
			graph[region1].second.push_back(region2);
			insertEdge(graph, region2, region1, height2, height1);
		}
	}
	catch(out_of_range){
		AdjList *adj = new AdjList();
		adj->push_back(region2);
		graph[region1] = make_pair(height1, *adj);
		insertEdge(graph, region2, region1, height2, height1);
	}

}

/* Produces graph from region map */
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
	
	/* Initiate the global stack */
	globalStack.push(make_pair(0, 0));

	while(!globalStack.empty()){
		
		/* Select the point to be processed from new region */
		Point_2D currentPoint = globalStack.top(); 
		globalStack.pop();
		
		/* If the point is unprocessed */
		if(regionMap.first[currentPoint.first][currentPoint.second] == 0){

			regionNumber += 1;
			localStack = Stack();
			
			/* Initiate local stack for processing next region */
			localStack.push(make_pair(currentPoint.first, currentPoint.second));

			while(!localStack.empty()){
				/* Get the point to be processed from current region */
				currentPoint = localStack.top(); 
				localStack.pop();
				
				/* If point is not already processed */
				if(regionMap.first[currentPoint.first][currentPoint.second] == 0){
					
					/* Associate the point to current region */
					regionMap.first[currentPoint.first][currentPoint.second] = regionNumber;

					/* Find neighbours of curret point */
					list<Point_2D> neighbour = findNeighbour(currentPoint, regionMap.second.first, regionMap.second.second);
					
					
					for(list<Point_2D>::iterator it = neighbour.begin(); it != neighbour.end(); it++){
						Point_2D currentNeighbour = (*it);
						
						/* Set point for future consideration if neighbour is not of the height of current region
						 * else push it in local queue for processing
						 */
						if(heightMap.first[currentPoint.first][currentPoint.second] == heightMap.first[currentNeighbour.first][currentNeighbour.second]){
							if(regionMap.first[currentNeighbour.first][currentNeighbour.second] == 0){
								localStack.push(make_pair(currentNeighbour.first, currentNeighbour.second));
							}
						}
						else{
							if(regionMap.first[currentNeighbour.first][currentNeighbour.second] == 0){
								globalStack.push(make_pair(currentNeighbour.first, currentNeighbour.second));
							}
							else{
								/* Adjust graph for identified bordering region */
								insertEdge(graph, regionMap.first[currentNeighbour.first][currentNeighbour.second], regionMap.first[currentPoint.first][currentPoint.second], heightMap.first[currentNeighbour.first][currentNeighbour.second], heightMap.first[currentPoint.first][currentPoint.second]);
							}
						}
					}
				}
			}
		}
	}
	
	return graph;
}

/* Converts Height Graph to Depth Graph */
void toDepthGraph(Graph &graph, int maxHeight){
	
	/* This loop basically converts height to depth */
	for(Graph::iterator it = graph.begin(); it != graph.end(); it++){
		((it->second).first) = maxHeight - ((it->second).first);
	}

}

/* picks the minimum depth node */
int pickMin(Graph &graph){
	
	Graph::iterator graphIter = graph.begin();
	
	/* Store first node as min Node and its depth as min Value */
	int minNode = graphIter->first;
	int minValue = (graphIter->second).first;
	
	while(graphIter != graph.end()){
		/* check if current node is minimum and if true set current node as minimum */
		if((graphIter->second).first < minValue){
			minValue = (graphIter->second).first;
			minNode = graphIter->first;
		}
		graphIter++;
	}
	return minNode;
}

/* Returns all the nodes connected to minNode */
AdjList connected(Graph &graph, int minNode){
	
	IntStack stack;
	AdjList vertexList;
	BoolDict explored;

	stack.push(minNode);
	
	for(Graph::iterator it = graph.begin(); it != graph.end(); it++){
		explored[it->first] = false;
	}
	
	while(stack.size() != 0){
		int currentNode = stack.top(); 
		stack.pop();
		
		if(!explored[currentNode]){
			vertexList.push_back(currentNode);
			explored[currentNode] = true;
			
			for(AdjList::iterator it = graph[currentNode].second.begin(); it != graph[currentNode].second.end(); it++){
				stack.push(*it);
			}
		}		
	}
	return vertexList;
}

/* Removed to add kokopalli system. To be erased completely in future */
/*
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


#include "contour_tracing.h"

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
	
	
	

	cout<<"i am here";
	for(dep = 0; dep<depth; dep+=depthPerPass){
		//toolpath += "G1 Z" + to_string(safeHeight-dep) + "\n";
		
		if(dep%2 == 0){
			for(i=0; i<m; i+=TOOL_DIA){
				cutting = false;
				if((i/TOOL_DIA)%2 == 0){
					for(j=0; j<n; j++){
						cout<<cutting;
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
						cout<<cutting;
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
*/

/* Debug function to display contents of graph */
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

/* Update graph after operation */ 
void processGraph(Graph &graph , AdjList &vertexList, int depth){

	AdjList zeroDepthList;

	/* Reduce depth of all nodes in vertex list and collect all the nodes with zero depth */
	for(AdjList::iterator it = vertexList.begin(); it!= vertexList.end(); it++){
		graph[*it].first = graph[*it].first - depth; 
		if(graph[*it].first == 0){
			zeroDepthList.push_back(*it);
		}
	}

	/* This loop finds connected nodes to each zero depth node and then removes that particular zero depth node from all their adjacency lists */
	for(AdjList::iterator it = zeroDepthList.begin(); it!= zeroDepthList.end(); it++){
		for(AdjList::iterator connected = graph[*it].second.begin(); connected != graph[*it].second.end(); connected++){
			
			AdjList::iterator toBeDeleted = find(graph[*connected].second.begin(), graph[*connected].second.end(), (*it));
			graph[*connected].second.erase(toBeDeleted);
		}	
		graph.erase(*it);
	}
}

/* Operation Sequencing */
string toToolpath(VolumetricModel &model, ToolConfig toolConfig, Orientation orientation, Graph &graph, RegionMap &regionMap, int maxHeight, HeightMap heightMap){
	
	string toolpath = "G21\nG90\nF" + to_string(toolConfig.feedRate) + "\n" ;
	
	int numberOfRegions = graph.size();
	int currentHeight = maxHeight;
	
	IntMap regionCurrentHeights;
	for(int i=1; i<=numberOfRegions; i++){
		regionCurrentHeights[i] = maxHeight;
	}

	toolpath += "G1 Z2\n";
	
	/* Operation Sequencing Algorithm */
	while(graph.size() != 0){
		
		/* Pick minimum depth node */
		int minNode = pickMin(graph);

		/* Get all nodes connected to current node */
		AdjList vertexList = connected(graph, minNode);
		
		/* Machine all the nodes to the respective height */
		toolpath += generate_toolpath_with_compatibility(model, orientation, vertexList, regionMap.first, graph[minNode].first, numberOfRegions, regionCurrentHeights[minNode], toolConfig.safeHeight,  maxHeight, heightMap.first, toolConfig.toolDiameter, toolConfig.stepSize);
		
		/* Display Operation */
		cerr<<"\nOperation: {<";
		for(AdjList::iterator it = vertexList.begin(); it != vertexList.end(); it++){
			cerr<<(*it)<<" ";
		}
		cerr<<">, "<<regionCurrentHeights[minNode]<<", "<<graph[minNode].first<<"}";
		
		/* Reduce regionCurrentHeight of all connected vertices by the depth of minNode */
		for(AdjList::iterator it = vertexList.begin(); it!= vertexList.end(); it++){
			regionCurrentHeights[*it] -= graph[minNode].first;
		}

		processGraph(graph, vertexList, graph[minNode].first);
		
	}	
	return toolpath;
}


/* Operation Planning */ 
string planOperation(VolumetricModel &model, ToolConfig toolConfig, Orientation orientation, HeightMap &heightMap){
	
	int xmax = model.xmax, ymax = model.ymax, zmax = model.zmax;

	RegionMap regionMap;
	
	Graph graph = toGraph(heightMap, toolConfig, regionMap);
	
	string toolpath;
	
	if(orientation == "xy+" || orientation == "xy-"){
		toolConfig.safeHeight = model.zmax +2;
		
		/* Convert Height Graph to Depth Graph */
		toDepthGraph(graph, model.zmax+1);
		
		toolpath = toToolpath(model, toolConfig, orientation, graph, regionMap, model.zmax + 1, heightMap);
	}
	else if(orientation == "yz+" || orientation == "yz-"){
		toolConfig.safeHeight = model.xmax +2;
		
		/* Convert Height Graph to Depth Graph */
		toDepthGraph(graph, model.xmax+1);
		
		toolpath = toToolpath(model, toolConfig, orientation, graph, regionMap, model.xmax + 1, heightMap);	
	}
	else if(orientation == "xz+" || orientation == "xz-"){
		toolConfig.safeHeight = model.ymax +2;
		
		/* Convert Height Graph to Depth Graph */
		toDepthGraph(graph, model.ymax+1);
		
		toolpath = toToolpath(model, toolConfig, orientation, graph, regionMap, model.ymax + 1, heightMap);
	}
	
	model.fillMachinableVolume(orientation, heightMap);
	
	return toolpath;
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

