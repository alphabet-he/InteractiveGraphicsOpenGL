#version 400 core
 
 in vec3 vPos[];

 out vec3 tcPos[];

 uniform int tessellation_level = 4;

 layout(vertices = 3) out;

 void main(){
	tcPos[gl_InvocationID] = vPos[gl_InvocationID];

	if(gl_InvocationID == 0){
		gl_TessLevelOuter[0] = tessellation_level;
		gl_TessLevelOuter[1] = tessellation_level;
		gl_TessLevelOuter[2] = tessellation_level;
		gl_TessLevelInner[0] = tessellation_level;
	}
 }