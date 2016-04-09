//oggpnosn 
//hkhr 

//operation planner 



#include <iostream>
#include <iomanip>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>
#include "./model/structures.h"

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
Graph toGraph(HeightMap heightMap, RegionMap &regionMap){
	
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




/* Operation Planning */ 
OperationPlan makeOperationPlan(HeightMap &heightMap, RegionMap &regionMap, int maxHeight){
	OperationPlan plan;
	
	
	Graph graph = toGraph(heightMap, regionMap);
	toDepthGraph(graph, maxHeight+1);
	
	int numberOfRegions = graph.size();
	int currentHeight = maxHeight;
	
	IntMap regionCurrentHeights;
	for(int i=1; i<=numberOfRegions; i++){
		regionCurrentHeights[i] = maxHeight;
	}

	
	/* Operation Sequencing Algorithm */
	while(graph.size() != 0){
		Operation action;	
		/* Pick minimum depth node */
		int minNode = pickMin(graph);

		/* Get all nodes connected to current node */
		AdjList vertexList = connected(graph, minNode);
		
		action = make_pair(vertexList, make_pair(regionCurrentHeights[minNode], graph[minNode].first));
		plan.push_back(action);
		/* Reduce regionCurrentHeight of all connected vertices by the depth of minNode */
		for(AdjList::iterator it = vertexList.begin(); it!= vertexList.end(); it++){
			regionCurrentHeights[*it] -= graph[minNode].first;
		}

		processGraph(graph, vertexList, graph[minNode].first);
	}
	return plan;
}


string print(OperationPlan plan, RegionMap regionmap){
	string jsonOutput = "{\"regionmap\": {";
	int length, width, i, j;
	length = regionmap.second.first;
	width = regionmap.second.second;

	jsonOutput += "\"length\":" + to_string(length) + ", \"width\":" + to_string(width) + ",\"raw\": \"";

	for(i=0; i<length; i++){
		for(j=0; j<width; j++){
			jsonOutput += to_string(regionmap.first[i][j]) + " ";
		}
	}

	jsonOutput += "\"},";

	jsonOutput += "\"operationPlan\": [";
	int noOfOperation = plan.size();
	OperationPlan::iterator iter ;

	for(iter=plan.begin(); iter != plan.end(); iter++ ){
		jsonOutput += "{\"regionlist\": \"";
		AdjList::iterator region;
		for(region = (iter->first).begin(); region != (iter->first).end(); region++){
			jsonOutput += to_string(*region) + " ";
		}
		jsonOutput += "\", \"startHeight\": " + to_string((iter->second).first) + ",\"depth\":" + to_string((iter->second).second) + "},";
	}
	jsonOutput.pop_back();
	jsonOutput += "]}";
	return jsonOutput;
}



int main(int argc, char **argv){
	int length, width, maxHeight;
	int i, j;
	HeightMap heightmap;
	RegionMap regionMap;
	
	cin>>length>>width>>maxHeight;
	heightmap = make_pair(*(new Matrix(length, vector<int>(width, 0))), make_pair(length, width));

	for(i=0; i<length; i++){
		for(j=0; j<width; j++){
			cin>>heightmap.first[i][j];
		}
	}

	OperationPlan operationPlan = makeOperationPlan(heightmap, regionMap, maxHeight);
	string planJson = print(operationPlan, regionMap);

	cout<<planJson;
	return 0;
}