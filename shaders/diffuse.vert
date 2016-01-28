#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform mat4 transform;

out vec3 worldPosition;
out vec3 fragNormal;

void main()
{
	fragNormal = vertexNormal;
    worldPosition = vertexPosition.xyz;
 	gl_Position =  transform*vec4(vertexPosition, 1);
}
