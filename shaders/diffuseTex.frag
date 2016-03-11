#version 330 core
 
layout(location=0) out vec3 fragColor;

in vec3 worldPosition;
in vec3 fragNormal;
in vec2 texCoord;

uniform vec3 light;
uniform sampler2D colorTexture;


void main(){
    
    vec3 lightDir = normalize(light-worldPosition);
    vec3 color = texture(colorTexture, texCoord).rgb;

    fragColor = (0.1 + 0.8*clamp(dot(lightDir, normalize(fragNormal)), 0, 1.0))*color;
}



