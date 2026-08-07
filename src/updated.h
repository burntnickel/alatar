#ifndef H_ALATAR_UPDATED
#define H_ALATAR_UPDATED

// Miscelanaeous data / functions that only apply to the updated mode display

#include <filesystem>
#include <string>

#include "opengl_helper.h"

namespace alatar_updated {

// Shader filenames
const std::string kUpdatedTileVertexShaderName{"updated_tile_vertex.glsl"};
const std::string kUpdatedTileFragmentShaderName{"updated_tile_fragment.glsl"};
// const std::string kClassicSpriteVertexShaderName{"classic_vertex.glsl"};
// const std::string kClassicSpriteFragmentShaderName{"classic_sprite_fragment.glsl"};

// Data use in main program for the updated display mode
struct UpdatedData {
  // std::array<unsigned char, wizard_level::kTileBufferSize> tile_set{};
  // std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> tile_buffer{};
  // std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> color_buffer{};
  // std::array<unsigned char, alatar_classic::kSpriteProcDataSize> processed_sprites{};

  // alatar_classic::ClassicTileGLBuffers tile_glbuffers;
  // alatar_classic::ClassicSpriteGLBuffers sprite_glbuffers;

  BurningLogic::ShaderVars tile_shader;
  BurningLogic::ShaderVars sprite_shader;
};

bool SetupUpdated(UpdatedData& data, std::filesystem::path resource_path, std::filesystem::path shader_path);

void UpdatedLevelInit(void);

}  // namespace alatar_updated

#endif
