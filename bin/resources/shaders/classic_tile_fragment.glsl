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
uniform usamplerBuffer u_tile_buffer;
uniform usamplerBuffer u_color_buffer;
uniform usamplerBuffer u_tileset_buffer;
uniform usamplerBuffer u_clut_buffer;

// Inputs
// ----------------------------------
in vec2 UV;

// Outputs
// ----------------------------------
out vec4 frag_color;

// Named constants
const int kTilesInRow = 40;
const int kTilesInColumn = 25;
const int kTilePixelWidth = 8;
const int kTilePixelHeight = 8;

void main() {
    float xx = UV.x;
    float yy = 1.0 - UV.y;

    // Determine tile coordinates and index into the tile buffer
    int tile_x = int(xx * kTilesInRow);
    int tile_y = int(yy * kTilesInColumn);
    int tile_buffer_index = tile_x + tile_y * kTilesInRow;

    // Get tile id from the tile buffer
    vec4 tile_index4 = texelFetch(u_tile_buffer, tile_buffer_index);
    int tile_index = int(tile_index4.r);

    // Get color index from the color buffer
    vec4 color_index4 = texelFetch(u_color_buffer, tile_buffer_index);
    int color_index = int(color_index4.r);

    // Which pixel and offset within the tile set
    int pixel_x = int((kTilesInRow * xx - tile_x) * kTilePixelWidth);
    int pixel_y = int((kTilesInColumn * yy - tile_y) * kTilePixelHeight);
    int pixel_offset = pixel_x + kTilePixelHeight * pixel_y;
    int tileset_index = kTilePixelWidth * kTilePixelHeight * tile_index + pixel_offset;
    vec4 pixel_val4 = texelFetch(u_tileset_buffer, tileset_index);
    float pixel_val = pixel_val4.r;

    vec4 local_color = texelFetch(u_clut_buffer, color_index);

    frag_color = (local_color / 255.0) * (pixel_val / 255.0);
}


