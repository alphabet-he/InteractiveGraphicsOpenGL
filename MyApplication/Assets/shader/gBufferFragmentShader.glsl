#version 330 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gObjColor;
layout(location = 3) out vec4 gAlbedoSpec;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D texture_Kd;
uniform sampler2D texture_Ks;

void main() {
    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normalize(Normal), 1.0);
    gObjColor = texture(texture_Kd, TexCoord).rgba;
    gAlbedoSpec = vec4(texture(texture_Ks, TexCoord).r);
}