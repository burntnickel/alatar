#version 400 core

// OpenGL classic sprite vertex shader for Alatar

// Per vertex inputs
// ----------------------------------
in vec2 in_pos;
in vec2 in_uv;

// Outputs
// ----------------------------------
out vec4 shader_coord;
out vec2 UV;

void main() {
    gl_Position = vec4(in_pos.x, in_pos.y, 0.0, 1.0);
    shader_coord = gl_Position;
    UV = in_uv;
}

