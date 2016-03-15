#ifndef NAVMESH_CPP
#define NAVMESH_CPP

#include "NavMesh.h"

//const unsigned int NO_VALUE = UINT_MAX;

string navMeshDirectory = "./navigation/";


Edge::Edge() :index(0), weight(0.f)
{
}

Edge::Edge(unsigned int _index) : index(_index), weight(1.f)
{
}

Edge::Edge(unsigned int _index, float _weight) : index(_index), weight(_weight)
{
}

EdgeMatrix::EdgeMatrix()
{

}

void EdgeMatrix::setSize(unsigned int size)
{
	matrix.resize(size);
	for (unsigned int i = 0; i < size; i++)
	{
		matrix[i].resize(size, 0);
	}
}

void EdgeMatrix::clear() 
{
	matrix.clear();
}

Node::Node()
{

}

Node::Node(vector<vec3> _points):
points(_points)
{

}

void Node::addPoint(vec3 point) 
{ 
	points.push_back(point); 
	calculateCenter();
}

void Node::addPoints(vector<vec3> _points) 
{ 
	points = _points; 
	calculateCenter();
}


vector<vec3> Node::getPoints() { return points; }

unsigned int Node::numVertices() const { return points.size(); }

void Node::calculateCenter()
{
	center = vec3(0.f);

	for (vector<vec3>::iterator it = points.begin(); it != points.end(); it++)
	{
		center += *it;
	}

	center /= (float)points.size();
}

bool Node::inPolygon(vec3 pos)
{
	float sign = 0.f;

	vector<vec3>::iterator it = points.begin();
	vector<vec3>::iterator end = points.end()-1;

	vec3 segment = *it - *end;
	vec2 perp = vec2(-segment.z, segment.x);
	vec2 toPos = vec2(pos.x - (*end).x, pos.z - (*end).z);

	sign = (dot(perp, toPos) > 0) ? 1.f : -1.f;
		

	for (it; it+1 != points.end(); it++)
	{
		segment = *(it + 1) - *it;
		perp = vec2(-segment.z, segment.x);
		toPos = vec2(pos.x - (*it).x, pos.z - (*it).z);

		float dot_val = dot(perp, toPos);
		//If point is on differnt side of this segment than the first, return false
		if ((dot_val > 0.f && sign <= 0.f) || (dot_val <= 0.f && sign > 0.f))
			return false;		
	}

	return true;
}

void Node::addEdge(unsigned int edge, float weight)
{
	edges.push_back(Edge(edge, weight));
}

vector<Edge>::iterator Node::getEdgeIterator()
{
	return edges.begin();
}

vector<Edge>::iterator Node::getEndIterator()
{
	return edges.end();
}


NavMesh::NavMesh()
{

}

void NavMesh::clear()
{
	nodes.clear();
	edges.clear();
}


bool NavMesh::loadNavMesh(string fileName)
{
	clear();

	printf("Loading navigation mesh\n");
	
	std::ifstream f(navMeshDirectory+fileName);
	if (!f.is_open())
	{
		printf("Navigation mesh could not be opened\n");
		return false;
	}

	vector<vec3> vertices;
	vector < vector<unsigned int>> indices;

	std::string line;
	vec3 temp;

	while (getline(f, line))
	{
		//Add vertex
		if (sscanf(line.c_str(), "v %f %f %f", &temp.x, &temp.y, &temp.z) == 3)
			vertices.push_back(temp);
		//Add index
		else if (line.find("f") != string::npos)
		{
			vector<unsigned int> face;
			int index;

			stringstream stream(line);
			stream.ignore(10, ' ');

			while (stream >> index)
			{
				face.push_back(index-1);
			}

			if (face.size() >= 3)
				indices.push_back(face);
		}
	}

	f.close();

	vector < vector<unsigned int> >::iterator face;
	//Iterate through faces
	for (face = indices.begin(); face != indices.end(); face++)
	{

		Node newNode;
		//Add each point on face to node
		for (unsigned int i = 0; i < (*face).size(); i++)
		{
			newNode.addPoint(vertices[(*face)[i]]);
		}

		nodes.push_back(newNode);
	}

	edges.setSize(nodes.size());

	return true;

}

