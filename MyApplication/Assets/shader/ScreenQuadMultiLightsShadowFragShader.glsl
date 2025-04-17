#version 330 core

#define MAX_LIGHTS 16

struct Light {
    vec3 position;
    vec3 color;
};

out vec4 frag_color;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gObjColor;
uniform sampler2D gAlbedoSpec;

uniform float k_ambient = 0.1f;
uniform float shininess = 16.0f;
uniform vec3 camera_position;

uniform int lightNum;
uniform Light Lights[MAX_LIGHTS];
uniform mat4 lightSpaceVP[MAX_LIGHTS];
uniform sampler2D ShadowMap[MAX_LIGHTS];


float computeShadow(int i, vec3 fragPos) {
    vec4 FragPosLightSpace = lightSpaceVP[i] * vec4(fragPos, 1.0);
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
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

    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoord).rgb);

    // Sample material textures
    vec4 texColor = texture(gObjColor, TexCoord);
    vec3 kd = texColor.rgb;
    float alpha = texColor.a;

    if (alpha < 0.1)
        discard;

    float ks = texture(gAlbedoSpec, TexCoord).r;

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

        float shadow = computeShadow(i, FragPos);

        result += ambient + (1.0 - shadow) * (diffuse + specular);
    }

    // Modulate lighting with diffuse texture
    frag_color = vec4(result * kd, 1.0);
}