#version 330 core
 
layout(location=0) out vec3 fragColor;

in vec3 worldPosition;
in vec3 fragNormal;

uniform vec3 light;
uniform vec3 color;


void main(){
    
    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 lightDir = normalize(light-worldPosition);
    //vec3 lightDir = normalize(light);

    fragColor = (0.1 + 0.8*clamp(dot(lightDir, normalize(fragNormal)), 0, 1.0))*color;
}



