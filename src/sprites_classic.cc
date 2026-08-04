#include <set>

#include "load_data.h"
#include "sprites_classic.h"

namespace alatar_classic {

static constexpr unsigned int kSpriteWidth = 24;
static constexpr unsigned int kSpriteWidthBytes = kSpriteWidth / 8;
static constexpr unsigned int kSpriteHeight = 21;
static constexpr unsigned int kProcWidth = 2 * kSpriteWidth;
static constexpr unsigned int kSpriteStride = 64;
static constexpr unsigned int kSpriteProcStride = 2 * kSpriteWidth * 2 * kSpriteHeight;

bool LoadSpriteData(std::filesystem::path path_and_name,
                    std::span<unsigned char, kSpriteProcDataSize> proc_sprite_data) {
  // Need to skip the load address at the beginning of the data file
  constexpr unsigned int kLoadAddressOffset = 2;

  // These are the sprites that are not multi-color
  const std::set<unsigned int> kSpriteHiResIndexes{46, 55, 67, 68,  69,  70,  71, 72,
                                                   73, 80, 83, 102, 115, 116, 117};

  std::array<unsigned char, kSpriteSetSize> buffer;

  bool success = alatar::LoadData(path_and_name, buffer, kLoadAddressOffset);

  if (!success) {
    return false;
  }

  enum : unsigned char {
    kSpriteBackground = 0,
    ckSpriteSharedColor1 = 1,
    kSpriteColor = 2,
    kSpriteSharedColor2 = 3
  };

  // Clear sprite data
  for (std::size_t ii = 0; ii < proc_sprite_data.size(); ++ii) {
    proc_sprite_data[ii] = 0;
  }

  // Base sprites
  for (unsigned int i_sprite = 0; i_sprite < kNumSprites; ++i_sprite) {
    std::size_t sprite_base = i_sprite * kSpriteStride;
    std::size_t sprite_proc_base = i_sprite * kSpriteProcStride;

    for (unsigned int xx = 0; xx < kSpriteWidth; ++xx) {
      for (unsigned int yy = 0; yy < kSpriteHeight; ++yy) {
        unsigned int sprite_col_byte = xx / 8;

        std::size_t sprite_byte_index = sprite_base + yy * kSpriteWidthBytes + xx / 8;
        std::size_t proc_byte_index = sprite_proc_base + yy * kProcWidth + xx;

        unsigned char byte_in = buffer[sprite_byte_index];

        unsigned int sprite_bit;

        if (kSpriteHiResIndexes.contains(i_sprite)) {
          sprite_bit = 7 - (xx - 8 * sprite_col_byte);

          if ((1 << sprite_bit) & byte_in) {
            proc_sprite_data[proc_byte_index] = kSpriteColor;
          } else {
            proc_sprite_data[proc_byte_index] = kSpriteBackground;
          }
        } else {
          sprite_bit = 2 * ((7 - (xx - 8 * sprite_col_byte)) / 2);
          proc_sprite_data[proc_byte_index] = (byte_in >> sprite_bit) & 3;
        }
      }
    }
  }

  // Create mirror images versions
  for (unsigned int i_sprite = 0; i_sprite < kNumSprites; ++i_sprite) {
    std::size_t sprite_base_orig = i_sprite * kSpriteProcStride;
    std::size_t sprite_base_flip = i_sprite * kSpriteProcStride + kNumSprites * kSpriteProcStride;

    for (unsigned int xx = 0; xx < kSpriteWidth; ++xx) {
      unsigned int xx_flip = kSpriteWidth - xx - 1;

      for (unsigned int yy = 0; yy < kSpriteHeight; ++yy) {
        std::size_t orig_byte_index = sprite_base_orig + yy * kProcWidth + xx;
        std::size_t flip_byte_index = sprite_base_flip + yy * kProcWidth + xx_flip;

        proc_sprite_data[flip_byte_index] = proc_sprite_data[orig_byte_index];
      }
    }
  }

  // Create 2xX versions
  for (unsigned int i_sprite = 0; i_sprite < 2 * kNumSprites; ++i_sprite) {
    std::size_t sprite_base_orig = i_sprite * kSpriteProcStride;
    std::size_t sprite_base_expand = i_sprite * kSpriteProcStride + 2 * kNumSprites * kSpriteProcStride;

    for (unsigned int xx = 0; xx < kSpriteWidth; ++xx) {
      unsigned int xx_expand = 2 * xx;

      for (unsigned int yy = 0; yy < kSpriteHeight; ++yy) {
        std::size_t orig_byte_index = sprite_base_orig + yy * kProcWidth + xx;
        std::size_t expand_byte_index = sprite_base_expand + yy * kProcWidth + xx_expand;

        proc_sprite_data[expand_byte_index] = proc_sprite_data[orig_byte_index];
        proc_sprite_data[expand_byte_index + 1] = proc_sprite_data[orig_byte_index];
      }
    }
  }

  // Create 2XY versions & 2xX & 2xY versions
  for (unsigned int i_sprite = 0; i_sprite < 4 * kNumSprites; ++i_sprite) {
    std::size_t sprite_base_orig = i_sprite * kSpriteProcStride;
    std::size_t sprite_base_expand = i_sprite * kSpriteProcStride + 4 * kNumSprites * kSpriteProcStride;

    for (unsigned int xx = 0; xx < 2 * kSpriteWidth; ++xx) {
      for (unsigned int yy = 0; yy < kSpriteHeight; ++yy) {
        unsigned int yy_expand = 2 * yy;

        std::size_t orig_byte_index = sprite_base_orig + yy * kProcWidth + xx;
        std::size_t expand_byte_index1 = sprite_base_expand + yy_expand * kProcWidth + xx;
        std::size_t expand_byte_index2 = sprite_base_expand + (yy_expand + 1) * kProcWidth + xx;

        proc_sprite_data[expand_byte_index1] = proc_sprite_data[orig_byte_index];
        proc_sprite_data[expand_byte_index2] = proc_sprite_data[orig_byte_index];
      }
    }
  }

  return true;
}

void PaintClassicSprite(BurningLogic::ShaderVars shader_vars, BurningLogic::mat4& view_matrix,
                        SDL_FRect sdl_rect, ClassicSpriteGLBuffers gl_buffers, int id,
                        std::array<int, 3> colors, int modifier) {
  std::array<GLfloat, 16> vertex_buffer = {{sdl_rect.x, sdl_rect.y, 0.0, 0.0, sdl_rect.x + sdl_rect.w,
                                            sdl_rect.y, 1.0, 0.0, sdl_rect.x, sdl_rect.y + sdl_rect.h, 0.0,
                                            1.0, sdl_rect.x + sdl_rect.w, sdl_rect.y + sdl_rect.h, 1.0, 1.0}};
  std::array<GLuint, 4> index_buffer = {{0, 1, 2, 3}};

  std::array<GLint, 4> color_index_array = {{0, 0, 0, 0}};

  for (std::size_t ii = 1; ii < 4; ++ii) {
    color_index_array[ii] = colors[ii - 1];
  }

  // Update sprite index based on modifiers
  if (modifier & kSpriteFlipX) {
    id = id + static_cast<int>(kNumSprites);
  }

  if (modifier & kSpriteExpandX) {
    id = id + 2 * static_cast<int>(kNumSprites);
  }

  if (modifier & kSpriteExpandY) {
    id = id + 4 * static_cast<int>(kNumSprites);
  }

  // Bind program
  glUseProgram(shader_vars.program);
  BurningLogic::PrintGLError("PaintClassicSprites:glUseProgram");

  glBindVertexArray(shader_vars.vao);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindVertexArray");

  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindBuffer");

  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertex_buffer.size() * sizeof(GLfloat)),
               vertex_buffer.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintClassicSprites:glBufferData");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindBuffer");

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_buffer.size() * sizeof(GLuint)),
               index_buffer.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintClassicSprites:glBufferData");

  // Enable vertex position
  GLuint in_pos_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "in_pos");
  glEnableVertexAttribArray(in_pos_location);
  BurningLogic::PrintGLError("PaintClassicSprites:glEnableVertexAttribArray");

  // Enable vertex texture coordinates
  GLuint in_uv_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "in_uv");
  glEnableVertexAttribArray(in_uv_location);
  BurningLogic::PrintGLError("PaintClassicSprites:glEnableVertexAttribArray");

  // Set vertex data
  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindBuffer");

  glVertexAttribPointer(in_pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
  BurningLogic::PrintGLError("PaintClassicSprites:glVertexAttribPointer");

  glVertexAttribPointer(in_uv_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (void*)(2 * sizeof(GLfloat)));
  BurningLogic::PrintGLError("PaintClassicSprites:glVertexAttribPointer");

  // Set sprite index uniform
  GLint sprite_index_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_sprite_index");
  glUniform1i(sprite_index_location, id);
  BurningLogic::PrintGLError("PaintClassicSprites:glUniform1i");

  // Set color mapping array
  GLint color_map_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_colors");
  glUniform1iv(color_map_location, color_index_array.size(), color_index_array.data());
  BurningLogic::PrintGLError("PaintClassicSprites:glUniform1i");

  // Texture stuff (sprite buffer)
  GLint tilebuffer_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_sprite_buffer");
  glUniform1i(tilebuffer_location, 4);  // TODO: how do I map these globally to GL_TEXTURE0 etc?
  BurningLogic::PrintGLError("PaintClassicSprites:glUniform1i");

  // Texture stuff (clut)
  GLint clut_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_clut_buffer");
  glUniform1i(clut_location, 3);
  BurningLogic::PrintGLError("PaintClassicSprites:glUniform1i");

  // View Matrix
  GLint view_mat_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_view_matrix");
  glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_matrix.data());
  BurningLogic::PrintGLError("PaintClassicTiles:glUniformMatrix4fv");

  // TODO: do I need these?
  //  ------
  glActiveTexture(GL_TEXTURE4);  // TODO: capture these inthe buffer struct
  BurningLogic::PrintGLError("PaintClassicSprites:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_sprite_buffer);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE3);
  BurningLogic::PrintGLError("PaintClassicSprites:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_clut_buffer);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindTexture");

  // Set index data and render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindBuffer");

  glDrawElements(GL_TRIANGLE_STRIP, 2 * 2, GL_UNSIGNED_INT, NULL);
  BurningLogic::PrintGLError("PaintClassicSprites:glDrawElements");

  // Disable vertex attributes
  glDisableVertexAttribArray(in_pos_location);
  glDisableVertexAttribArray(in_uv_location);

  // Unbind program
  glUseProgram(0);
}

