#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out mat3 TBN;
out vec3 FragPos;
out vec3 Tangent;

void main(){
	vec3 T = normalize(mat3(model) * aTangent);
    vec3 N = normalize(mat3(model) * aNormal);
	vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

	TexCoord = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));
	Tangent = T;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}