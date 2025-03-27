#version 400 core

layout(triangles, equal_spacing, ccw) in;

 in vec3 tcPos[];
 in vec3 tcNormal[];
 in vec2 tcTexCoord[];
 in vec3 tcTangent[];

out vec2 FragTexCoord;
out vec3 FragPos;
out mat3 TBN;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceVP;

uniform sampler2D displacementMap;
uniform float displacementScale = 0.1;

void main(){
	vec3 pos = tcPos[0] * gl_TessCoord.x + tcPos[1] * gl_TessCoord.y + tcPos[2] * gl_TessCoord.z;
	vec2 uv = tcTexCoord[0] * gl_TessCoord.x + tcTexCoord[1] * gl_TessCoord.y + tcTexCoord[2] * gl_TessCoord.z;
	vec3 normal = normalize(tcNormal[0] * gl_TessCoord.x + tcNormal[1] * gl_TessCoord.y + tcNormal[2] * gl_TessCoord.z);
	vec3 tangent = normalize(tcTangent[0] * gl_TessCoord.x + tcTangent[1] * gl_TessCoord.y + tcTangent[2] * gl_TessCoord.z);
	vec3 bitangent = normalize(cross(normal, tangent));

	float height = texture(displacementMap, uv).r;
	pos += normal * height * displacementScale;

	FragTexCoord = uv;
	FragPos = vec3(model * vec4(pos, 1.0));
	FragPosLightSpace = lightSpaceVP * model * vec4(pos, 1.0);
	TBN = mat3(normalize(mat3(model) * tangent), normalize(mat3(model) * bitangent), normalize(mat3(model) * normal));

	gl_Position = projection * view * model * vec4(pos, 1.0);
}