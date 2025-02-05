#version 330 core

in vec3 vertex_normal_out;
out vec4 FragColor;

void main(){
	vec3 color = (vertex_normal_out + 1.0) * 0.5;
	FragColor = vec4(color, 1.0);
}