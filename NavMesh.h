#ifndef NAVMESH_H
#define NAVMESH_H

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;
using namespace glm;


class Node
{
private:
	vector<vec3> points;
	vec3 center;

	void calculateCenter();

public:
	Node();
	Node(vector<vec3> _points);

	void addPoint(vec3 point);
	void addPoints(vector<vec3> _points);

	vec3 operator[] (unsigned int index) const { return points[index]; }
	vec3& operator[] (unsigned int index) { return points[index]; }

	vector<vec3> getPoints();

	unsigned int numVertices() const;

	vec3 getCenter() const { return center; }
	bool inPolygon(vec3 pos);
};

//Stores edges in matrix form
//To retrieve weight of edge from i to j, use edges[i][j]
//0 means no edge between i and j
class Edges
{
private:
	vector<vector<float>> edgeMatrix;

public:
	Edges();

	void setSize(unsigned int size);
	unsigned int size() const { return edgeMatrix.size(); }

	vector<float> operator[] (unsigned int index) const { return edgeMatrix[index]; }
	vector<float>& operator[] (unsigned int index) { return edgeMatrix[index]; }

	void clear();

};


class NavMesh
{
private:
	vector<Node> nodes;

	vector<vector<vec3>> point_a;	//First point on edge
	vector<vector<vec3>> point_b;	//Second point on edge

public:
	NavMesh();
	Edges edges;

	Node operator[] (unsigned int index) const { return nodes[index]; }
	Node& operator[] (unsigned int index) { return nodes[index]; }

	unsigned int numNodes() const { return nodes.size(); }

	void add(Node newNode) { nodes.push_back(newNode); }
	void clear();

	bool findSimilarEdge(const Node& a, const Node& b, unsigned int* a_index, unsigned int* b_index);
	void calculateImplicitEdges();

	void resizeEdges(unsigned int newSize);

	bool loadNavMesh(string fileName);

};

















#endif //NAVMESH_H