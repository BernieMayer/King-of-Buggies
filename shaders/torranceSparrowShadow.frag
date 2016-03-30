#version 330 core
 
out vec3 fragColor;

in vec3 worldPosition;
in vec3 fragNormal;
in vec2 texCoord;
in vec4 shadowCoord;

uniform vec3 light;
uniform vec3 viewPos;
uniform float shininess = 0.7;

uniform sampler2D colorTexture;
uniform sampler2DShadow shadowMap;

float ka = 0.2;
float kd = 0.5;
float ks = 0.5;

const float baseBias = 0.005;
const float biasRange = 0.05;
const float radius = 0.002; 

const float PI = 3.14159265359;

const int POINT_SIZE = 16;

uniform vec2 points [POINT_SIZE]; 

//Random function found from internet - Couldn't find out whose it was originally
float rand(vec2 co)
{
   return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

float inLight(vec4 point, float bias, vec2 offset)
{
	vec3 newPoint = point.xyz/point.w;
	newPoint = vec3(newPoint.xy + offset, (newPoint.z-bias));

	//vec3 newPoint = vec3(point.xy + offset, (point.z-bias)*0.5/point.w);
	if((newPoint.x < 0.0) || (newPoint.x > 1.0) || (newPoint.y < 0.0) || (newPoint.y > 1.0))
		return 1.0;
	return texture(shadowMap, newPoint);
}

float testVisibility(vec4 point, float bias)
{
	float theta = 2*PI*rand(shadowCoord.xy);

	vec2 right = vec2(sin(theta), cos(theta));
	vec2 up = vec2(-right.y, right.x);
	right *= radius;
	up *= radius;

	float visibility = 0.0;

	for(int i=0; i< POINT_SIZE; i++)
	{
		//vec2 offset = points[i]*radius; //right*points[i].x + up*points[i].y;		//Static Poisson disk
		vec2 offset = right*points[i].x + up*points[i].y;		//Rotated Poisson disk
		visibility += inLight(point, bias, offset);
	} 
	return visibility*(1.0/16.0);
}

vec2 mappedCoordinate(vec4 point, float bias, vec2 offset)
{
	vec3 newPoint = point.xyz/point.w;
	newPoint = vec3(newPoint.xy + offset, (newPoint.z-bias));

	//vec3 newPoint = vec3(point.xy + offset, (point.z-bias)*0.5/point.w);
	if((newPoint.x < 0.0) || (newPoint.x > 1.0) || (newPoint.y < 0.0) || (newPoint.y > 1.0))
		return vec2(0.f, 0.f);
	return newPoint.xy;
}


float torranceSparrowLighting(float visibility)
{
	vec3 normal = normalize(fragNormal);
	vec3 viewer = normalize(viewPos-worldPosition);
	vec3 lightDir = normalize(light - worldPosition);
	vec3 h = normalize(viewer + lightDir);

	return 	(ks*(shininess+2.0)*(0.5/PI) * pow(clamp(dot(normal, h), 0.0, 1.0), shininess)
			+ kd*clamp(dot(normal, lightDir), 0.0, 1.0))*visibility 
			+ka;
}

void main(){
    
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(light - worldPosition);
	float bias = biasRange*(1-clamp(dot(normal, lightDir), 0.0, 1.0)+baseBias);	
	
	float visibility = testVisibility(shadowCoord, bias);

	//vec2 mc = mappedCoordinate(shadowCoord, bias, vec2(0.f, 0.f));	
	//fragColor = vec3(mc.x, mc.y, 0.f);

	
	vec3 color = texture(colorTexture, texCoord).rgb;
    fragColor = torranceSparrowLighting(visibility)*color;
}



