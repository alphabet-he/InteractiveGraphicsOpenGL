#version 330 core

#define MAX_LIGHTS 16

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceVP[MAX_LIGHTS];
uniform int lightNum;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace[MAX_LIGHTS];
out int LightNum;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
    TexCoord = aTex;
    LightNum = lightNum;

    for(int i = 0; i < lightNum; i++){
        FragPosLightSpace[i] = lightSpaceVP[i] * model * vec4(aPos, 1.0); // light space
    }

    gl_Position = projection * view * vec4(FragPos, 1.0);
}