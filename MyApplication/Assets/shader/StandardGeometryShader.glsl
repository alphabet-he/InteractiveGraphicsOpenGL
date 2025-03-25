#version 400 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

uniform float z_offset = 0.1;

void emit_edge(int i, int j) {
    vec4 p1 = gl_in[i].gl_Position;
    vec4 p2 = gl_in[j].gl_Position;

    // Move closer to the camera to prevent z-fighting
    p1.z -= z_offset;
    p2.z -= z_offset;

    gl_Position = p1;
    EmitVertex();

    gl_Position = p2;
    EmitVertex();

    EndPrimitive(); // finish the line
}

void main() {
    emit_edge(0, 1); 
    emit_edge(1, 2);
    emit_edge(2, 0);
}