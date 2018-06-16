#include <iostream>
#include <vector>

using namespace std;

void dist_transform(vector<vector<int>> heightMap, int xMax, int yMax){

	vector<vector<int>> g = vector(xMax, vector(yMax, int));
	for(int x = 0; x<xMax; x++){
		if(heightMap[x][0]){
			g[x][0] = 0;
		}
		else{
			g[x][0] = -1;
		}
	
		for(int y = 1; y<n; y++){
			if(g[x][y]){
				g[x][y] = 0;
			} 
			else{
				g[x][y] = 1 + g[x][y-1];
			}
		}
		for(int y = yMax - 2; y>=0; y--){
			if(g[x][y=1] < g[x][y]){
				g[x][y] = g[x][y+1];
			}
		}
	}
}