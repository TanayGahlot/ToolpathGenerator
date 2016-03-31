#include <iostream>
#include <map>
#include <list>
#include <vector>

using namespace std;

typedef long long int ll;

typedef string Orientation;
typedef vector< vector<int> > Matrix;

typedef pair<int, int> Point_2D;
typedef pair<Point_2D, int> Point_3D;

typedef pair<Matrix, pair<int, int>> HeightMap;
typedef pair<Matrix, pair<int, int>> RegionMap;

typedef stack<Point_2D> Stack;

typedef list<int> AdjList;
typedef map<int, pair<int, AdjList > > Graph;

typedef map<int, int> IntMap;
typedef map<int , bool> BoolDict;
typedef stack<int> IntStack;
