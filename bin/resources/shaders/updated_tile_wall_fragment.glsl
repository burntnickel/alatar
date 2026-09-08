#version 400 core

// OpenGL tile fragment shader for Alatar updated graphics mode wall tiles/masks

// Uniforms
// ----------------------------------
uniform usamplerBuffer u_color_buffer;
uniform usamplerBuffer u_clut_buffer;
uniform usamplerBuffer u_tile_buffer;
uniform sampler2DArray u_tile_mask_texture;
uniform sampler2D u_wall_texture;

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

float ToGreyscale(vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

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

    // Get wall mask index from the tile buffer & wall texture data
    vec4 mask_index4 = texelFetch(u_tile_buffer, tile_buffer_index);
    int mask_index = int(mask_index4.r);

    vec4 tmp_color = texture(u_wall_texture, vec2(u_global, v_global));
    vec4 mask = texture(u_tile_mask_texture, vec3(u_local, v_local, mask_index));

    //float gray = 0.2126 * tmp_color.r + 0.7152 *tmp_color.g + 0.0722 * tmp_color.b;
    float gray = ToGreyscale(tmp_color.rgb);

    // Get color index from the color buffer
    vec4 color_index4 = texelFetch(u_color_buffer, tile_buffer_index);
    int color_index = int(color_index4.r);

    vec4 local_color = texelFetch(u_clut_buffer, color_index);
    frag_color = local_color / 255.0;

    //frag_color = vec4(gray, gray, gray, 1.0) * mask.rrrr * frag_color;
    frag_color.rgb = pow(vec3(gray, gray, gray) * mask.rgb * frag_color.rgb, vec3(1.0 / 2.2));
    frag_color.a = mask.a;
}


