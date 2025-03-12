#version 330 core

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 uv_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertex_normal_out;
out vec3 vertex_position_out;
out vec2 uv_coordinate_out;

void main(){
	mat3 normalMatrix = transpose(inverse(mat3(model))); 
	vertex_normal_out = normalize(normalMatrix * vertex_normal);

	vertex_position_out = vec3(model * vec4(vertex_pos, 1.0)); // world space

	uv_coordinate_out = uv_coordinate;

	gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}