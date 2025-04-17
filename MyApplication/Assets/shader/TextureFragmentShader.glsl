#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 frag_color;

uniform sampler2D texture_Kd;

void main(){

	vec4 texColor = texture(texture_Kd, TexCoord);
	vec3 object_color = texColor.rgb;
	float alpha = texColor.a;
	if (alpha < 0.1)
    discard;

	frag_color = vec4(object_color, 1.0);
}