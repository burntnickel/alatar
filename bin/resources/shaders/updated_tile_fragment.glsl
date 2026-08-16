#version 400 core

// OpenGL tile fragment shader for Alatar updated graphics mode

// Uniforms
// ----------------------------------

// Inputs
// ----------------------------------
in vec2 UV;
in vec4 shader_coord;

// Outputs
// ----------------------------------
out vec4 frag_color;

// Named constants

void main() {
    float u_global = (shader_coord.x + 1.0) / 2.0;
    float v_global = (-1.0 * shader_coord.y + 1.0) / 2.0;

    frag_color = vec4(u_global, v_global, 0.0, 1.0);
    //frag_color = vec4((shader_coord.x + 1.0) / 2.0, (shader_coord.y + 1.0) / 2.0, 0.0, 1.0);
    //frag_color = vec4(UV.x, UV.y, 0.0, 1.0);
}


