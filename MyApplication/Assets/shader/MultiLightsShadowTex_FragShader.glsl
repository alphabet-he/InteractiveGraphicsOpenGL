#version 330 core

#define MAX_LIGHTS 16

struct Light {
    vec3 position;
    vec3 color;
};

out vec4 frag_color;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace[MAX_LIGHTS];

uniform vec3 camera_position;

uniform float k_ambient = 0.1;
uniform float shininess = 64.0;

uniform sampler2D texture_Kd;
uniform sampler2D texture_Ks;

uniform int lightNum;
uniform Light Lights[MAX_LIGHTS];
uniform sampler2D ShadowMap[MAX_LIGHTS];


float computeShadow(int i) {
    vec3 projCoords = FragPosLightSpace[i].xyz / FragPosLightSpace[i].w;
    projCoords = projCoords * 0.5 + 0.5;

    // depth comparison
    float currentDepth = projCoords.z;
    float closestDepth = texture(ShadowMap[i], projCoords.xy).r;

    float bias = 0.005;
    float shadow = currentDepth > closestDepth + bias ? 1.0 : 0.0;

    // optional: discard out of light frustum
    if (projCoords.z > 1.0) shadow = 0.0;

    return shadow;
}

void main() {

    // Sample material textures
    vec4 texColor = texture(texture_Kd, TexCoord);
    vec3 kd = texColor.rgb;
    float alpha = texColor.a;

    if (alpha < 0.1)
        discard;

    vec3 ks = texture(texture_Ks, TexCoord).rgb;

    vec3 norm = normalize(Normal);
    vec3 V = normalize(camera_position - FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < lightNum; ++i) {
        vec3 L = normalize(Lights[i].position - FragPos);
        vec3 H = normalize(L + V);

        // Blinn–Phong components
        vec3 ambient = k_ambient * Lights[i].color;
        float diff = max(dot(norm, L), 0.0);
        float spec = pow(max(dot(norm, H), 0.0), shininess);

        vec3 diffuse  = diff * Lights[i].color;
        vec3 specular = spec * ks * Lights[i].color;

        float shadow = computeShadow(i);

        result += ambient + (1.0 - shadow) * (diffuse + specular);
    }

    // Modulate lighting with diffuse texture
    frag_color = vec4(result * kd, 1.0);
    //frag_color = texture(texture_Kd, TexCoord);
}