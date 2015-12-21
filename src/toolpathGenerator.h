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
typedef pair<pair<int, int>, pair<char, char> > SeedPoint;
typedef stack<SeedPoint> SeedStack;
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
						if(space[x][y][z] == true){
							break;	
						}
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

	int fillMachinableVolume(string orientation, Matrix heightmap){
		//Optimization: this function isnt required since this job can be done while calculating machinable volume 

		int x, y, z;
		int machinedVolume=0;
		
		if(orientation == "xy+"){
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmax; z>=heightmap[x][y]; z--){
						if(space[x][y][z] == false){
							machinedVolume +=1;
						}
						space[x][y][z] = 2;
						
					}
					//machinedVolume += (zmax-heightmap[x][y]-1);
				}
			}
		}
		else if(orientation == "xy-"){
			for(x=xmin; x<=xmax; x++){
				for(y=ymin; y<=ymax; y++){
					for(z=zmin; z<=zmax-heightmap[x][y]; z++){
						if(space[x][y][z] == false){
							machinedVolume +=1;
						}
						space[x][y][z] = 2;
					}
					// machinedVolume += (heightmap[x][y]-1);
				}
			}	
		}
		else if(orientation == "yz+"){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					for(x= xmax; x>=heightmap[y][z]; x--){
						if(space[x][y][z] == false){
							machinedVolume +=1;
						}
						space[x][y][z] = 2;
					}	
					// machinedVolume += (xmax-heightmap[y][z]-1);	
				}
			}
		} 
		else if(orientation == "yz-"){
			for(y=ymin; y<=ymax; y++){
				for(z=zmin; z<=zmax; z++){
					//cout<<"("<<y<<","<<z<<"): ";
					for(x= xmin; x<=xmax- heightmap[y][z]; x++){
						if(space[x][y][z] == false){
							machinedVolume +=1;
							//cout<<x<<" ";
						}
						
						space[x][y][z] = 2;
					}		
					//cout<<"\n";
					// machinedVolume += (heightmap[y][z]-1);
				}

			}
		}
		else if(orientation == "xz+"){
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymax; y>=heightmap[x][z]; y--){	
						if(space[x][y][z] == false){
							machinedVolume +=1;
						}
						space[x][y][z] = 2;
					}
					// machinedVolume += (ymax-heightmap[x][z]-1);
				}
			}
		}
		else if(orientation == "xz-"){
			for(x=xmin; x<=xmax; x++){
				for(z=zmin; z<=zmax; z++){
					for(y=ymin; y<=ymax- heightmap[x][z]; y++){
						if(space[x][y][z] == false){
							machinedVolume +=1;
						}
						space[x][y][z] = 2;
					}
					// machinedVolume += (heightmap[x][z]-1);		
				}
			}
		}
		return machinedVolume;
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
							heightmap[x][y] = zmax - z +1;
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
							heightmap[y][z] = xmax + 1 - x;
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
							heightmap[x][z] = ymax -y +1;
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



