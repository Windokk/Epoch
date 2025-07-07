#version 430 core

out vec4 fragColor;

in vec2 texCoord;
in vec4 color;
in vec3 worldPos;
in vec3 worldNormal;

uniform bool useTexture;
uniform sampler2D diffuse;

void main(){
    if(useTexture){
        fragColor = texture(diffuse, texCoord);
    }
    else{
        fragColor = color;
    }
}