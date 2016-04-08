//oggpnosn 
//hkhr 

#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <string>
using namespace std;

typedef long long int ll;

/* type for orientation for readability */
typedef string Orientation;

/* base class for 2D space */
typedef vector< vector<int> > Matrix;

/* Base class for pixel */
typedef pair<int, int> Point_2D;
/* Base class for voxel kept for future reference */
typedef pair<Point_2D, int> Point_3D;

/* Structures for RegionMap and HeightMap. first element is map and second element is l and b dimensions of the map */
typedef pair<Matrix, pair<int, int>> HeightMap;
typedef pair<Matrix, pair<int, int>> RegionMap;

/* Stack structure for region map generation algorithm */
typedef stack<Point_2D> Stack;

/* Adjacency list for a single node in graph */
typedef list<int> AdjList;

/* Structure for graph. First element is region number. Second element represents height/depth of region and adjancency list. */
typedef map<int, pair<int, AdjList > > Graph;

/* Map of integers */
typedef map<int, int> IntMap;
/* boolean dictionary. Consider removal after merging with dist transform algorithm */
typedef map<int , bool> BoolDict;
/* Integer stack */
typedef stack<int> IntStack;

typedef vector<vector<vector<int>>> Voxels;

typedef pair<AdjList , pair<int, int > > Operation;

typedef list<Operation> OperationPlan;