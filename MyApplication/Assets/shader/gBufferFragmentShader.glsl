#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gObjColor;
layout(location = 3) out float gAlbedoSpec;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D texture_Kd;
uniform sampler2D texture_Ks;

void main() {
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gObjColor = texture(texture_Kd, TexCoord).rgba;
    gAlbedoSpec = texture(texture_Ks, TexCoord).r;
}