#version 400 core

// OpenGL updated vertex shader for Alatar

// Uniforms
// ----------------------------------
uniform mat4 u_view_matrix;

// Per vertex inputs
// ----------------------------------
in vec2 in_pos;
in vec2 in_uv;

// Outputs
// ----------------------------------
out vec4 shader_coord;
out vec2 UV;

void main() {
    vec4 position4 = vec4(in_pos.x, in_pos.y, 0.0, 1.0);
    gl_Position = u_view_matrix * position4;
    //gl_Position = 2.0 * u_view_matrix * vec4(in_pos.x, in_pos.y, 0.0, 1.0) - vec4(1.0, 1.0, 0.0, 0.0);
    //gl_Position.w = 1.0;
    //shader_coord = gl_Position;
    shader_coord = position4;
    UV = in_uv;
}
