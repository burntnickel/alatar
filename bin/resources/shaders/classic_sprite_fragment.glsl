#version 400 core

// Copyright 2026 Jude Giampaolo
//
// This file is part of Alatar.
//
// Alatar is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Alatar is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Alatar.
// If not, see <https://www.gnu.org/licenses/>. 

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


