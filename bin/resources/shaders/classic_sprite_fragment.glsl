#version 400 core

// OpenGL tile fragment shader for Alatar classic graphics mode

// Uniforms
// ----------------------------------
uniform usamplerBuffer u_sprite_buffer;
uniform usamplerBuffer u_clut_buffer;
uniform int u_sprite_index;
uniform int u_colors[4] = int[](0,1,2,3);

// Inputs
// ----------------------------------
in vec2 UV;

// Outputs
// ----------------------------------
out vec4 frag_color;

// Named constants
const int kWidthPixels = 2 * 24;
const int kHeightPixels = 2 * 21;
const int kStride = kWidthPixels * kHeightPixels;

void main() {
    // Pixel coordinates
    int pixel_x = int(kWidthPixels * UV.x);
    int pixel_y = int(kHeightPixels * UV.y);

    // Get pixel value (color index)
    int sprite_buffer_index = pixel_x + pixel_y * kWidthPixels + u_sprite_index * kStride;
    vec4 pixel_index4 = texelFetch(u_sprite_buffer, sprite_buffer_index);

    int color_idx = u_colors[int(pixel_index4.r)];

    vec4 local_color = texelFetch(u_clut_buffer, color_idx);

    frag_color.r = local_color.r / 255.0;
    frag_color.g = local_color.g / 255.0;
    frag_color.b = local_color.b / 255.0;
    frag_color.a = float(color_idx != 0);
}


