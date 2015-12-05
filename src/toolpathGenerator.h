//oggpnosn 
//hkhr 

//heightmap to toolpath generator


#include<iostream>
#include<list>
#include<vector>
#include<map>
#include<stack>
#include<algorithm>
#include <fstream>

using namespace std;

typedef map<int, pair<int, list<int> > > Graph;
typedef vector< vector<int> > Matrix;
typedef stack<pair<int, int > > Stack;
typedef list<pair<int, int> > Lop;
typedef list<int> AdjList;
typedef map<int , bool> BoolDict;
typedef stack<int> IntStack;
typedef map<int, int> IntMap;
typedef long long int ll;

class VolumetricModel{
public:

	int xmax, ymax, zmax;
	int xmin, ymin, zmin;//x,y,z varies from *min to *max with both end included 
	vector<vector<vector<int> > > space;
	
	ll calculateMachinableVolume(string orientation){
		/*	
			input: orientation to machine in 
			purpose: it simulates a cut and adds up all the voxels that would be removed while machining
			output: volume, i.e. number of voxels machined
		*/
		int x, y, z;
		ll volume=0;
		
		if(orientation == "xy+"){
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmax; z>=zmin; z--){
						if(space[x][y][z] == true)
							break;
						else if(space[x][y][z] == false)
							volume += 1;
					}
					
				}
			}
		}
		else if(orientation == "xy-"){
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmin; z<=zmax; z++){
						if(space[x][y][z] == true){
							break;
						}
						else if(space[x][y][z] == false)
							volume += 1;
					}
				}
			}	
		}
		else if(orientation == "yz+"){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					for(x= xmax; x>=xmin; x--){
						if(space[x][y][z] == true)
							break;	
						else if(space[x][y][z] == false)
							volume += 1;
					}		
					
				}
			}
		} 
		else if(orientation == "yz-"){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					for(x= xmin; x<=xmax; x++){
						if(space[x][y][z] == true)
							break;	
						else if(space[x][y][z] == false)
							volume += 1;
					}		
					
				}
			}
		}
		else if(orientation == "xz+"){
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymax; y>=ymin; y--){
						if(space[x][y][z] == true)
							break;	
						else if(space[x][y][z] == false)
							volume += 1;
					}
					
				}
			}
		}
		else if(orientation == "xz-"){
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymin; y<=ymax; y++){
						if(space[x][y][z] == true)
							break;	
						else if(space[x][y][z] == false)
							volume += 1;
					}
					
				}
			}
		}

		return volume;
	}

	void fillMachinableVolume(string orientation){
		//Optimization: this function isnt required since this job can be done while calculating machinable volume 

		int x, y, z;
		
		if(orientation == "xy+"){
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmax; z>=zmin; z--){
						if(space[x][y][z] == true)
							break;
						space[x][y][z] = 2;
					}
				}
			}
		}
		else if(orientation == "xy-"){
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmin; z<=zmax; z++){
						if(space[x][y][z] == true){
							break;
						}
						space[x][y][z] = 2;
					}
				}
			}	
		}
		else if(orientation == "yz+"){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					for(x= xmax; x>=xmin; x--){
						if(space[x][y][z] == true)
							break;	
						space[x][y][z] = 2;
					}		
				}
			}
		} 
		else if(orientation == "yz-"){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					for(x= xmin; x<=xmax; x++){
						if(space[x][y][z] == true)
							break;	
						space[x][y][z] = 2;
					}		
				}
			}
		}
		else if(orientation == "xz+"){
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymax; y>=ymin; y--){
						if(space[x][y][z] == true)
							break;	
						space[x][y][z] = 2;
					}
				}
			}
		}
		else if(orientation == "xz-"){
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymin; y<=ymax; y++){
						if(space[x][y][z] == true)
							break;	
						space[x][y][z] = 2;
					}
				}
			}
		}
	}

	Matrix toHeightmap(string orientation){
		//Optimization: this function isnt required since this job can be done while calculating machinable volume 
		/*
			input: orientation in which heightmap is to be created 
			purpose: it converts the give  volumetric model to heightmap in an orientation 
			output: heightmap
		*/
		int x, y, z;
		Matrix heightmap;
		
		if(orientation == "xy+"){
			heightmap = *(new Matrix(xmax-xmin+1, vector<int>(ymax-ymin+1, 0)));
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmax; z>=zmin; z--){
						if(space[x][y][z] == true){
							heightmap[x][y] = z - zmin + 1;
							break;
						}
					}
				}
			}
		}
		else if(orientation == "xy-"){
			heightmap = *(new Matrix(xmax-xmin+1, vector<int>(ymax-ymin+1, 0)));
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmin; z<=zmax; z++){
						if(space[x][y][z] == true){
							heightmap[x][ymax-y] = zmax - z +1;
							break;
						}
						
					}
				}
			}	
		}
		else if(orientation == "yz+"){
			heightmap = *(new Matrix(ymax-ymin+1, vector<int>(zmax-zmin+1, 0)));
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					for(x= xmax; x>=xmin; x--){
						if(space[x][y][z] == true){
							heightmap[y][z] = x - xmin +1;
							break;	
						}
						
					}		
				}
			}
		} 
		else if(orientation == "yz-"){
			heightmap = *(new Matrix(ymax-ymin+1, vector<int>(zmax-zmin+1, 0)));
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					for(x= xmin; x<=xmax; x++){
						if(space[x][y][z] == true){
							heightmap[ymax -y][z] = xmax + 1 - x;
							break;	
						}
					}		
				}
			}
		}
		else if(orientation == "xz+"){
			heightmap = *(new Matrix(xmax-xmin+1, vector<int>(zmax-zmin+1, 0)));
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymax; y>=ymin; y--){
						if(space[x][y][z] == true){
							heightmap[x][z] = y - ymin + 1;
							break;	
						}
					}
				}
			}
		}
		else if(orientation == "xz-"){
			heightmap = *(new Matrix(xmax-xmin+1, vector<int>(zmax-zmin+1, 0)));
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymin; y<=ymax; y++){
						if(space[x][y][z] == true){
							heightmap[xmax-x][z] = ymax -y +1;
							break;		
						}
					}
				}
			}
		}
		return heightmap;
	}

	
	//---------obsolete: consider deleting it!-------
	// //written for testing volume calculation
	// VolumetricModel(vector<vector<int> > heightmap)	{
	// 	/*For testing purpose, it converts given heightmap to a volumetric model*/
	// 	xmin = 0; xmax = heightmap.size()-1;
	// 	ymin = 0; ymax = heightmap[0].size()-1;
	// 	zmin = 0; zmax = calculateMaxZ(heightmap)-1;
	// 	int x, y, z;
	// 	space = *(new vector<vector<vector<bool> > >(xmax-xmin+1, 
	// 							vector<vector<bool> >(ymax-ymin+1, 
	// 									vector<bool>(zmax - zmin+1, false))));

	// 	for(x=xmin; x<=xmax; x++){
	// 		for(y=ymin; y<=ymax; y++){
	// 			for(z=zmin; z<heightmap[x][y]; z++){
	// 				space[x][y][z] = true;
	// 			}
	// 		}
	// 	}
	// }

	VolumetricModel(vector<vector<vector<int> > > voxels, int lMax, int bMax, int hMax){
		/*input: 3d vector array 
		  purpose: convert it to volumetric object
		  output:
		 */
		xmin =0; xmax = lMax-1;
		ymin =0; ymax = bMax-1;
		zmin =0; zmax = hMax-1;

		space = voxels;
	}

	void print(ofstream &fileStream){
		/*input: None
		  purpose: prints voxels for debugging purpose 
		  output: None
		 */
		
		int x, y,z;
		for(z=zmin; z<=zmax; z++){
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					if(space[x][y][z] == true){
						fileStream << std::hex << 0x1;	
					}
					else if(space[x][y][z] == false){
						fileStream << std::hex << 0x0;	
					}
					else{
						fileStream << std::hex << 0x2;	
					}
				}
				
			}
			
		}
		
	}
};