bool NavMesh::findSimilarEdge(const Node& a, const Node& b, unsigned int* a_index, unsigned int* b_index)
{
	unsigned int ind_a, ind_b;

	for (unsigned int i = 0; i < a.numVertices(); i++)
	{
		for (unsigned int j = 0; j < b.numVertices(); j++)
		{
			if (a[i] == b[j])
			{
				ind_a = i;
				ind_b = j;
				goto found;
			}
		}
	}
	
	return false;

found:
	if (a[(ind_a+1)%a.numVertices()] == b[(ind_b+1)%b.numVertices()])
	{
		*a_index = ind_a;
		*b_index = ind_b;
		return true;
	}
	else if (a[(ind_a + 1) % a.numVertices()] == b[(ind_b + b.numVertices()- 1) % b.numVertices()])
	{
		*a_index = ind_a;
		*b_index = ind_b;
		return true;
	}
	else if (a[(ind_a + a.numVertices() - 1) % a.numVertices()] == b[(ind_b + 1) % b.numVertices()])
	{
		*a_index = (ind_a - 1)%a.numVertices();
		*b_index = (ind_b + 1) % b.numVertices();
		return true;
	}
	else
	{
		return false;
	}
		
}

void NavMesh::resizeEdges(unsigned int newSize)
{
	edges.setSize(newSize);

	point_a.resize(newSize);
	point_b.resize(newSize);

	for (unsigned int i = 0; i < newSize; i++)
	{
		point_a[i].resize(newSize, vec3(0.f, 0.f, 0.f));
		point_b[i].resize(newSize, vec3(0.f, 0.f, 0.f));
	}
}

void NavMesh::calculateImplicitEdges()
{
	resizeEdges(nodes.size());

	unsigned int i_a, i_b;
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		for (unsigned int j = i; j < nodes.size(); j++)
		{
			if ((i != j) && (findSimilarEdge(nodes[i], nodes[j], &i_a, &i_b)))
			{
				edges[i][j] = 1.f;
				edges[j][i] = 1.f;

				nodes[i].addEdge(j, 1.f);
				nodes[j].addEdge(i, 1.f);


				point_a[j][i] = point_a[i][j] = nodes[i][i_a];
				point_b[j][i] = point_b[i][j] = nodes[i][(i_a+1)%nodes[i].numVertices()];
			}
		}
	}
}

unsigned int NavMesh::getPolygon(vec3 position)
{
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i].inPolygon(position))
			return i;
	}

	return NO_VALUE;
}

class IndexWeightPair
{
public:
	unsigned int i;
	float w;
	IndexWeightPair(unsigned int index, float weight) :i(index), w(weight) {}
};

struct IndexWeightComparison
{
	bool operator()(const IndexWeightPair& a, const IndexWeightPair& b) const
	{
		return a.w > b.w;
	}
};


//Cost for traversing edge from node i to node j
float NavMesh::edgeCost(unsigned int i, unsigned int j)
{
	return edges[i][j];
}

float NavMesh::trajectoryCost(vec3 trajectory, vec3 newTrajectory)
{
	float weight = 1.f - (dot(newTrajectory, trajectory) + 1.f)*0.5;
	return weight*weight*20.f;
}

float NavMesh::heuristic(unsigned int current, unsigned int finish)
{
	vec3 diff = nodes[current].getCenter() - nodes[finish].getCenter();
	return abs(diff.x) + abs(diff.y) + abs(diff.z);
}

float NavMesh::proximityCost(unsigned int node)
{
	const float closeRange = 10.f;
	const float midRange = 30.f;

	vec3 nodePos = nodes[node].getCenter();
	float sumOfDists = 0.f;

	for (unsigned int i = 0; i < state->numberOfPlayers(); i++)
	{
		if (state->getPlayer(i) != state->getGoldenBuggy())
		{
			vec3 diff = nodePos - state->getPlayer(i)->getPos();
			sumOfDists += dot(diff, diff);
		}
	}

	sumOfDists /= (float)(state->numberOfPlayers() - 1);

	float weight = std::max((closeRange*closeRange - sumOfDists)/(closeRange*closeRange), 0.f)*100.f;
	//If outside close range
	if (weight == 0)
		return std::max((midRange*midRange - sumOfDists) / (midRange*midRange), 0.f) * 30.f;

	return weight;
}



