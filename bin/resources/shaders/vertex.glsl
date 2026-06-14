#version 400 core

// OpenGL vertex shader for BeforeDawn frame buffer

// Per vertex inputs
// ----------------------------------
in vec2 v_pos;

// Outputs
// ----------------------------------
out vec4 shader_coord;

void main() {
    gl_Position = vec4(v_pos.x, v_pos.y, 0.0, 1.0);
    shader_coord = gl_Position;
}

