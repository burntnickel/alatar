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

#ifndef H_ALATAR_CLASSIC
#define H_ALATAR_CLASSIC

// Miscelanaeous data / functions that only apply to the classic mode display

#include <array>
#include <optional>
#include <string>

#include "graphics_common.h"
#include "opengl_helper.h"
#include "sprites_classic.h"
#include "tiles_classic.h"
#include "value_cycle.h"
#include "wiz_level.h"

namespace alatar_classic {

// Color sequences for animating fire and treasures
const std::array<const unsigned char, 3> kFireCycleColors{{8, 7, 10}};     // orange, yellow, pink
const std::array<const unsigned char, 3> kTreasureCycleColors{{7, 1, 3}};  // yellow, white, cyan

// Bounds of the fire animtation charaters to cycle through
constexpr unsigned char kFireTileFirst = 114;
constexpr unsigned char kFireTileLast = 117;

// Rather then specify the number of frame between animation updates here we specify the time between frames
// and do the computation dynamically in the game loop
constexpr double kTreasureColorCycleFrameTimeMs = 1000.0 / 15.0;
constexpr double kFireColorCycleFrameTimeMs = 1000.0 / 20.0;
constexpr double kFireAnimationFrameTimeMs = 1000.0 / 10.0;

// Shader filenames
const std::string kClassicTileVertexShaderName{"classic_vertex.glsl"};
const std::string kClassicTileFragmentShaderName{"classic_tile_fragment.glsl"};
const std::string kClassicSpriteVertexShaderName{"classic_vertex.glsl"};
const std::string kClassicSpriteFragmentShaderName{"classic_sprite_fragment.glsl"};

// Texture mappings used by the classic shaders
constexpr auto kClassicTileTextureUnit = GL_TEXTURE0;
constexpr auto kClassicColorTextureUnit = GL_TEXTURE1;
constexpr auto kClassicTilesetTextureUnit = GL_TEXTURE2;
constexpr auto kClassicClutTextureUnit = GL_TEXTURE3;
constexpr auto kClassicSpriteTextureUnit = GL_TEXTURE4;

// Data use in main program for classic display mode
struct ClassicData {
  std::array<unsigned char, alatar::kTileBufferSize> tile_set{};
  std::array<unsigned char, alatar::kRowTiles * alatar::kColTiles> tile_buffer{};
  std::array<unsigned char, alatar::kRowTiles * alatar::kColTiles> color_buffer{};
  std::array<unsigned char, kSpriteProcDataSize> processed_sprites{};

  alatar::ValueCycle<unsigned char> treasure_color_cycle{kTreasureCycleColors};
  alatar::ValueCycle<unsigned char> fire_color_cycle{kFireCycleColors};

  ClassicTileGLBuffers tile_glbuffers;
  ClassicSpriteGLBuffers sprite_glbuffers;

  BurningLogic::ShaderVars tile_shader;
  BurningLogic::ShaderVars sprite_shader;

  Uint64 treasure_color_cycle_counter;
  Uint64 fire_color_cycle_counter;
  Uint64 fire_animation_counter;
};

// Don't want to be able to construct, move, copy, or assgn this class
// This is the virtual base class for the graphics for each mode
class ClassicClass : public alatar::GraphicsCommonClass {
  // I don't like this but it does prevent me from accidentally constructing an object
 private:
  struct Token {};

 public:
  // Delete default constuctor as classes should only
  ClassicClass(void) = delete;

  // Delete copy constructor as this class should not be copied, only moved
  ClassicClass(const ClassicClass& copyFrom) = delete;

  // Delete copy assignment operator as this class should not be copied, only moved
  ClassicClass& operator=(const ClassicClass& copyFrom) = delete;

  // Need the move constructor
  ClassicClass(ClassicClass&& other);

  // Need the more assigment operator
  ClassicClass& operator=(ClassicClass&& other);

  // Destructor
  ~ClassicClass(void) override;

  // Constructor for the factory (protected by the token)
  ClassicClass(Token, const ClassicData& data);

 private:
  ClassicData data_;

 public:
  void LevelInit(const alatar::LevelClass& level) override;

  void Update(Uint64 counter, const alatar::LevelClass& level) override;

  void Draw(const alatar::MonsterClassArray& monster_info, const alatar::WizardInfo& wizard_info,
            const BurningLogic::mat4& view_matrix) override;

  // Factory function to make sure return objects are always properly constructed
  // Ideally should use std:expected but that would require c++23
  static std::optional<alatar::GraphicsCommonPtr> ClassicClassFactory(std::filesystem::path resource_path,
                                                                      std::filesystem::path shader_path,
                                                                      Uint64 sdl_counter);
};

}  // namespace alatar_classic

#endif
