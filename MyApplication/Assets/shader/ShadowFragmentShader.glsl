#version 330 core

out vec4 frag_color;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform vec3 light_color = vec3(1.0, 1.0, 1.0);
uniform vec3 object_color = vec3(0.9, 0.9, 0.9);

uniform float k_ambient = 0.1;
uniform float k_diffuse = 0.5;
uniform float k_specular = 5.0; 
uniform float shininess = 128.0;

void main() {


    vec3 norm = normalize(Normal);
	vec3 L = normalize(light_position - FragPos); // light direction
	vec3 V = normalize(camera_position - FragPos); // view direction
	vec3 H = normalize(L + V); // half way vector

	vec3 ambient = k_ambient * light_color;
	vec3 diffuse = k_diffuse * light_color * max(0, dot(norm, L));
	vec3 specular = k_specular * light_color * pow(max(0, dot(norm, H)), shininess);
	vec3 blinn_color = (ambient + diffuse + specular) * object_color;

	frag_color = vec4(blinn_color, 1.0);
}