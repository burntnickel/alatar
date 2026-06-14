#version 400 core

// OpenGL fragment shader for Alatar classic graphics mode

// Uniforms
// ----------------------------------
uniform usamplerBuffer u_tile_buffer;
uniform usamplerBuffer u_color_buffer;

// Inputs
// ----------------------------------
in vec4 shader_coord;

// Outputs
// ----------------------------------
out vec4 frag_color;

// Named constants
const int kTilesInRow = 40;
const int kTilesInColumn = 25;

void main() {
    // Normalized and flipped coordinates
    float xx = (shader_coord.x + 1.0) / 2.0;
    float yy = (-1.0 * shader_coord.y + 1.0) / 2.0;

    // Determine tile coordinates and index into the tile buffer
    int tile_x = int(xx * kTilesInRow);
    int tile_y = int(yy * kTilesInColumn);
    int tile_buffer_index = tile_x + tile_y * kTilesInRow;

    // Get tile id from the tile buffer
    vec4 tile_index = texelFetch(u_tile_buffer, tile_buffer_index);

    // Get color index from the color buffer
    vec4 color_index = texelFetch(u_color_buffer, tile_buffer_index);



    //vec4 raw_color = texelFetch(u_clut, color_index);

    //vec4 local_color = raw_color / 255.0;
    //local_color.a = 0.0;

    //frag_color = local_color;
    //frag_color = vec4(1.0, 0.5, 0.25, 1.0);
 
    

    //int pixel_x = int((kTilesInRow * xx - tile_x) * 8);
    //int pixel_y = int((kTilesInColumn * yy - tile_y) * 8);
    //int pixel_offset = pixel_x + 8 * pixel_y;
    //int pixel_idx = 64 * tile_idx + pixel_offset;

    //frag_color = vec4(shader_coord.x, shader_coord.y, shader_coord.z, 1.0);
    //frag_color = vec4(qq.a/255.0, tile_x/40.0, tile_y/25.0, 1.0);
    frag_color = vec4(0.0001*tile_index.r/255.0, color_index.r/15.0, 0.0, 1.0);
}