void DrawClassicSprite(BurningLogic::ShaderVars shader_vars, BurningLogic::mat4& view_matrix,
                       ClassicSpriteGLBuffers gl_buffers, int id, int x, int y, std::array<int, 3> colors,
                       int modifier) {
  constexpr int screen_width_pixels = 320;  // TODO: move stuff like this into a common header
  constexpr int screen_height_pixels = 200;
  constexpr int sprite_width_1x = 24;
  constexpr int sprite_height_1x = 21;
  constexpr int sprite_width_draw = 2 * sprite_width_1x;
  constexpr int sprite_height_draw = 2 * sprite_height_1x;

  SDL_FRect rect;

  rect.x = -1.0f + 2.0f * (static_cast<float>(x) / static_cast<float>(screen_width_pixels));
  rect.y = 1.0f - 2.0f * (static_cast<float>(y) / static_cast<float>(screen_height_pixels));
  rect.w = 2.0f * static_cast<float>(sprite_width_draw) / static_cast<float>(screen_width_pixels);
  rect.h = -2.0f * static_cast<float>(sprite_height_draw) / static_cast<float>(screen_height_pixels);

  PaintClassicSprite(shader_vars, view_matrix, rect, gl_buffers, id, colors, modifier);
}

// Same as DrawClassicSprite but uses the Commorode 64 coordinates
void DrawClassicSpriteC64(BurningLogic::ShaderVars shader_vars, BurningLogic::mat4& view_matrix,
                          ClassicSpriteGLBuffers gl_buffers, int id, int x, int y, std::array<int, 3> colors,
                          int modifier) {
  DrawClassicSprite(shader_vars, view_matrix, gl_buffers, id, x - 24, y - 50, colors, modifier);
}

}  // namespace alatar_classic
