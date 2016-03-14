#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 uv;

uniform mat4 transform;
uniform mat4 objectTransform;

out vec2 texCoord;

void main()
{
	texCoord = uv;
 	gl_Position =  transform*vec4(vertexPosition, 1);
}
