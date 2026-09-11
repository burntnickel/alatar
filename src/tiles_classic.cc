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

#include "tiles_classic.h"

#include "load_data.h"

namespace alatar_classic {

bool LoadCharSet(std::filesystem::path path_and_name,
                 std::span<unsigned char, alatar::kTileBufferSize> char_set) {
  std::array<unsigned char, alatar::kCharSetSize> buffer;

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

void PaintClassicTiles(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                       SDL_FRect sdl_rect, ClassicTileGLBuffers gl_buffers) {
  std::array<GLfloat, 16> vertex_buffer = {{sdl_rect.x, sdl_rect.y, 0.0, 0.0, sdl_rect.x + sdl_rect.w,
                                            sdl_rect.y, 1.0, 0.0, sdl_rect.x, sdl_rect.y + sdl_rect.h, 0.0,
                                            1.0, sdl_rect.x + sdl_rect.w, sdl_rect.y + sdl_rect.h, 1.0, 1.0}};
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
  GLuint in_pos_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "in_pos");
  glEnableVertexAttribArray(in_pos_location);
  BurningLogic::PrintGLError("PaintClassicTiles:glEnableVertexAttribArray");

  // Enable vertex texture coordinates
  GLuint in_uv_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "in_uv");
  glEnableVertexAttribArray(in_uv_location);
  BurningLogic::PrintGLError("PaintClassicTiles:glEnableVertexAttribArray");

  // Set vertex data
  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindBuffer");

  glVertexAttribPointer(in_pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
  BurningLogic::PrintGLError("PaintClassicTiles:glVertexAttribPointer");

  glVertexAttribPointer(in_uv_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (void*)(2 * sizeof(GLfloat)));
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

  // View Matrix
  GLint view_mat_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_view_matrix");
  glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_matrix.data());
  BurningLogic::PrintGLError("PaintClassicTiles:glUniformMatrix4fv");

  // TODO: do I need these?
  //  ------
  glActiveTexture(GL_TEXTURE0);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

  //glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tile_buffer);
  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tile_texture);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE1);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

  //glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.color_buffer);
  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.color_texture);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE2);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

 // glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tileset_buffer);
  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tileset_texture);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE3);
  BurningLogic::PrintGLError("PaintClassicTiles:glActiveTexture");

  //glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.clut_buffer);
  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.clut_texture);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindTexture");

  // Set index data and render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintClassicTiles:glBindBuffer");

  glDrawElements(GL_TRIANGLE_STRIP, 2 * 2, GL_UNSIGNED_INT, NULL);
  BurningLogic::PrintGLError("PaintClassicTiles:glDrawElements");

  // Disable vertex attributes
  glDisableVertexAttribArray(in_pos_location);
  glDisableVertexAttribArray(in_uv_location);

  // Unbind program
  glUseProgram(0);
}

}  // namespace alatar_classic
