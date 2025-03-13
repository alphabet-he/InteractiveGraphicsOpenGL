#version 330 core

out vec4 frag_color;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D screen_texture; // Rendered texture

uniform vec3 light_position;
uniform vec3 camera_position;

uniform vec3 light_color = vec3(1.0, 1.0, 1.0);
uniform vec3 object_color = vec3(0.9, 0.9, 0.9);

uniform float k_ambient = 0.05;
uniform float k_diffuse = 0.5;
uniform float k_specular = 5.0; 
uniform float shininess = 128.0;

uniform float object_reflect_ratio = 0.8;
uniform float reflect_ratio = 0.8;
uniform samplerCube skybox;

void main() {

	// render to texture
    vec4 screen_tex_color = texture(screen_texture, TexCoords);

    vec3 norm = normalize(Normal);
	vec3 L = normalize(light_position - FragPos); // light direction
	vec3 V = normalize(camera_position - FragPos); // view direction
	vec3 H = normalize(L + V); // half way vector

	// reflection
	vec3 reflect_dir = reflect(-V, norm);
    vec4 env_reflected_color = texture(skybox, reflect_dir);

	vec4 combined_reflection = mix(env_reflected_color, screen_tex_color, object_reflect_ratio);

	vec3 ambient = k_ambient * light_color;
	vec3 diffuse = k_diffuse * light_color * max(0, dot(norm, L));
	vec3 specular = k_specular * light_color * pow(max(0, dot(norm, H)), shininess);
	vec3 blinn_color = (ambient + diffuse + specular) * object_color;


	if (dot(norm, V) <= 0.0) {
		vec3 final_color = mix(blinn_color, env_reflected_color.rgb, reflect_ratio);
		frag_color = vec4(final_color, 1.0);
	}
	else {
		vec3 final_color = mix(blinn_color, combined_reflection.rgb, reflect_ratio);
		frag_color = vec4(final_color, 1.0);
	}
}