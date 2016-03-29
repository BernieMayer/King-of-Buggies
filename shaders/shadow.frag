#version 330 core
//layout(location=0) out float fragDepth;
layout(location=0) out vec3 outColor;
 
//in vec3 vertexPosition;

void main()
{
	outColor = vec3(1.f, 1.f, 1.f)*(gl_FragCoord.z);
   
}
