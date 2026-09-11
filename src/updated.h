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

#ifndef H_ALATAR_UPDATED
#define H_ALATAR_UPDATED

// Miscelanaeous data / functions that only apply to the updated mode display

#include <SDL.h>

#include <array>
#include <filesystem>
#include <string>

#include "graphics_common.h"
#include "opengl_helper.h"
#include "tiles_updated.h"

namespace alatar_updated {

// Shader filenames
const std::string kUpdatedTileVertexShaderName{"updated_tile_vertex.glsl"};
const std::string kUpdatedTileWallFragmentShaderName{"updated_tile_wall_fragment.glsl"};
const std::string kUpdatedTileMiscFragmentShaderName{"updated_tile_misc_fragment.glsl"};
// const std::string kClassicSpriteVertexShaderName{"classic_vertex.glsl"};
// const std::string kClassicSpriteFragmentShaderName{"classic_sprite_fragment.glsl"};

// Texture mappings used by the updated shaders
constexpr int kUpdatedWallTextureUnitNumber = 0;
constexpr int kUpdatedTileMaskTextureUnitNumber = 1;
constexpr int kUpdatedTileDataTextureUnitNumber = 2;
constexpr int kUpdatedClutTextureUnitNumber = 3;
constexpr int kUpdatedColorTextureUnitNumber = 4;

// Map texture unit numbers to the corresponding enums
constexpr auto kUpdatedWallTextureUnit = GL_TEXTURE0 + kUpdatedWallTextureUnitNumber;
constexpr auto kUpdatedTileMaskTextureUnit = GL_TEXTURE0 + kUpdatedTileMaskTextureUnitNumber;
constexpr auto kUpdatedTileDataTextureUnit = GL_TEXTURE0 + kUpdatedTileDataTextureUnitNumber;
constexpr auto kUpdatedClutTextureUnit = GL_TEXTURE0 + kUpdatedClutTextureUnitNumber;
constexpr auto kUpdatedColorTextureUnit = GL_TEXTURE0 + kUpdatedColorTextureUnitNumber;

constexpr unsigned int kLevelTileCount = alatar::kRowTiles * alatar::kColTiles;
constexpr unsigned int kWallMaskOffset = 0 * kLevelTileCount;
constexpr unsigned int kTileMiscOffset = 1 * kLevelTileCount;

// Data use in main program for the updated display mode
struct UpdatedData {
  std::array<unsigned char, 2 * kLevelTileCount> tile_buffer;
  std::array<unsigned char, kLevelTileCount> color_buffer;

  // std::array<unsigned char, wizard_level::kTileBufferSize> tile_set{};
  // std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> tile_buffer{};
  // std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> color_buffer{};
  // std::array<unsigned char, alatar_classic::kSpriteProcDataSize> processed_sprites{};

  UpdatedTileGLBuffers tile_glbuffers;
  // alatar_classic::ClassicSpriteGLBuffers sprite_glbuffers;

  BurningLogic::ShaderVars tile_wall_shader;
  BurningLogic::ShaderVars tile_misc_shader;
  // BurningLogic::ShaderVars sprite_shader;

  UpdatedTileVertexManager vertex_manager;

  SDL_Surface* wall_texture_surface = nullptr;
  SDL_Surface* tiles_and_masks_surface = nullptr;
};

// Don't want to be able to construct, move, copy, or assgn this class
// This is the virtual base class for the graphics for each mode
class UpdatedClass : public alatar::GraphicsCommonClass {
  // I don't like this but it does prevent me from accidentally constructing an object
 private:
  struct Token {};

 public:
  // Delete default constuctor
  UpdatedClass(void) = delete;

  // Delete copy constructor as this class should not be copied, only moved
  UpdatedClass(const UpdatedClass& copyFrom) = delete;

  // Delete copy assignment operator as this class should not be copied, only moved
  UpdatedClass& operator=(const UpdatedClass& copyFrom) = delete;

  // Need the move constructor
  UpdatedClass(UpdatedClass&& other);

  // Need the more assigment operator
  UpdatedClass& operator=(UpdatedClass&& other);

  // Destructor
  ~UpdatedClass(void) override;

  // Constructor for the factory (protected by the token)
  UpdatedClass(Token, const UpdatedData& data);

 private:
  void UpdateWalls(const alatar::LevelClass& level);
  void UpdateMiscTiles(const alatar::LevelClass& level);

 private:
  UpdatedData data_;

 public:
  void LevelInit(const alatar::LevelClass& level) override;

  void Update(Uint64 counter, const alatar::LevelClass& level) override;

  void Draw(const alatar::MonsterClassArray& monster_info, const alatar::WizardInfo& wizard_info,
            const BurningLogic::mat4& view_matrix) override;

  // Factory function to make sure return objects are always properly constructed
  // Ideally should use std:expected but that would require c++23
  static std::optional<alatar::GraphicsCommonPtr> UpdatedClassFactory(std::filesystem::path resource_path,
                                                                      std::filesystem::path shader_path,
                                                                      Uint64 sdl_counter);
};

}  // namespace alatar_updated

#endif
