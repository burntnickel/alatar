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
const std::string kUpdatedTileFragmentShaderName{"updated_tile_fragment.glsl"};
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

// Data use in main program for the updated display mode
struct UpdatedData {
  std::array<unsigned char, kLevelTileCount> tile_buffer;
  std::array<unsigned char, kLevelTileCount> color_buffer;

  // std::array<unsigned char, wizard_level::kTileBufferSize> tile_set{};
  // std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> tile_buffer{};
  // std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> color_buffer{};
  // std::array<unsigned char, alatar_classic::kSpriteProcDataSize> processed_sprites{};

  UpdatedTileGLBuffers tile_glbuffers;
  // alatar_classic::ClassicSpriteGLBuffers sprite_glbuffers;

  BurningLogic::ShaderVars tile_shader;
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
