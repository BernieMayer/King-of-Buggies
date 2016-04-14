#version 330 core
 
out vec4 fragColor;

in vec3 worldPosition;
in vec3 fragNormal;
in vec2 texCoord;

uniform vec3 light;
uniform vec3 viewPos;
uniform float shininess = 0.7;

uniform sampler2D colorTexture;

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
    
	vec3 color = texture(colorTexture, texCoord).rgb;
	//vec3 color = vec3(texCoord.x, texCoord.y, 0.f);
    fragColor = vec4(torranceSparrowLighting()*color, 1.0);
}



