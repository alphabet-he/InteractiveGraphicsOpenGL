#version 330 core

out vec4 frag_color;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform vec3 light_color = vec3(1.0, 1.0, 1.0);
uniform vec3 object_color = vec3(0.9, 0.9, 0.9);

uniform float k_ambient = 0.1;
uniform float k_diffuse = 0.5;
uniform float k_specular = 3.0; 
uniform float shininess = 64.0;

uniform sampler2D shadow_map;

float ComputeShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float shadowDepth = texture(shadow_map, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    float shadow = currentDepth > shadowDepth + 0.005 ? 1.0 : 0.0; 
    return shadow;
}

void main() {

    vec3 norm = normalize(Normal);
	vec3 L = normalize(light_position - FragPos); // light direction
	vec3 V = normalize(camera_position - FragPos); // view direction
	vec3 H = normalize(L + V); // half way vector

	vec3 ambient = k_ambient * light_color;
	vec3 diffuse = k_diffuse * light_color * max(0, dot(norm, L));
	vec3 specular = k_specular * light_color * pow(max(0, dot(norm, H)), shininess);

	float shadow = ComputeShadow(FragPosLightSpace);
	vec3 blinn_color = ambient + (1.0-shadow) * (diffuse + specular);
	blinn_color *= object_color;

	frag_color = vec4(blinn_color, 1.0);
}