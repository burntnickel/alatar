#version 400 core

// OpenGL tile fragment shader for Alatar updated graphics mode

// Uniforms
// ----------------------------------
uniform sampler2D u_wall_texture;
uniform sampler2DArray u_tile_mask_texture;
uniform usamplerBuffer u_clut_buffer;

// Inputs
// ----------------------------------
in vec2 UV;
in vec4 shader_coord;

// Outputs
// ----------------------------------
out vec4 frag_color;

// Named constants

void main() {
    // Move these to the vertex shader?
    float u_global = (shader_coord.x + 1.0) / 2.0;
    float v_global = (-1.0 * shader_coord.y + 1.0) / 2.0;

    float u_local = UV.x;
    float v_local = UV.y;

    //frag_color = vec4(u_global, v_global, 0.0, 1.0);
    //frag_color = vec4(UV.x, UV.y, 0.0, 1.0);
    vec4 tmp_color = texture(u_wall_texture, vec2(u_global, v_global));
    vec4 mask = texture(u_tile_mask_texture, vec3(u_local, v_local, 20));

    float gray = 0.2126 * tmp_color.r + 0.7152 *tmp_color.g + 0.0722 * tmp_color.b;

    //vec4 local_color = texelFetch(u_clut_buffer, color_index);
    vec4 local_color = texelFetch(u_clut_buffer, 3);
    frag_color = local_color / 255.0;

    frag_color = vec4(gray, gray, gray, 1.0) * mask.rrrr * frag_color;
    //frag_color = tmp_color;
}