/*
bool NavMesh::getPath_AStar(vector<unsigned int>* path, vec3 position, vec3 forwards, vec3 target)
{
	path->clear();

	unsigned int start = getPolygon(position);		//Index of polygon containing starting position
	unsigned int finish = getPolygon(target);		//Index of polygon containing finishing position

	if ((start == NO_VALUE) || (finish == NO_VALUE))
		return false;

	vector<unsigned int> came_from(nodes.size(), NO_VALUE);	//Stores node with shortest path to this one
	vector<float> cost(nodes.size(), -1.f);		//Stores cost of node
	vector<vec3> trajectory(nodes.size(), vec3(0.f));
	trajectory[start] = forwards;

	//Priority queue storing outer ring of explored nodes
	priority_queue<IndexWeightPair, vector<IndexWeightPair>, IndexWeightComparison> frontier;

	cost[start] = 0;
	frontier.push(IndexWeightPair(start, 0.f));

	while (!frontier.empty())
	{
		IndexWeightPair current = frontier.top();
		frontier.pop();

		if (current.i == finish)
			break;

		//Iterate through adjacent nodes to current
		vector<Edge>::iterator next = nodes[current.i].getEdgeIterator();
		for (next; next != nodes[current.i].getEndIterator(); next++)
		{
			vec3 newTrajectory = normalize(nodes[next->index].getCenter() - nodes[current.i].getCenter());
			float newCost = cost[current.i] + edgeCost(current.i, next->index) 
				+ trajectoryCost(trajectory[current.i], newTrajectory);

			if ((cost[next->index] == -1.f) || (newCost < cost[next->index]))
			{
				cost[next->index] = newCost;
				float priority = newCost + heuristic(next->index, finish);

				frontier.push(IndexWeightPair(next->index, priority));
				came_from[next->index] = current.i;
				trajectory[next->index] = newTrajectory;
			}
		}

	}

	unsigned int index = finish;
	vector<unsigned int> temp;

	//Trace path from finish to start
	while (index != NO_VALUE)
	{
		temp.push_back(index);
		index = came_from[index];
	}

	//Make sure path reaches beginning
	if (temp.at(temp.size() - 1) != start)
		return false;

	//Reverse path - consider swapping end and beginning to get already reversed path
	for (unsigned int i = 0; i < temp.size(); i++)
	{
		path->push_back(temp[temp.size() - 1 - i]);
	}

	return true;
}*/


float prevPathWeight(unsigned int i, unsigned int num)
{
	const float weight = 10.f;

	float f_i = (float)i;
	float f_num = (float)num;

	return -(1.f - f_i / f_num)*weight;
}


bool NavMesh::getPath_AStar(vector<unsigned int>* path, vec3 position, vec3 forwards, vec3 target)
{

	unsigned int start = getPolygon(position);		//Index of polygon containing starting position
	unsigned int finish = getPolygon(target);		//Index of polygon containing finishing position

	if ((start == NO_VALUE) || (finish == NO_VALUE))
		return false;

	vector<unsigned int> came_from(nodes.size(), NO_VALUE);	//Stores node with shortest path to this one
	vector<float> cost(nodes.size(), -1.f);		//Stores cost of node
	vector<vec3> trajectory(nodes.size(), vec3(0.f));
	trajectory[start] = forwards;

	vector<float> initWeight(nodes.size(), 0.f);


	//Set points in last path as lower weights
	for (unsigned int i = 0; i < path->size(); i++)
	{
		initWeight[path->at(i)] = prevPathWeight(i, path->size());
	}

	path->clear();

	//Priority queue storing outer ring of explored nodes
	priority_queue<IndexWeightPair, vector<IndexWeightPair>, IndexWeightComparison> frontier;

	cost[start] = 0;
	frontier.push(IndexWeightPair(start, 0.f));

	while (!frontier.empty())
	{
		IndexWeightPair current = frontier.top();
		frontier.pop();

		if (current.i == finish)
			break;

		//Iterate through adjacent nodes to current
		vector<Edge>::iterator next = nodes[current.i].getEdgeIterator();
		for (next; next != nodes[current.i].getEndIterator(); next++)
		{
			vec3 newTrajectory = normalize(nodes[next->index].getCenter() - nodes[current.i].getCenter());
			float newCost = cost[current.i] + initWeight[next->index] 
							+ edgeCost(current.i, next->index)
							+ trajectoryCost(trajectory[current.i], newTrajectory);
			newCost = std::max(newCost, cost[current.i] + 1.f);

			if ((cost[next->index] == -1.f) || (newCost < cost[next->index]))
			{
				cost[next->index] = newCost;
				float priority = newCost + heuristic(next->index, finish);

				frontier.push(IndexWeightPair(next->index, priority));
				came_from[next->index] = current.i;
				trajectory[next->index] = newTrajectory;
			}
		}

	}

	unsigned int index = finish;
	vector<unsigned int> temp;

	//Trace path from finish to start
	while (index != NO_VALUE)
	{
		temp.push_back(index);
		index = came_from[index];
	}

	//Make sure path reaches beginning
	if (temp.at(temp.size() - 1) != start)
		return false;

	//Reverse path - consider swapping end and beginning to get already reversed path
	for (unsigned int i = 0; i < temp.size(); i++)
	{
		path->push_back(temp[temp.size() - 1 - i]);
	}

	return true;
}