pair<Graph, Matrix > toGraph(Matrix &heightmap, int toolRadius, int toolLength){
	int N = heightmap.size(), M = heightmap[0].size(); 
	Matrix regionmap(N, vector<int >(M, 0));
	Graph graph;
	Stack globalStack, localStack;
	int regionNo;
	int i, j, a, b, x, y;
	pair<int, int> pr;
	Lop neighbour;

	heightmap = makeHeightmapMachinable(heightmap, toolRadius, toolLength);
	globalStack.push(make_pair(0, 0));
	regionNo = 0;


	 // Matrix machinable = calculateMachinability(heightmap, toolRadius, toolLength);
	 // int n, m;// N = machinable.size(), M = machinable[0].size();
		// for(n=0; n<N; n++){
		// 	// cout<<"m ";
		// 	for(m=0; m<M; m++){
				
		// 			cout<<machinable[n][m]<<" ";
		// 	}
		// 	cout<<"\n";
		// }
		// cout<<"\n";

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
								//cout<<regionmap[x][y]<<"( "<<x<<","<<y<<") -> "<<regionmap[a][b]<<"("<<a<<","<<b<<")\n";
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
bool doesntHaveEther(VolumetricModel &model, int x, int y, int depth, string orientation, int maxHeight, int regionCurrentHeight){
	
	if(orientation ==  "xy+"){
		if(model.space[x][y][regionCurrentHeight-depth] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xy-"){
		if(model.space[x][y][maxHeight- regionCurrentHeight + depth] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xz+"){
		if(model.space[x][regionCurrentHeight-depth-1][y] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "xz-"){
		if(model.space[x][maxHeight- regionCurrentHeight + depth][y] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "yz+"){

		if(model.space[regionCurrentHeight-depth][x][y] == 2)
			return false;
		else
			return true;
	}
	else if(orientation == "yz-"){
		// cout<<orientation<<" "<<maxHeight- regionCurrentHeight + depth<<" "<<x<<", "<<y<<"\n";
		if(model.space[maxHeight- regionCurrentHeight + depth][x][y] == 2)
			return false;
		else
			return true;
	}
}

bool isExplored(map<string, bool> explored, string coordinate){
	try{
		explored.at(coordinate);
		return true;
	}
	catch(out_of_range){
		return false;
	}
}



bool canWeCut(VolumetricModel &model, string orientation, int x, int y, int maxHeight,map<int, bool> isInVlist,  map<string, bool> explored, int regionCurrentHeight, int depth, Matrix regionmap){
	string coordinate = to_string(x) + "," + to_string(y);
	int Xmax = regionmap.size() , Ymax = regionmap[0].size();
	if(y<0 || x<0 || y>=Ymax || x>=Xmax)
		return false;
	if(!isExplored(explored, coordinate) && doesntHaveEther(model,  x,  y, depth, orientation, maxHeight, regionCurrentHeight) && isInVlist[regionmap[x][y]])
		return true;
	else
		return false;
}

SeedPoint getInitialSeedPoint(VolumetricModel &model, string orientation, int Xmax, int Ymax, char xdirection, char ydirection, int maxHeight,map<int, bool> isInVlist , map<string, bool> explored, int regionCurrentHeight, int depth, Matrix regionmap){
	int x, y; 
	if(xdirection == '+'){
		for(x=0; x<Xmax; x++){
			if(ydirection == '+'){
				for(y=0; y<Ymax; y++){
					if(canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
						return make_pair(make_pair(x, y), make_pair(xdirection, ydirection));
					}
				}	
			}
			else if(ydirection == '-'){
				for(y=Ymax-1; y>=0; y--){
					if(canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
						return make_pair(make_pair(x, y), make_pair(xdirection, ydirection));
					}
				}
			}
		}
	}
	else if(xdirection == '-'){
		for(x=Xmax-1; x>=0; x--){
			if(ydirection == '+'){
				for(y=0; y<Ymax; y++){
					if(canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
						return make_pair(make_pair(x, y), make_pair(xdirection, ydirection));
					}
				}	
			}
			else if(ydirection == '-'){
				for(y=Ymax-1; y>=0; y--){
					if(canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
						return make_pair(make_pair(x, y), make_pair(xdirection, ydirection));
					}
				}
			}
		}
	}

	return make_pair(make_pair(-1, -1), make_pair('*', '*'));
}

int findNextPoint(VolumetricModel &model, string orientation, int Xs, int Ys,int Xmax, int Ymax, int maxHeight,map<int, bool> isInVlist, char xdirection, map<string, bool> explored,int regionCurrentHeight, int depth, Matrix regionmap, int TOOL_DIA){
	int  dy=0;
	
	// cout<<"again me \n";
	
	if(xdirection == '+'){
		
		if(Xs+TOOL_DIA < Xmax){
			if(canWeCut(model, orientation, Xs+TOOL_DIA, Ys, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
				while(Ys+dy<Ymax && Ys-dy>=0){
					if(!canWeCut(model, orientation, Xs+TOOL_DIA, Ys+dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys+dy-1;
					else if(!canWeCut(model, orientation, Xs+TOOL_DIA, Ys-dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys-dy+1;
					dy+=1;
				}
				if(Ys+dy == Ymax)
					return Ymax-1;
				else if(Ys-dy < 0)
					return 0;

			}
			else{
				while(Ys+dy<Ymax || Ys-dy>=0){
					if(canWeCut(model, orientation, Xs+TOOL_DIA, Ys+dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys+dy;
					else if(canWeCut(model, orientation, Xs+TOOL_DIA, Ys-dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys-dy;
					dy+=1;
				}
			}
		}
	}
	else{
		
		if(Xs - TOOL_DIA >=0){
			if(canWeCut(model, orientation, Xs+TOOL_DIA, Ys, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
				while(Ys+dy<Ymax && Ys-dy>=0){
					if(!canWeCut(model, orientation, Xs-TOOL_DIA, Ys+dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys+dy-1;
					else if(!canWeCut(model, orientation, Xs-TOOL_DIA, Ys-dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys-dy+1;
					dy+=1;
				}
				if(Ys+dy == Ymax)
					return Ymax-1;
				else if(Ys-dy < 0)
					return 0;

			}
			else{
				while(Ys+dy<Ymax ||Ys-dy>=0){
					if(canWeCut(model, orientation, Xs-TOOL_DIA, Ys+dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys+dy;
					else if(canWeCut(model, orientation, Xs-TOOL_DIA, Ys-dy, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap))
						return Ys-dy;
					dy+=1;
				}
			}
		}
	}
	

	return -1;


}

SeedPoint findMissedSeed(VolumetricModel &model, string orientation, int Xs, int Ys, int maxHeight,map<int, bool> isInVlist , int Ymax,char xdirection, char ydirection, map<string, bool> explored, int regionCurrentHeight, int depth, Matrix regionmap){
	int y;
	if(ydirection == '+'){
		for(y=Ys; y<Ymax; y++){
			if(canWeCut(model, orientation, Xs, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
				return make_pair(make_pair(Xs, y), make_pair(xdirection, ydirection));
			}
		}
	}
	else if(ydirection == '-'){
		for(y=Ys; y>=0; y--){
			if(canWeCut(model, orientation, Xs, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
				return make_pair(make_pair(Xs, y), make_pair(xdirection, ydirection));
			}
		}
	}
	return make_pair(make_pair(-1, -1), make_pair('*', '*'));
}

//machines connected regions 
string machine(VolumetricModel &model, string orientation, AdjList vlist, Matrix regionmap, int depth, int noOfRegion, int regionCurrentHeight, int safeHeight, int maxHeight, int TOOL_DIA, int depthPerPass){
	char xdirection = '+', ydirection = '+';
	int Xmax = regionmap.size(), Ymax = regionmap[0].size();  
	int regNo;
	map<string, bool> dummyExplored;//a hack!

	// creating map of regions in region map to their existence in vlist to speed up lookup 
	map<int, bool> isInVlist;
	for(regNo = 1; regNo <= noOfRegion; regNo++){
		AdjList::iterator it = find(vlist.begin(), vlist.end(), regNo);
		if(it == vlist.end())
			isInVlist[regNo] = false;
		else
			isInVlist[regNo] = true;
	}

	// fetching seed point and getting the tool to that region 
	SeedPoint seedPoint = getInitialSeedPoint(model, orientation, Xmax,  Ymax,  xdirection, ydirection, maxHeight, isInVlist ,  dummyExplored,  regionCurrentHeight, 0, regionmap);
	SeedPoint missedSeed;
	int Xs, Ys, d, x, y;
	Xs = seedPoint.first.first;
	Ys = seedPoint.first.second;

	string gcode = "G0 X"+ to_string(Xs) + " Y" + to_string(Ys) + "\n";


	for(d=0; d<depth; d++){ 
		map<string, bool> explored;
		seedPoint = getInitialSeedPoint(model, orientation, Xmax, Ymax, xdirection, ydirection, maxHeight, isInVlist,explored, regionCurrentHeight, d, regionmap );
		Xs = seedPoint.first.first;
		Ys = seedPoint.first.second;
		SeedStack Q;
		Q.push(seedPoint);cout<<"d------"<<d<<"\n";
		gcode += "G0 X" + to_string(Xs) + " Y" + to_string(Ys) + "\n";
		gcode += "G0 Z-"+ to_string(maxHeight - regionCurrentHeight+d) + "\n";
		while(Q.size() != 0){
			seedPoint = Q.top();Q.pop();
			// xdirection = '+';
			// ydirection = '-';
			Xs = seedPoint.first.first;
			Ys = seedPoint.first.second;
			gcode += "G0 Z"+ to_string(safeHeight) + "\n"; 
			gcode += "G0 X" + to_string(Xs) + " Y" + to_string(Ys) + "\n";
			gcode += "G0 Z-"+ to_string(maxHeight - regionCurrentHeight+d) + "\n";
			
			// cout<<Xs<<","<<Ys<<","<<d<<"\n";
			ydirection = seedPoint.second.second;
			xdirection = seedPoint.second.first;
			cout<<"("<<Xs<<", "<<Ys<<")"<<ydirection<<"\n";
			bool cutting = true;
			if(xdirection == '+'){
				
				for(x = Xs; x<Xmax; x+=TOOL_DIA){
					if(cutting){
						if(!isExplored(explored , to_string(x) + "," + to_string(Ys))){
							if(ydirection == '+'){
								for(y=Ys; y<Ymax; y+=1){
									if(!canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
										
										break;
									}
								}
								explored[to_string(x) + "," + to_string(Ys)] = true;//cout<<x<<", "<<y-1<<"\n";
								gcode += "G1 X" + to_string(x) + " Y" + to_string(y-1) + "\n";
								Ys = findNextPoint(model, orientation, x, y-1,Xmax, Ymax,  maxHeight,isInVlist, xdirection, explored, regionCurrentHeight, d, regionmap,TOOL_DIA);
								
								if(Ys<0){
									
									cutting = false;
								}
								missedSeed = findMissedSeed(model, orientation, x, y, maxHeight,isInVlist , Ymax,xdirection, ydirection, explored, regionCurrentHeight, d, regionmap);
								if(missedSeed.first.first>=0){
									Q.push(missedSeed);
								}
								explored[to_string(x) + "," + to_string(y-1)] = true;
								ydirection = '-';

							}
							else if(ydirection == '-'){
								
								for(y=Ys; y>=0; y-=1){
									if(!canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
										
										break;
									}
								}
								explored[to_string(x) + "," + to_string(Ys)] = true;
								gcode += "G1 X" + to_string(x) + " Y" + to_string(y+1)+ "\n";
								Ys = findNextPoint(model, orientation, x, y+1,Xmax, Ymax,  maxHeight,isInVlist, xdirection, explored, regionCurrentHeight, d, regionmap,TOOL_DIA);
								
								if(Ys<0){
									
									cutting = false;
								}
								missedSeed = findMissedSeed(model, orientation, x, y, maxHeight,isInVlist , Ymax,xdirection, ydirection, explored, regionCurrentHeight, d, regionmap);
								if(missedSeed.first.first>=0){
									Q.push(missedSeed);
								}
								explored[to_string(x) + "," + to_string(y+1)] = true;
								ydirection = '+';
							}
							if(Ys >= 0)
								gcode += "G1 X" + to_string(x+TOOL_DIA) + " Y" + to_string(Ys) + "\n";
						}
					}
					else{
						break;
					}
					
				}
				xdirection = '-';	
			}
			else if(xdirection == '-'){
				
				for(x = Xs; x>=0; x-=TOOL_DIA){
					if(cutting){
						if(!isExplored(explored , to_string(x) + "," + to_string(Ys))){
							
							if(ydirection == '+'){
								
								for(y=Ys; y<Ymax; y+=1){
									if(!canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
										
										break;
									}
								}
								explored[to_string(x) + "," + to_string(Ys)] = true;//cout<<x<<", "<<y-1<<"\n";
								gcode += "G1 X" + to_string(x) + " Y" + to_string(y-1) + "\n";
								Ys = findNextPoint(model, orientation, x, y-1,Xmax, Ymax,  maxHeight,isInVlist, xdirection, explored, regionCurrentHeight, d, regionmap,TOOL_DIA);
								
								if(Ys<0){
									
									cutting = false;
								}
								missedSeed = findMissedSeed(model, orientation, x, y, maxHeight,isInVlist , Ymax,xdirection, ydirection, explored, regionCurrentHeight, d, regionmap);
								if(missedSeed.first.first>=0){
									Q.push(missedSeed);
								}
								explored[to_string(x) + "," + to_string(y-1)] = true;
								ydirection = '-';
							}
							else if(ydirection == '-'){
								
								for(y=Ys; y>=0; y-=1){
									if(!canWeCut(model, orientation, x, y, maxHeight, isInVlist, explored, regionCurrentHeight,  depth, regionmap)){
										
										
										break;
									}
								}
								gcode += "G1 X" + to_string(x) + " Y" + to_string(y+1)+ "\n";
								explored[to_string(x) + "," + to_string(Ys)] = true;//cout<<x<<", "<<y+1<<"\n";
								Ys = findNextPoint(model, orientation, x, y+1,Xmax, Ymax,  maxHeight,isInVlist, xdirection, explored, regionCurrentHeight, d, regionmap,TOOL_DIA);

								if(Ys<0){
									
									cutting = false;
								}
								missedSeed = findMissedSeed(model, orientation, x, y, maxHeight,isInVlist , Ymax,xdirection, ydirection, explored, regionCurrentHeight, d, regionmap);
								if(missedSeed.first.first>=0){
									Q.push(missedSeed);
								}
								explored[to_string(x) + "," + to_string(y+1)] = true;
								ydirection = '+';
							}
							if(Ys>=0)
								gcode += "G1 X" + to_string(x-TOOL_DIA) + " Y" + to_string(Ys) + "\n";
							
						}	
					}
					else{
						break;
					}
					
				}
				xdirection = '+';
			}
		}
	}
	gcode += "G0 Z"+ to_string(safeHeight) + "\n"; 
	return gcode;
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