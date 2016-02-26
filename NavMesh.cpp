#ifndef NAVMESH_CPP
#define NAVMESH_CPP

#include "NavMesh.h"

string navMeshDirectory = "./models/";

Edges::Edges()
{

}

void Edges::setSize(unsigned int size)
{
	edgeMatrix.resize(size);
	for (unsigned int i = 0; i < size; i++)
	{
		edgeMatrix[i].resize(size, 0);
	}
}

void Edges::clear() 
{
	edgeMatrix.clear();
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
	vec3 segment = *(it + 1) - *it;
	vec2 perp = vec2(-segment.z, segment.x);
	vec2 toPos = vec2(segment.x - (*it).x, segment.z - (*it).z);

	sign = (dot(perp, toPos) > 0) ? 1.f : -1.f;
		

	for (it; it != points.end(); it++)
	{
		segment = *(it + 1) - *it;
		perp = vec2(-segment.z, segment.x);
		toPos = vec2(segment.x - (*it).x, segment.z - (*it).z);

		float dot_val = dot(perp, toPos);
		//If point is on differnt side of this segment than the first, return false
		if ((dot_val > 0.f && sign <= 0.f) || (dot_val <= 0.f && sign > 0.f))
			return false;		
	}

	return true;
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
	
	std::ifstream f(fileName);
	if (!f.is_open())
	{
		printf("Configuration file could not be opened\n");
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
			cout << "f ";

			vector<unsigned int> face;
			unsigned int index;

			stringstream stream(line);
			while (stream >> index)
			{
				face.push_back(index-1);
				cout << index - 1 << " ";
			}

			if (face.size() >= 3)
				indices.push_back(face);
			cout << endl;
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
	if (a[ind_a%a.numVertices()] == b[ind_b%b.numVertices()])
	{
		*a_index = ind_a;
		*b_index = ind_b;
	}

	return true;
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
			if (findSimilarEdge(nodes[i], nodes[j], &i_a, &i_b))
			{
				edges[i][j] = 1.f;
				edges[j][i] = 1.f;

				point_a[j][i] = point_a[i][j] = nodes[i][i_a];
				point_b[j][i] = point_b[i][j] = nodes[i][(i_a+1)%nodes[i].numVertices()];
			}
		}
	}
}










#endif //NAVMESH_CPP