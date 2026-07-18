#include "tiles_classic.h"

#include "load_data.h"

namespace alatar_classic {

bool LoadCharSet(std::filesystem::path path_and_name,
                 std::span<unsigned char, wizard_level::kTileBufferSize> char_set) {
  std::array<unsigned char, wizard_level::kCharSetSize> buffer;

  bool success = alatar::LoadData(path_and_name, buffer, alatar::kLoadAddressOffset);

  if (!success) {
    return false;
  }

  // This includes all of the funny decoding of the byte/bit ordering
  for (unsigned int chr = 0; chr < 256; ++chr) {
    for (unsigned int in_rr = 0; in_rr < 8; ++in_rr) {
      unsigned int out_rr = 8 * chr + in_rr;
      unsigned int in_idx = 8 * chr + in_rr;

      unsigned char c = static_cast<unsigned char>(buffer[in_idx]);

      for (unsigned int bb = 0; bb < 8; ++bb) {
        unsigned int out_idx = 8 * out_rr + bb;

        if (c & 128) {
          char_set[out_idx] = 255;
        } else {
          char_set[out_idx] = 0;
        }

        c = c << 1;
      }
    }
  }

  return true;
}

void PaintClassicTiles(BurningLogic::ShaderVars shader_vars, SDL_FRect sdl_rect,
                       ClassicTileGLBuffers gl_buffers) {
  std::array<GLfloat, 8> vertex_buffer = {{sdl_rect.x, sdl_rect.y, sdl_rect.x + sdl_rect.w, sdl_rect.y,
                                           sdl_rect.x, sdl_rect.y + sdl_rect.h, sdl_rect.x + sdl_rect.w,
                                           sdl_rect.y + sdl_rect.h}};
  std::array<GLuint, 8> index_buffer = {{0, 1, 2, 3}};

  // Bind program
  glUseProgram(shader_vars.program);
  BurningLogic::PrintGLError("PaintClassicTiles:glUseProgram");

  glBindVertexArray(shader_vars.vao);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindVertexArray");

  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindBuffer");

  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertex_buffer.size() * sizeof(GLfloat)),
               vertex_buffer.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintClassicTiles:glBufferData");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindBuffer");

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_buffer.size() * sizeof(GLuint)),
               index_buffer.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintClassicTiles:glBufferData");

  // Enable vertex position
  GLuint vpos_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "v_pos");
  glEnableVertexAttribArray(vpos_location);
  BurningLogic::PrintGLError("PaintClassicTiles:glEnableVertexAttribArray");

  // Set vertex data
  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindBuffer");

  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
  BurningLogic::PrintGLError("PaintClassicTiles:glVertexAttribPointer");

  // Texture stuff (tile buffer)
  GLint tilebuffer_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_tile_buffer");
  glUniform1i(tilebuffer_location, 0);
  BurningLogic::PrintGLError("PaintClassicTiles:glUniform1i");

  // Texture stuff (color buffer)
  GLint colorbuffer_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_color_buffer");
  glUniform1i(colorbuffer_location, 1);
  BurningLogic::PrintGLError("PaintClassicTiles:glUniform1i");

  // Texture stuff (tile set buffer)
  GLint tileset_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_tileset_buffer");
  glUniform1i(tileset_location, 2);
  BurningLogic::PrintGLError("PaintClassicTiles:glUniform1i");

  // Texture stuff (clut)
  GLint clut_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_clut_buffer");
  glUniform1i(clut_location, 3);
  BurningLogic::PrintGLError("PaintClassicTiles:glUniform1i");

  // TODO: do I need these?
  //  ------
  glActiveTexture(GL_TEXTURE0);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_tile_buffer);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE1);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_color_buffer);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE2);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_tileset_buffer);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE3);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_clut_buffer);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // Set index data and render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindBuffer");

  glDrawElements(GL_TRIANGLE_STRIP, 2 * 2, GL_UNSIGNED_INT, NULL);
  BurningLogic::PrintGLError("PaintClassicTiles:glDrawElements");

  // Disable vertex position
  glDisableVertexAttribArray(vpos_location);

  // Unbind program
  glUseProgram(0);
}

}  // namespace alatar_classic
