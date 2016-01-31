#version 330 core
 
layout(location=0) out vec3 fragColor;

in vec3 worldPosition;
in vec3 fragNormal;

uniform vec3 light;
uniform vec3 color;
uniform vec3 viewPos;
uniform float shininess;

float ka = 0.1;
float kd = 0.5;
float ks = 0.5;

const float PI = 3.14159265359;

float torranceSparrowLighting()
{
	vec3 normal = normalize(fragNormal);
	vec3 viewer = normalize(viewPos-worldPosition);
	vec3 lightDir = normalize(light - worldPosition);
	vec3 h = normalize(viewer + lightDir);

	return ks*(shininess+2.0)*(0.5/PI) * pow(clamp(dot(normal, h), 0.0, 1.0), shininess)
			+ kd*clamp(dot(normal, lightDir), 0.0, 1.0)+ka;
}

void main(){
    
    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 lightDir = normalize(light-worldPosition);
    //vec3 lightDir = normalize(light);

    fragColor = torranceSparrowLighting()*color;
}



