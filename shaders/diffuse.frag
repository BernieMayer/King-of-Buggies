#version 330 core
 
in vec3 worldPosition;
in vec3 fragNormal;

uniform vec3 light;
uniform vec3 color;


void main(){
    
    vec3 lightDir = normalize(light-worldPosition);

    color = 0.2 + 0.8*clamp(dot(lightDir, normal), 0, 1);
}
