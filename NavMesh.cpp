#ifndef NAVMESH_CPP
#define NAVMESH_CPP

#include "NavMesh.h"

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

unsigned int Node::numVertices() { return points.size(); }

void Node::calculateCenter()
{
	vec3 center = vec3(0.f);

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
















#endif //NAVMESH_CPP