bool NavMesh::getPath_Avoidance(vector<unsigned int>* path, vec3 position, vec3 forwards)
{

	unsigned int start = getPolygon(position);		//Index of polygon containing starting position

	if ((start == NO_VALUE))
		return false;

	vector<unsigned int> came_from(nodes.size(), NO_VALUE);	//Stores node with shortest path to this one
	vector<float> cost(nodes.size(), -1.f);		//Stores cost of node
	vector<vec3> trajectory(nodes.size(), vec3(0.f));
	trajectory[start] = forwards;

	vector<float> initWeight(nodes.size(), 0.f);


	//Set points in last path as lower weights
	for (unsigned int i = 0; i < path->size(); i++)
	{
		initWeight[path->at(i)] = prevPathWeight(i, path->size());
	}

	path->clear();

	//Priority queue storing outer ring of explored nodes
	priority_queue<IndexWeightPair, vector<IndexWeightPair>, IndexWeightComparison> frontier;

	cost[start] = 0;
	frontier.push(IndexWeightPair(start, 0.f));

	unsigned int finish;

	while (!frontier.empty())
	{
		IndexWeightPair current = frontier.top();
		frontier.pop();

		//Iterate through adjacent nodes to current
		vector<Edge>::iterator next = nodes[current.i].getEdgeIterator();
		for (next; next != nodes[current.i].getEndIterator(); next++)
		{
			vec3 newTrajectory = normalize(nodes[next->index].getCenter() - nodes[current.i].getCenter());
			float newCost = cost[current.i] + initWeight[next->index]
				+ edgeCost(current.i, next->index)
				+ trajectoryCost(trajectory[current.i], newTrajectory)
				+ proximityCost(next->index);
			newCost = std::max(newCost, cost[current.i] + 1.f);

			if ((cost[next->index] == -1.f) || (newCost < cost[next->index]))
			{
				cost[next->index] = newCost;
				float priority = newCost;

				frontier.push(IndexWeightPair(next->index, priority));
				came_from[next->index] = current.i;
				trajectory[next->index] = newTrajectory;

				if (length(nodes[next->index].getCenter() - nodes[start].getCenter()) > 30.f)
				{
					finish = next->index;
					goto foundTarget;
				}
			}
		}

	}

foundTarget:
	vector<unsigned int> temp;

	unsigned int index = finish;

	//Trace path from finish to start
	while (index != NO_VALUE)
	{
		temp.push_back(index);
		index = came_from[index];
	}

	//Make sure path reaches beginning
	if (temp.at(temp.size() - 1) != start)
		return false;

	//Reverse path - consider swapping end and beginning to get already reversed path
	for (unsigned int i = 0; i < temp.size(); i++)
	{
		path->push_back(temp[temp.size() - 1 - i]);
	}

	return true;
}



