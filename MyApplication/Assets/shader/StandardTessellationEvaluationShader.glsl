#version 400 core

layout(triangles, equal_spacing, ccw) in;

 in vec3 tcPos[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	vec3 pos = tcPos[0] * gl_TessCoord.x + tcPos[1] * gl_TessCoord.y + tcPos[2] * gl_TessCoord.z;
	gl_Position = projection * view * model * vec4(pos, 1.0);
}