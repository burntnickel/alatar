#ifndef H_ALATAR_CLASSIC
#define H_ALATAR_CLASSIC

// Miscelanaeous data / functions that only apply to the classic mode display

#include <array>
#include <string>

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
const auto classic_tile_texture = GL_TEXTURE0;
const auto classic_color_texture = GL_TEXTURE1;
const auto classic_tileset_texture = GL_TEXTURE2;
const auto classic_clut_texture = GL_TEXTURE3;
const auto classic_sprite_texture = GL_TEXTURE4;

// Data use in main program for classic display mode
struct ClassicData {
  std::array<unsigned char, wizard_level::kTileBufferSize> tile_set{};
  std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> tile_buffer{};
  std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> color_buffer{};
  std::array<unsigned char, alatar_classic::kSpriteProcDataSize> processed_sprites{};

  alatar::ValueCycle<unsigned char> treasure_color_cycle{alatar_classic::kTreasureCycleColors};
  alatar::ValueCycle<unsigned char> fire_color_cycle{alatar_classic::kFireCycleColors};

  alatar_classic::ClassicTileGLBuffers tile_glbuffers;
  alatar_classic::ClassicSpriteGLBuffers sprite_glbuffers;

  BurningLogic::ShaderVars tile_shader;
  BurningLogic::ShaderVars sprite_shader;

  Uint64 treasure_color_cycle_counter;
  Uint64 fire_color_cycle_counter;
  Uint64 fire_animation_counter;
};

bool SetupClassic(ClassicData& data, std::filesystem::path resource_path, std::filesystem::path shader_path);

void ClassicLevelInit(ClassicData& data, const wizard_level::LevelClass& level);

void ClassicUpdate(alatar_classic::ClassicData& classic_data, Uint64 start_counter,
                   double counter_to_ms_scale, const wizard_level::LevelClass& level);

void ClassicDraw(const alatar_classic::ClassicData& classic_data,
                 const wizard_level::MonsterInfoArray& monster_info,
                 const wizard_level::WizardInfo& wizard_info, const BurningLogic::mat4& view_matrix);

}  // namespace alatar_classic

#endif
