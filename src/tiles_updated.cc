#include "tiles_updated.h"

#include "updated.h"

namespace alatar_updated {

UpdatedTileVertexManager::UpdatedTileVertexManager(void) {
  // Local UV texture coordinates
  const float u0 = 0.0;
  const float u1 = 1.0;
  const float v0 = 0.0;
  const float v1 = 1.0;

  float dx = 1.0f / static_cast<float>(alatar::kColTiles);
  float dy = 1.0f / static_cast<float>(alatar::kRowTiles);

  for (unsigned int r = 0; r < alatar::kRowTiles; ++r) {
    for (unsigned int c = 0; c < alatar::kColTiles; ++c) {
      // Global XY vertex coordinates
      float x0 = 2.0f * static_cast<float>(c) * dx - 1.0f;
      float x1 = x0 + 2.0f * dx;
      float y0 = 2.0f * static_cast<float>(r) * dy - 1.0f;
      float y1 = y0 + 2.0f * dy;

      SetXY(r, c, x0, x1, y0, y1);
      SetUV(r, c, u0, u1, v0, v1);

      // Populate index buffer
      PopulateIndexBufferAt(r, c);
    }
  }
}

void UpdatedTileVertexManager::SetXY(unsigned int row, unsigned int col, float x0, float x1, float y0,
                                     float y1) {
  std::size_t base_index = RowColToVertexIndex(row, col);

  // Upper left?
  vertex_buffer_[base_index + 0 * num_attributes_ + 0] = x0;
  vertex_buffer_[base_index + 0 * num_attributes_ + 1] = y0;

  // Upper right?
  vertex_buffer_[base_index + 1 * num_attributes_ + 0] = x1;
  vertex_buffer_[base_index + 1 * num_attributes_ + 1] = y0;

  // Lower left?
  vertex_buffer_[base_index + 2 * num_attributes_ + 0] = x0;
  vertex_buffer_[base_index + 2 * num_attributes_ + 1] = y1;

  // Lower right?
  vertex_buffer_[base_index + 3 * num_attributes_ + 0] = x1;
  vertex_buffer_[base_index + 3 * num_attributes_ + 1] = y1;
}

void UpdatedTileVertexManager::SetUV(unsigned int row, unsigned int col, float u0, float u1, float v0,
                                     float v1) {
  const std::size_t base_index = RowColToVertexIndex(row, col);

  // Upper left?
  vertex_buffer_[base_index + 0 * num_attributes_ + 2] = u0;
  vertex_buffer_[base_index + 0 * num_attributes_ + 3] = v0;

  // Upper right?
  vertex_buffer_[base_index + 1 * num_attributes_ + 2] = u1;
  vertex_buffer_[base_index + 1 * num_attributes_ + 3] = v0;

  // Lower left?
  vertex_buffer_[base_index + 2 * num_attributes_ + 2] = u0;
  vertex_buffer_[base_index + 2 * num_attributes_ + 3] = v1;

  // Lower right?
  vertex_buffer_[base_index + 3 * num_attributes_ + 2] = u1;
  vertex_buffer_[base_index + 3 * num_attributes_ + 3] = v1;
}

void UpdatedTileVertexManager::PopulateIndexBufferAt(unsigned int row, unsigned int col) {
  const std::size_t base_index_index = (col + alatar::kColTiles * row) * 6;
  const std::size_t base_vertex = (col + alatar::kColTiles * row) * 4;

  // First triangle
  index_buffer_[base_index_index + 0] = static_cast<GLuint>(base_vertex + 0);
  index_buffer_[base_index_index + 1] = static_cast<GLuint>(base_vertex + 1);
  index_buffer_[base_index_index + 2] = static_cast<GLuint>(base_vertex + 2);

  // Second triangle
  index_buffer_[base_index_index + 3] = static_cast<GLuint>(base_vertex + 1);
  index_buffer_[base_index_index + 4] = static_cast<GLuint>(base_vertex + 2);
  index_buffer_[base_index_index + 5] = static_cast<GLuint>(base_vertex + 3);
}

std::size_t UpdatedTileVertexManager::RowColToVertexIndex(unsigned int row, unsigned int col) const {
  // Times 4 is to account for four verticies per tile
  return (col + alatar::kColTiles * row) * num_attributes_ * 4;
}

void PaintUpdatedTiles(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                       const UpdatedTileVertexManager& vertex_manager, UpdatedTileGLBuffers gl_buffers) {
  /* std::array<GLfloat, 16> vertex_buffer = {{sdl_rect.x, sdl_rect.y, 0.0, 0.0, sdl_rect.x + sdl_rect.w,
                                             sdl_rect.y, 1.0, 0.0, sdl_rect.x, sdl_rect.y + sdl_rect.h, 0.0,
                                             1.0, sdl_rect.x + sdl_rect.w, sdl_rect.y +
   sdl_rect.h, 1.0, 1.0}}; std::array<GLuint, 8> index_buffer = {{0, 1, 2, 3}};*/

  // Bind program
  glUseProgram(shader_vars.program);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glUseProgram");

  glBindVertexArray(shader_vars.vao);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBindVertexArray");

  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBindBuffer");

  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertex_manager.vertex_buffer_.size() * sizeof(GLfloat)),
               vertex_manager.vertex_buffer_.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBufferData");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBindBuffer");

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertex_manager.index_buffer_.size() * sizeof(GLuint)),
               vertex_manager.index_buffer_.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBufferData");

  // Enable vertex position
  GLuint in_pos_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "in_pos");
  glEnableVertexAttribArray(in_pos_location);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glEnableVertexAttribArray");

  // Enable vertex texture coordinates
  GLuint in_uv_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "in_uv");
  glEnableVertexAttribArray(in_uv_location);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glEnableVertexAttribArray");

  // Set vertex data
  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBindBuffer");

  glVertexAttribPointer(in_pos_location, 2, GL_FLOAT, GL_FALSE,
                        vertex_manager.num_attributes_ * sizeof(GLfloat), (void*)0);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glVertexAttribPointer");

  glVertexAttribPointer(in_uv_location, 2, GL_FLOAT, GL_FALSE,
                        vertex_manager.num_attributes_ * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
  BurningLogic::PrintGLError("PaintUpdatedTiles:glVertexAttribPointer");

  // Texture stuff (wall texture)
  GLint wall_texture_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_wall_texture");
  glUniform1i(wall_texture_location, kUpdatedWallTextureUnitNumber);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glUniform1i(wall_texture_location)");

  // Texture stuff (tile and mask texture)
  GLint tile_mask_texture_location =
      BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_tile_mask_texture");
  glUniform1i(tile_mask_texture_location, kUpdatedTileMaskTextureUnitNumber);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glUniform1i(tile_mask_texture_location)");

  /*// Texture stuff (tile buffer)
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
  BurningLogic::PrintGLError("PaintClassicTiles:glUniform1i");*/

  // Texture stuff (clut)
  GLint clut_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_clut_buffer");
  glUniform1i(clut_location, kUpdatedClutTextureUnitNumber);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glUniform1i");

  // View Matrix
  GLint view_mat_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_view_matrix");
  glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_matrix.data());
  BurningLogic::PrintGLError("PaintClassicTiles:glUniformMatrix4fv");

  // TODO: do I need these?
  //  ------
  /* glActiveTexture(GL_TEXTURE0);
   BurningLogic::PrintGLError("PaintUpdatedTiles:glActiveTexture");

   glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_tile_buffer);
   BurningLogic::PrintGLError("PaintUpdatedTiles:glBindTexture");

   // ------
   glActiveTexture(GL_TEXTURE1);
   BurningLogic::PrintGLError("PaintUpdatedTiles:glActiveTexture");

   glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_color_buffer);
   BurningLogic::PrintGLError("PaintUpdatedTiles:glBindTexture");

   // ------
   glActiveTexture(GL_TEXTURE2);
   BurningLogic::PrintGLError("PaintUpdatedTiles:glActiveTexture");

   glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.tbo_tileset_buffer);
   BurningLogic::PrintGLError("PaintUpdatedTiles:glBindTexture");*/

  // ------
  glActiveTexture(kUpdatedClutTextureUnit);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glActiveTexture");

  /*std::cout << "clut_buffer: " << gl_buffers.clut_buffer << "\n";
  std::cout << "clut_texture: " << gl_buffers.clut_texture << "\n";
  std::cout << "wall_texture: " << gl_buffers.wall_texture << "\n";*/

  // glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.clut_buffer);
  glBindTexture(GL_TEXTURE_BUFFER, gl_buffers.clut_texture);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBindTexture");

  // Set index data and render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glBindBuffer");

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_manager.index_buffer_.size()), GL_UNSIGNED_INT,
                 NULL);
  BurningLogic::PrintGLError("PaintUpdatedTiles:glDrawElements");

  // Disable vertex attributes
  glDisableVertexAttribArray(in_pos_location);
  glDisableVertexAttribArray(in_uv_location);

  // Unbind program
  glUseProgram(0);
}

}  // namespace alatar_updated
