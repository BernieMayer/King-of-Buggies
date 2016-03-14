#version 330 core
 
layout(location=0) out vec3 fragColor;

in vec2 texCoord;

uniform sampler2D colorTexture;


void main(){
    
	//vec3 color = vec3(texCoord.x, texCoord.y, 0.f);
    vec3 color = texture(colorTexture, texCoord).rgb;

    fragColor = color;
}