Lop findNeighbour(int a, int b, int N, int M){
	Lop neighbour;

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

pair<Graph, Matrix > toGraph(Matrix heightmap){
	int N = heightmap.size(), M = heightmap[0].size(); 
	Matrix regionmap(N, vector<int >(M, 0));
	Graph graph;
	Stack globalStack, localStack;
	int regionNo;
	int i, j, a, b, x, y;
	pair<int, int> pr;
	Lop neighbour;


	globalStack.push(make_pair(0, 0));
	regionNo = 0;

	while(!globalStack.empty()){
		pr = globalStack.top(); globalStack.pop();
		i = pr.first;
		j = pr.second;
		if(regionmap[i][j] == 0){
			regionNo += 1;
			localStack = Stack();
			localStack.push(make_pair(i, j));

			while(!localStack.empty()){
				pr = localStack.top(); localStack.pop();
				a = pr.first;
				b = pr.second;
				if(regionmap[a][b] == 0){
					regionmap[a][b] = regionNo;

					neighbour = findNeighbour(a,b, N, M);
					Lop::iterator it;
					for(it = neighbour.begin(); it != neighbour.end(); it++){
						x = (*it).first;
						y = (*it).second;

						if(heightmap[a][b] == heightmap[x][y]){
							if(regionmap[x][y] == 0){
								localStack.push(make_pair(x,y));
							}
						}
						else{
							if(regionmap[x][y] == 0){
								globalStack.push(make_pair(x,y));
							}
							else{
								insertEdge(graph, regionmap[x][y], regionmap[a][b], heightmap[x][y], heightmap[a][b]);
								//cout<<heightmap[x][y]<<"( "<<x<<":"<<y<<") -> "<<heightmap[a][b]<<"("<<a<<":"<<b<<")\n";
							}
						}
					}
				}
			}	
		}
	}
	
	return make_pair(graph, regionmap);
}

//it converts heightmap to depthmap by subtracting maximum height with all the heights 
void toDepthGraph(Graph &graph, int maxHeight){
	Graph::iterator it;

	for(it = graph.begin(); it != graph.end(); it++){
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
	if(orientation ==  "xy+"){
		if(model.space[i][j][safeHeight-delta-dep-1] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xy-"){
		if(model.space[i][j][dep] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xz+"){
		if(model.space[i][safeHeight-delta-dep-1][j] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xz-"){
		if(model.space[i][dep][j] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "yz+"){

		if(model.space[safeHeight-delta-dep-1][i][j] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "yz-"){
		if(model.space[dep][i][j] == 2)
			return false;
		else
			return true;
	}
}

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
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta))
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2 \n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}
						iprev = i;jprev =j;
					}
					
				}
				else{
					for(j=n-1; j>=0; j--){
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta))
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2\n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}
						iprev = i;jprev =j;
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
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta))
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2\n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}
						iprev = i;jprev =j;
					}
					
				}
				else{
					for(j=n-1; j>=0; j--){
						if(callItMagic(model, i, j, dep, orientation, safeHeight, delta))
							writeGcode(cutting, isInList, regionmap, toolpath, safeHeight, regionCurrentHeight, i ,  j, dep, iprev, jprev, delta);
						else{
							if(cutting){
								toolpath += ("G1 X" + to_string(iprev) + " Y" + to_string(jprev) +"\n"); 
								toolpath += ("G1 Z2\n");
								toolpath += ("G0 ");
								cutting = false; 
							}
						}
						iprev = i;jprev =j;	
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
string toToolpath(VolumetricModel &model, string orientation, Graph &graph, Matrix &regionmap,int safeHeight, int maxHeight, int TOOL_DIA, int depthPerPass, int feedrate){
	//string toolpath = "G21\nG90\nG92 X0 Y0 Z" + to_string(maxHeight) + "\n";
	string toolpath = "G21\nG90\nF" + to_string(feedrate) + "\n" ;
	int u;
	int i;
	int noOfRegion = graph.size();
	int currentHeight = maxHeight;
	IntMap regionCurrentHeights;

	for(i=1; i<=noOfRegion; i++){
		regionCurrentHeights[i] = maxHeight;
	}

	toDepthGraph(graph, maxHeight);

	toolpath += "G1 Z2\n";
	while(graph.size() != 0){
		u = pickMin(graph);

		AdjList vlist = connected(graph, u);

		toolpath += machine(model, orientation, vlist, regionmap, graph[u].first, noOfRegion, regionCurrentHeights[u],safeHeight,  maxHeight, TOOL_DIA, depthPerPass);

		AdjList::iterator it;
		for(it = vlist.begin(); it!= vlist.end(); it++){
			regionCurrentHeights[*it] -= graph[u].first;
		}

		processGraph(graph, vlist, graph[u].first); 
		
	}	
	return toolpath;
}
