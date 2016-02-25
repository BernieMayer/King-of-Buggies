#ifndef NAVMESH_H
#define NAVMESH_H

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

//Stores edges in matrix form
//To retrieve weight of edge from i to j, use edges[i][j]
//0 means no edge between i and j
class Edges
{
private:
	vector<vector<unsigned int>> edgeMatrix;

public:
	Edges();

	void setSize(unsigned int size);
	unsigned int size() { return edgeMatrix.size(); }

	vector<unsigned int> operator[] (unsigned int index) const { return edgeMatrix[index]; }
	vector<unsigned int>& operator[] (unsigned int index) { return edgeMatrix[index]; }

};

class Node
{
private:
	vector<vec3> points;
	vector <vec3> center;

	void calculateCenter();

public:
	Node();
	Node(vector<vec3> _points);

	void addPoint(vec3 point);
	void addPoints(vector<vec3> _points);

	vector<vec3> getPoints();

	unsigned int numVertices();

	vec3 getCenter();
	bool inPolygon(vec3 pos);
};


class NavMesh
{
private:
	vector<Node> nodes;
	Edges edges;

public:
	NavMesh();

	Node operator[] (unsigned int index) const { return nodes[index]; }
	Node& operator[] (unsigned int index) { return nodes[index]; }

	void add(Node newNode) { nodes.push_back(newNode); }
	void clear() { nodes.clear(); }

};

















#endif //NAVMESH_H