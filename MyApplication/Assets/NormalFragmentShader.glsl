#version 330 core

in vec3 FragNormal;
out vec4 FragColor;

void main(){
	vec3 color = (FragNormal + 1.0) * 0.5;
	FragColor = vec4(color, 1.0);
}