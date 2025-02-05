#version 330 core

in vec3 vertex_normal_out;
in vec3 vertex_position_out;
out vec4 frag_color;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform vec3 light_color = vec3(1.0, 1.0, 1.0);
uniform vec3 object_color = vec3(0.8, 0.0, 0.0);

uniform float k_ambient = 0.1;
uniform float k_diffuse = 1.0;
uniform float k_specular = 0.5;
uniform float shininess = 32.0;

void main(){
	vec3 L = normalize(light_position - vertex_position_out); // light direction
	vec3 V = normalize(camera_position - vertex_position_out); // view direction
	vec3 H = normalize(L + V); // half way vector

	vec3 ambient = k_ambient * light_color;
	vec3 diffuse = k_diffuse * light_color * max(0, dot(vertex_normal_out, L));
	vec3 specular = k_specular * light_color * pow(max(0, dot(vertex_normal_out, H)), shininess);

	vec3 final_color = (ambient + diffuse + specular) * object_color;
	frag_color = vec4(final_color, 1.0);
}