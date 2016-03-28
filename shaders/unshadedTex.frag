#version 330 core
 
layout(location=0) out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D colorTexture;


void main(){
    
	//vec3 color = vec3(texCoord.x, texCoord.y, 0.f);
    vec4 color = texture(colorTexture, texCoord);

    fragColor = color;
}



