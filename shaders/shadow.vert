#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 transform;
uniform mat4 objectTransform;

void main()
{
 	gl_Position =  transform*vec4(vertexPosition_modelspace, 1);
}
