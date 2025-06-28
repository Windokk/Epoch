#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 projectionView;

out vec2 texCoord;
out vec4 color;
out vec3 worldPos;
out vec3 worldNormal;

void main()
{
    texCoord = aTexCoord;
    color = aColor;
    
    vec4 world = model * vec4(aPos, 1.0);
    worldPos = world.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    worldNormal = normalize(normalMatrix * aNormal);

    gl_Position = projectionView * world;
}