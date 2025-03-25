#version 400 core
 
 in vec3 vPos[];
 in vec3 vNormal[];
 in vec2 vTexCoord[];
 in vec3 vTangent[];

 out vec3 tcPos[];
 out vec3 tcNormal[];
 out vec2 tcTexCoord[];
 out vec3 tcTangent[];

 uniform int tessellation_level = 4;

 layout(vertices = 3) out;

 void main(){
	tcPos[gl_InvocationID] = vPos[gl_InvocationID];
	tcTexCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];
	tcTangent[gl_InvocationID] = vTangent[gl_InvocationID];
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];

	if(gl_InvocationID == 0){
		gl_TessLevelOuter[0] = tessellation_level;
		gl_TessLevelOuter[1] = tessellation_level;
		gl_TessLevelOuter[2] = tessellation_level;
		gl_TessLevelInner[0] = tessellation_level;
	}
 }