#ifndef H_ALATAR_TILES_UPDATED
#define H_ALATAR_TILES_UPDATED

// #include <SDL.h>

#include <array>
#include <cstddef>

#include "opengl_helper.h"
#include "wiz_level.h"

namespace alatar_updated {

struct UpdatedTileGLBuffers {
  // GLuint tbo_tile_buffer;
  // GLuint tbo_tex_tile_buffer;
  // GLuint tbo_color_buffer;
  // GLuint tbo_tex_color_buffer;
  // GLuint tbo_tileset_buffer;
  // GLuint tbo_tex_tileset_buffer;
  GLuint tbo_clut_buffer;
  GLuint tbo_tex_clut_buffer;
};

constexpr std::size_t kNumTiles = wizard_level::kRowTiles * wizard_level::kColTiles;

// Vertex buffer is number of tiles * 4 (the four verticies of each tile) * the number of attributes
// Index buffer is number of tiles * 2 (triangles per tile) * 3 (verticies per triangle)
class UpdatedTileVertexManager {
 public:
  UpdatedTileVertexManager(void);

 public:
  // Attribute list
  // x, y - global vertex coordinates
  // u, v - local texture coordinates
  const static std::size_t num_attributes_ = 4;

 public:
  std::array<GLfloat, 4 * num_attributes_ * kNumTiles> vertex_buffer_;
  std::array<GLuint, 2 * 3 * kNumTiles> index_buffer_;

 private:
  void SetXY(unsigned int row, unsigned int col, float x0, float x1, float y0, float y1);
  void SetUV(unsigned int row, unsigned int col, float u0, float u1, float v0, float v1);
  void PopulateIndexBufferAt(unsigned int row, unsigned int col);
  std::size_t RowColToVertexIndex(unsigned int row, unsigned int col) const;
};

/*bool LoadCharSet(std::filesystem::path path_and_name,
                 std::span<unsigned char, wizard_level::kTileBufferSize> char_set);*/

void PaintUpdatedTiles(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                       const UpdatedTileVertexManager& vertex_manager, UpdatedTileGLBuffers gl_buffers);

}  // namespace alatar_updated

#endif
