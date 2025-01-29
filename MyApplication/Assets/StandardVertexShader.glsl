#version 330 core

layout (location = 0) in vec3 vertex_pos;


void main(){
	gl_Position = 0.1 * vec4(vertex_pos, 1.0);
}