bool NavMesh::debugPath_Avoidance(vector<unsigned int>* path, vector<vec3>* pathFinding, vec3 position, vec3 forwards, unsigned int numIterations)
{

	unsigned int start = getPolygon(position);		//Index of polygon containing starting position

	if ((start == NO_VALUE))
		return false;

	vector<unsigned int> came_from(nodes.size(), NO_VALUE);	//Stores node with shortest path to this one
	vector<float> cost(nodes.size(), -1.f);		//Stores cost of node
	vector<vec3> trajectory(nodes.size(), vec3(0.f));
	trajectory[start] = forwards;

	vector<float> initWeight(nodes.size(), 0.f);


	//Set points in last path as lower weights
	for (unsigned int i = 0; i < path->size(); i++)
	{
		initWeight[path->at(i)] = prevPathWeight(i, path->size());
	}

	//Priority queue storing outer ring of explored nodes
	priority_queue<IndexWeightPair, vector<IndexWeightPair>, IndexWeightComparison> frontier;

	cost[start] = 0;
	frontier.push(IndexWeightPair(start, 0.f));

	unsigned int finish;

	unsigned int iterations = 0;

	while ((!frontier.empty()) && (iterations < numIterations))
	{
		IndexWeightPair current = frontier.top();
		frontier.pop();

		//Iterate through adjacent nodes to current
		vector<Edge>::iterator next = nodes[current.i].getEdgeIterator();
		for (next; next != nodes[current.i].getEndIterator(); next++)
		{
			vec3 newTrajectory = normalize(nodes[next->index].getCenter() - nodes[current.i].getCenter());
			float newCost = cost[current.i] + initWeight[next->index]
				+ edgeCost(current.i, next->index)
				+ trajectoryCost(trajectory[current.i], newTrajectory)
				+ proximityCost(next->index);
			newCost = std::max(newCost, cost[current.i] + 1.f);

			if ((cost[next->index] == -1.f) || (newCost < cost[next->index]))
			{
				cost[next->index] = newCost;
				float priority = newCost;

				frontier.push(IndexWeightPair(next->index, priority));
				came_from[next->index] = current.i;
				trajectory[next->index] = newTrajectory;

				if (length(nodes[next->index].getCenter() - nodes[start].getCenter()) > 30.f)
				{
					finish = next->index;
					goto foundTarget;
				}
			}
		}

		iterations++;
	}

foundTarget:
	
	for (unsigned int i = 0; i < came_from.size(); i++)
	{
		if (came_from[i] != NO_VALUE)
		{
			float scale = 0.1;

			vec3 a = nodes[i].getCenter();
			a.y += scale*cost[i];
			pathFinding->push_back(a);
			pathFinding->push_back(nodes[i].getCenter());
			pathFinding->push_back(a);

			vec3 b = nodes[came_from[i]].getCenter();
			b.y += scale*cost[came_from[i]];
			pathFinding->push_back(b);
			pathFinding->push_back(nodes[came_from[i]].getCenter());
			pathFinding->push_back(b);


		}
	}

	return true;
}



//Member function
void NavMesh::navMeshToLines(vector<vec3>* polygons, vector<vec3>* outEdges)
{

	//Lines for polygons
	for (unsigned int i = 0; i < numNodes(); i++)
	{
		for (unsigned int j = 0; j < nodes[i].numVertices(); j++)
		{
			polygons->push_back(nodes[i][j]);
			polygons->push_back(nodes[i][(j + 1) % nodes[i].numVertices()]);
		}
	}

	//Edges between polygons
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		for (unsigned int j = 0; j < edges[i].size(); j++)
		{
			if (edges[i][j] > 0.f)
			{
				outEdges->push_back(nodes[i].getCenter());
				outEdges->push_back(nodes[j].getCenter());
			}
		}
	}
}

bool NavMesh::getPathLines(vector<vec3>* path, vec3 position, vec3 target)
{
	vector<unsigned int> indices;
	path->push_back(vec3(position.x, position.y+6.f, position.z));

	if (getPath_AStar(&indices, position, target, vec3(0.f, 0.f, 1.f)))
	{
		for (unsigned int i = 1; i < indices.size(); i++)
		{
			//path->push_back(nodes[indices[i - 1]].getCenter());
			//path->push_back(nodes[indices[i]].getCenter());
			vec3 a = point_a[indices[i - 1]][indices[i]];
			vec3 b = point_b[indices[i - 1]][indices[i]];
			vec3 midpoint = b + (a - b)*0.5f;

			path->push_back(midpoint);
			path->push_back(midpoint);
		}

		path->push_back(vec3(target.x, target.y+6.f, target.z));
		
		return true;
	}
	else
		return false;
}

//Use this one!
bool NavMesh::getPathPoints(vector<vec3>* path, vector<unsigned int>* nodes, vec3 position, vec3 forwards, vec3 target, bool chasing)
{
	vector<unsigned int> indices;
	//nodes->clear();

	bool pathFound = (chasing) ? getPath_AStar(nodes, position, forwards, target) :
								getPath_Avoidance(nodes, position, forwards);

	if (pathFound)
	{
		for (unsigned int i = 1; i < nodes->size(); i++)
		{
			vec3 a = point_a[(*nodes)[i - 1]][(*nodes)[i]];
			vec3 b = point_b[(*nodes)[i - 1]][(*nodes)[i]];
			vec3 midpoint = b + (a - b)*0.5f;

			path->push_back(midpoint);
		}

		path->push_back(target);

		return true;
	}
	else
		return false;
}


bool NavMesh::debugAvoidance(vector<vec3>* path, vector<unsigned int>* nodePaths, vec3 position, vec3 forwards, unsigned int numIterations)
{
	bool pathFound = debugPath_Avoidance(nodePaths, path, position, forwards, numIterations);

	return pathFound;
}



unsigned int NavMesh::getRandomNode()
{
	return rand() % nodes.size();
}





#endif //NAVMESH_CPP