#version 330 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 aNormal; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceVP;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(model))); 
	Normal = normalize(normalMatrix * aNormal);

	FragPosLightSpace = lightSpaceVP * model * vec4(aPos, 1.0); // light space
	FragPos = vec3(model * vec4(aPos, 1.0)); // world space

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}