#include <set>

#include "load_data.h"
#include "sprites_classic.h"

namespace sprites_classic {

enum SpriteModification { kNone = 0, kReverse = 1, kExpandX = 2, kExpandY = 4 };

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

  constexpr unsigned int kSpriteStride = 64;
  constexpr unsigned int kSpriteProcStride = kSpriteStride * 8 * 2 * 2;
  constexpr unsigned int kSpriteWidth = 24;
  constexpr unsigned int kSpriteWidthBytes = kSpriteWidth / 8;
  constexpr unsigned int kSpriteHeight = 21;
  constexpr unsigned int kProcWidth = 2 * kSpriteWidth;

  // Base sprites
  for (unsigned int i_sprite = 0; i_sprite < kNumSprites; ++i_sprite) {
    std::size_t sprite_base = i_sprite * kSpriteStride;
    std::size_t sprite_proc_base = i_sprite * kSpriteProcStride;

    for (unsigned int i_sprite_x = 0; i_sprite_x < kSpriteWidth; ++i_sprite_x) {
      for (unsigned int i_sprite_y = 0; i_sprite_y < kSpriteHeight; ++i_sprite_y) {
        unsigned int sprite_col_byte = i_sprite_x / 8;

        std::size_t sprite_byte_index = sprite_base + i_sprite_y * kSpriteWidthBytes + i_sprite_x;
        std::size_t proc_byte_index = sprite_proc_base + i_sprite_y * kProcWidth + i_sprite_x;

        unsigned char byte_in = buffer[sprite_byte_index];

        unsigned int sprite_bit;

        if (kSpriteHiResIndexes.contains(i_sprite)) {
          sprite_bit = i_sprite_x - 8 * sprite_col_byte;

          if ((1 << sprite_bit) & byte_in) {
            proc_sprite_data[proc_byte_index] = kSpriteColor;
          } else {
            proc_sprite_data[proc_byte_index] = kSpriteBackground;
          }
        } else {
          sprite_bit = 2 * ((i_sprite_x - 8 * sprite_col_byte) / 2);
          proc_sprite_data[proc_byte_index] = (byte_in >> sprite_bit) & 3;
        }
      }
    }
  }

  // Create mirror images versions
  // Create 2xX versions
  // Create 2XY versions
  // Create 2xX & 2xY versions

  return true;
}

void PaintClassicSprites(BurningLogic::ShaderVars shader_vars, SDL_FRect sdl_rect,
                         ClassicSpriteGLBuffers gl_buffers) {
  std::array<GLfloat, 8> vertex_buffer = {{sdl_rect.x, sdl_rect.y, sdl_rect.x + sdl_rect.w, sdl_rect.y,
                                           sdl_rect.x, sdl_rect.y + sdl_rect.h, sdl_rect.x + sdl_rect.w,
                                           sdl_rect.y + sdl_rect.h}};
  std::array<GLuint, 8> index_buffer = {{0, 1, 2, 3}};

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
  GLuint vpos_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "v_pos");
  glEnableVertexAttribArray(vpos_location);
  BurningLogic::PrintGLError("PaintClassicSprites:glEnableVertexAttribArray");

  // Set vertex data
  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintClassicSprites:glBindBuffer");

  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
  BurningLogic::PrintGLError("PaintClassicSprites:glVertexAttribPointer");

  // Texture stuff (tile buffer)
  GLint tilebuffer_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_sprite_buffer");
  glUniform1i(tilebuffer_location, 4);  // TODO: how do I map these globally to GL_TEXTURE0 etc?
  BurningLogic::PrintGLError("PaintClassicSprites:glUniform1i");

  // Texture stuff (clut)
  GLint clut_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_clut_buffer");
  glUniform1i(clut_location, 3);
  BurningLogic::PrintGLError("PaintClassicSprites:glUniform1i");

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

  // Disable vertex position
  glDisableVertexAttribArray(vpos_location);

  // Unbind program
  glUseProgram(0);
}

}  // namespace sprites_classic
