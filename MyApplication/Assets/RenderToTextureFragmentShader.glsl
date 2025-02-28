#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture; // Rendered texture

void main() {
    vec4 texColor = texture(screenTexture, TexCoords);
    if (texColor.a == 0.0) { // If the texture is empty, show magenta
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        FragColor = texColor; // Otherwise, show the texture
    }
}