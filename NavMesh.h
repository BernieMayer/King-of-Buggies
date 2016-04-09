#ifndef NAVMESH_H
#define NAVMESH_H

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <queue>
#include <random>

#include "GameState.h"

using namespace std;
using namespace glm;

class Edge
{
public:
	unsigned int index;
	float weight;

	Edge();
	Edge(unsigned int index);
	Edge(unsigned int index, float weight);
};

class Node
{
private:
	vector<vec3> points;
	vector<Edge> edges;
	vec3 center;

	void calculateCenter();

public:
	Node();
	Node(vector<vec3> _points);

	void addPoint(vec3 point);
	void addPoints(vector<vec3> _points);

	void addEdge(unsigned int edge, float weight);
	vector<Edge>::iterator getEdgeIterator();
	vector<Edge>::iterator getEndIterator();

	vec3 operator[] (unsigned int index) const { return points[index]; }
	vec3& operator[] (unsigned int index) { return points[index]; }

	vector<vec3> getPoints();

	unsigned int numVertices() const;

	vec3 getCenter() const { return center; }
	bool inPolygon(vec3 pos);
	float getMin() const;

	unsigned int powerup;
};

//Stores edges in matrix form
//To retrieve weight of edge from i to j, use edges[i][j]
//0 means no edge between i and j
class EdgeMatrix
{
private:
	vector<vector<float>> matrix;

public:
	EdgeMatrix();

	void setSize(unsigned int size);
	unsigned int size() const { return matrix.size(); }

	vector<float> operator[] (unsigned int index) const { return matrix[index]; }
	vector<float>& operator[] (unsigned int index) { return matrix[index]; }

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
	EdgeMatrix edges;
	GameState* state;

	Node operator[] (unsigned int index) const { return nodes[index]; }
	Node& operator[] (unsigned int index) { return nodes[index]; }

	unsigned int numNodes() const { return nodes.size(); }

	void add(Node newNode) { nodes.push_back(newNode); }
	void clear();

	bool findSimilarEdge(const Node& a, const Node& b, unsigned int* a_index, unsigned int* b_index);
	void calculateImplicitEdges();

	void resizeEdges(unsigned int newSize);

	bool loadNavMesh(string fileName);
	bool fetchPowerupLocations();

	void navMeshToLines(vector<vec3>* polygons, vector<vec3>* edges);

	float edgeCost(unsigned int i, unsigned int j);
	float trajectoryCost(vec3 trajectory, vec3 newTrajectory);
	float heuristic(unsigned int i, unsigned int j);
	float proximityCost(unsigned int node);
	float powerupBoxCost(unsigned int node);

	unsigned int getPolygon(vec3 position);

	//Combine these two into one
	//bool getPath_AStar(vector<unsigned int>* path, vec3 position, vec3 forward, vec3 target);
	bool getPath_AStar(vector<unsigned int>* path, vec3 position, vec3 forward, vec3 target);

	bool getPath_Avoidance(vector<unsigned int>* path, vec3 position, vec3 forward);
	bool debugPath_Avoidance(vector<unsigned int>* path, vector<vec3>* pathFinding, vec3 position, vec3 forward, unsigned int numIterations);

	bool getPathLines(vector<vec3>* path, vec3 position, vec3 target);
	bool getPathPoints(vector<vec3>* path, vector<unsigned int>* nodes, vec3 position, vec3 target, vec3 forwards, bool updateOld);		//Use this one for AI
	bool debugAvoidance(vector<vec3>* path, vector<unsigned int>* nodes, vec3 position, vec3 forwards, unsigned int numIterations);


	unsigned int getRandomNode();


};

















#endif //NAVMESH_H