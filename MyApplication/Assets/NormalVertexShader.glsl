#version 330 core

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragNormal;

void main(){
	mat3 normalMatrix = transpose(inverse(mat3(model))); 
	FragNormal = normalize(normalMatrix * vertex_normal);

	gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
	gl_PointSize = 1.6;
}