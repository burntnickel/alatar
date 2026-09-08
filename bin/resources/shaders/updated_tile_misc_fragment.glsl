#version 400 core

// OpenGL tile fragment shader for Alatar updated graphics mode misc tiles
// This includes: ladders

// Uniforms
// ----------------------------------
uniform usamplerBuffer u_color_buffer;
uniform usamplerBuffer u_clut_buffer;
uniform usamplerBuffer u_tile_buffer;
uniform sampler2DArray u_tile_mask_texture;

// Inputs
// ----------------------------------
in vec2 UV;
in vec4 shader_coord;

// Outputs
// ----------------------------------
out vec4 frag_color;

// Named constants
const int kTilesInRow = 40;
const int kTilesInColumn = 25;
const int kMiscBufferOffset = 1000;

float ToGreyscale(vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

/*vec3 Desaturate(vec3 color, float gray, float threshold) {
    return mix(vec3(gray), color, threshold);
}*/

void main() {
    // Move these to the vertex shader?
    float u_global = (shader_coord.x + 1.0) / 2.0;
    float v_global = (-1.0 * shader_coord.y + 1.0) / 2.0;

    float u_local = UV.x;
    float v_local = UV.y;

    // Determine tile coordinates and index into the tile buffer
    int tile_x = int(u_global * kTilesInRow);
    int tile_y = int(v_global * kTilesInColumn);
    int tile_buffer_index = tile_x + tile_y * kTilesInRow;

    // Get misc tile index from the tile buffer
    vec4 tile_index4 = texelFetch(u_tile_buffer, tile_buffer_index + kMiscBufferOffset);
    int tile_index = int(tile_index4.r);
    vec4 tile_data = texture(u_tile_mask_texture, vec3(u_local, v_local, tile_index));

    //float gray = tile_data.r;
    float gray = ToGreyscale(tile_data.rgb);

    // Get color index from the color buffer
    vec4 color_index4 = texelFetch(u_color_buffer, tile_buffer_index);
    int color_index = int(color_index4.r);

    vec4 local_color = texelFetch(u_clut_buffer, color_index);
    frag_color = local_color / 255.0;

    frag_color = vec4(gray, gray, gray, 1.0) * frag_color;
    //frag_color.rgb = pow(vec3(gray, gray, gray) * frag_color.rgb, vec3(1.0 / 2.2)); // Do I want this or not?
    //frag_color.rgb = Desaturate(frag_color.rgb, gray, 0.7);
    frag_color.a = tile_data.a;
}


