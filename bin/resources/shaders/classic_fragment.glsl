#version 400 core

// OpenGL fragment shader for Alatar classic graphics mode

// Uniforms
// ----------------------------------
uniform usamplerBuffer u_tile_buffer;
uniform usamplerBuffer u_color_buffer;
uniform usamplerBuffer u_tileset_buffer;
uniform usamplerBuffer u_clut_buffer;

// Inputs
// ----------------------------------
in vec4 shader_coord;

// Outputs
// ----------------------------------
out vec4 frag_color;

// Named constants
const int kTilesInRow = 40;
const int kTilesInColumn = 25;
const int kTilePixelWidth = 8;
const int kTilePixelHeight = 8;

void main() {
    // Normalized and flipped coordinates
    float xx = (shader_coord.x + 1.0) / 2.0;
    float yy = (-1.0 * shader_coord.y + 1.0) / 2.0;

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

    //vec4 local_color = vec4(tile_index/255.0, color_index.r/15.0, 0.0, 1.0);
    vec4 local_color = texelFetch(u_clut_buffer, color_index);
    //local_color.g=0;
    //local_color.b=0;

    frag_color = (local_color / 255.0) * (pixel_val / 255.0);
}


