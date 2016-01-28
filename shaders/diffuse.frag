#version 330 core
 
layout(location=0) out vec3 fragColor;

in vec3 worldPosition;
in vec3 fragNormal;

uniform vec3 light;
uniform vec3 color;


void main(){
    
    vec3 lightDir = normalize(light-worldPosition);

    fragColor = (0.2 + 0.8*clamp(dot(lightDir, fragNormal), 0, 1))*color;
}
