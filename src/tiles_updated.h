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
  GLuint color_buffer;
  GLuint color_texture;
  GLuint tile_buffer;
  GLuint tile_texture;
  GLuint clut_buffer;
  GLuint clut_texture;
  GLuint wall_texture;
  GLuint tile_mask_texture;
};

constexpr std::size_t kNumTiles = alatar::kRowTiles * alatar::kColTiles;

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

unsigned int GetWallTileMaskIndex(alatar::MaskTiles mask_tiles);

// Lookup tables for "smoothing" wall tiles
// Shallow maping could be improved by expanding the context one more to the left and right
const std::array<unsigned char, 32> kWallMaskSmoothTable{
    {6, 7, 5, 7, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 4, 3, 1, 3, 1, 1, 1, 1, 4, 3, 1, 3, 1, 1, 1, 1}};
const std::array<unsigned char, 32> kSteepStairLeftSmoothTable{
    {9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}};
const std::array<unsigned char, 32> kSteepStairRightSmoothTable{{11, 10, 10, 10, 10, 10, 10, 10, 11, 10, 10,
                                                                 10, 10, 10, 10, 10, 11, 10, 10, 10, 10, 10,
                                                                 10, 10, 11, 10, 10, 10, 10, 10, 10, 10}};
const std::array<unsigned char, 32> kShallowStairLeft1SmoothTable{{13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 12,
                                                                   12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                                                                   12, 12, 12, 12, 12, 12, 12, 12, 12, 12}};
const std::array<unsigned char, 32> kShallowStairLeft2SmoothTable{{15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14,
                                                                   14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
                                                                   14, 14, 14, 14, 14, 14, 14, 14, 14, 14}};
const std::array<unsigned char, 32> kShallowStairRight2SmoothTable{
    {17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16,
     17, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16}};
const std::array<unsigned char, 32> kShallowStairRight1SmoothTable{
    {19, 18, 18, 18, 18, 18, 18, 18, 19, 18, 18, 18, 18, 18, 18, 18,
     19, 18, 18, 18, 18, 18, 18, 18, 19, 18, 18, 18, 18, 18, 18, 18}};
const std::array<unsigned char, 32> kSlideLeftSmoothTable{{21, 21, 21, 21, 20, 20, 20, 20, 20, 20, 20,
                                                           20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
                                                           20, 20, 20, 20, 20, 20, 20, 20, 20, 20}};
const std::array<unsigned char, 32> kSlideRightSmoothTable{{23, 22, 22, 22, 22, 22, 22, 22, 23, 22, 22,
                                                            22, 22, 22, 22, 22, 23, 22, 22, 22, 22, 22,
                                                            22, 22, 23, 22, 22, 22, 22, 22, 22, 22}};

}  // namespace alatar_updated

#endif
