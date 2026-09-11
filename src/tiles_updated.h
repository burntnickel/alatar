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

#ifndef H_ALATAR_TILES_UPDATED
#define H_ALATAR_TILES_UPDATED

// #include <SDL.h>

#include <array>
#include <cstddef>

#include "maskstiles.h"
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

void PaintUpdatedTilesWalls(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                            const UpdatedTileVertexManager& vertex_manager, UpdatedTileGLBuffers gl_buffers);

void PaintUpdatedTilesMisc(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                           const UpdatedTileVertexManager& vertex_manager, UpdatedTileGLBuffers gl_buffers);

unsigned int GetWallTileMaskIndex(alatar::MaskTiles mask_tiles);

// Lookup tables for "smoothing" wall tiles
// Shallow maping could be improved by expanding the context one more to the left and right
const std::array<unsigned char, 32> kWallMaskSmoothTable{
    {kMaskTileWall101,  kMaskTileWall110,  kMaskTileWall100,  kMaskTileWall110,  kMaskTileWallBase,
     kMaskTileWallBase, kMaskTileWallBase, kMaskTileWallBase, kMaskTileWall001,  kMaskTileWallBase,
     kMaskTileWallBase, kMaskTileWallBase, kMaskTileWallBase, kMaskTileWallBase, kMaskTileWallBase,
     kMaskTileWallBase, kMaskTileWall011,  kMaskTileWall010,  kMaskTileWallBase, kMaskTileWall010,
     kMaskTileWallBase, kMaskTileWallBase, kMaskTileWallBase, kMaskTileWallBase, kMaskTileWall011,
     kMaskTileWall010,  kMaskTileWallBase, kMaskTileWall010,  kMaskTileWallBase, kMaskTileWallBase,
     kMaskTileWallBase, kMaskTileWallBase}};
const std::array<unsigned char, 32> kSteepStairLeftSmoothTable{
    {kMaskTileSteepStairsLeft1,    kMaskTileSteepStairsLeft1,    kMaskTileSteepStairsLeft1,
     kMaskTileSteepStairsLeft1,    kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase,
     kMaskTileSteepStairsLeftBase, kMaskTileSteepStairsLeftBase}};
const std::array<unsigned char, 32> kSteepStairRightSmoothTable{
    {kMaskTileSteepStairsRight1,    kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRight1,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRight1,    kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRight1,    kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase,
     kMaskTileSteepStairsRightBase, kMaskTileSteepStairsRightBase}};
const std::array<unsigned char, 32> kShallowStairLeft1SmoothTable{
    {kMaskTileShallowStairsLeftLeftPart1,    kMaskTileShallowStairsLeftLeftPart1,
     kMaskTileShallowStairsLeftLeftPart1,    kMaskTileShallowStairsLeftLeftPart1,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase,
     kMaskTileShallowStairsLeftLeftPartBase, kMaskTileShallowStairsLeftLeftPartBase}};
const std::array<unsigned char, 32> kShallowStairLeft2SmoothTable{
    {kMaskTileShallowStairsLeftRightPart1,    kMaskTileShallowStairsLeftRightPart1,
     kMaskTileShallowStairsLeftRightPart1,    kMaskTileShallowStairsLeftRightPart1,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase,
     kMaskTileShallowStairsLeftRightPartBase, kMaskTileShallowStairsLeftRightPartBase}};
const std::array<unsigned char, 32> kShallowStairRight2SmoothTable{
    {kMaskTileShallowStairsRightLeftPart1,    kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPart1,    kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPart1,    kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPart1,    kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase,
     kMaskTileShallowStairsRightLeftPartBase, kMaskTileShallowStairsRightLeftPartBase}};
const std::array<unsigned char, 32> kShallowStairRight1SmoothTable{
    {kMaskTileShallowStairsRightRightPart1,    kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPart1,    kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPart1,    kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPart1,    kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase,
     kMaskTileShallowStairsRightRightPartBase, kMaskTileShallowStairsRightRightPartBase}};
const std::array<unsigned char, 32> kSlideLeftSmoothTable{
    {kMaskTileSlideLeft1,    kMaskTileSlideLeft1,    kMaskTileSlideLeft1,    kMaskTileSlideLeft1,
     kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase,
     kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase,
     kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase,
     kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase,
     kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase,
     kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase,
     kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase, kMaskTileSlideLeftBase}};
const std::array<unsigned char, 32> kSlideRightSmoothTable{
    {kMaskTileSlideRight1,    kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase,
     kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase,
     kMaskTileSlideRight1,    kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase,
     kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase,
     kMaskTileSlideRight1,    kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase,
     kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase,
     kMaskTileSlideRight1,    kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase,
     kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase, kMaskTileSlideRightBase}};

}  // namespace alatar_updated

#endif
