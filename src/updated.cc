
#include "updated.h"

#include <SDL_image.h>

#include <iostream>

#include "c64_clut.h"
#include "sdl_helper.h"

namespace alatar_updated {

static bool LoadWallTexture(std::filesystem::path path_and_name, UpdatedData& data) {
  SDL_Surface* raw_surface = IMG_Load(path_and_name.c_str());

  bool success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling IMG_Load:");

  // Convert to the required RGBA format here
  SDL_Surface* converted_surface;

  if (success) {
    converted_surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA8888, 0);

    success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling SDL_ConvertSurfaceFormat:");
  }

  if (raw_surface != NULL) {
    SDL_FreeSurface(raw_surface);
  }

  if (success) {
    data.wall_texture_surface = converted_surface;
  }

  // TODO: Need to add another function to do the clean up for make the UpdatedData a class with a destructor

  return success;
}

//--------------------------------------------------------------------
// UpdatedClass implementation
//--------------------------------------------------------------------

// Move constructor
UpdatedClass::UpdatedClass(UpdatedClass&& other) {
  data_ = other.data_;
}

// Move assignment operator
UpdatedClass& UpdatedClass::operator=(UpdatedClass&& other) {
  if (this != &other) {
    data_ = other.data_;
  }

  return *this;
}

// Destructor
UpdatedClass::~UpdatedClass(void) {
  // Delete tile shader opengl constructs
  glDeleteBuffers(1, &(data_.tile_shader.vbo));
  glDeleteBuffers(1, &(data_.tile_shader.ibo));
  glDeleteVertexArrays(1, &(data_.tile_shader.vao));
  glDeleteProgram(data_.tile_shader.program);

  /*// Delete sprite shader opengl constructs
  glDeleteBuffers(1, &(data_.sprite_shader.vbo));
  glDeleteBuffers(1, &(data_.sprite_shader.ibo));
  glDeleteVertexArrays(1, &(data_.sprite_shader.vao));
  glDeleteProgram(data_.sprite_shader.program);*/

  // Clean up SDL stuff used for textures
  SDL_FreeSurface(data_.wall_texture_surface);
};

// Private constructor for factory
UpdatedClass::UpdatedClass(Token, const UpdatedData& data) {
  data_ = data;
}

void UpdatedClass::LevelInit(const wizard_level::LevelClass& level) {}

void UpdatedClass::Update(Uint64 start_counter, double counter_to_ms_scale,
                          const wizard_level::LevelClass& level) {}

void UpdatedClass::Draw(const wizard_level::MonsterInfoArray& monster_info,
                        const wizard_level::WizardInfo& wizard_info, const BurningLogic::mat4& view_matrix) {
  alatar_updated::PaintUpdatedTiles(data_.tile_shader, view_matrix, data_.vertex_manager,
                                    data_.tile_glbuffers);
}

std::optional<alatar::GraphicsCommonPtr> UpdatedClass::UpdatedClassFactory(
    std::filesystem::path resource_path, std::filesystem::path shader_path, Uint64 sdl_counter) {
  UpdatedData data;

  const std::string kUpdatedDirName{"updated"};
  const std::string kWallTextureName{"wall_texture.png"};

  auto wall_texture_path_and_name = resource_path / kUpdatedDirName / kWallTextureName;

  bool success = LoadWallTexture(wall_texture_path_and_name, data);

  if (!success) {
    std::cerr << "Failed to load wall texture\n";
    return {};
  }

  /*

  // Tile buffer
  BurningLogic::TextureSetupHelper(classic_tile_texture, &data.tile_glbuffers.tbo_tile_buffer,
                                   data.tile_buffer, GL_DYNAMIC_DRAW,
                                   &data.tile_glbuffers.tbo_tex_tile_buffer, GL_R8UI);

  // Color buffer
  BurningLogic::TextureSetupHelper(classic_color_texture, &data.tile_glbuffers.tbo_color_buffer,
                                   data.color_buffer, GL_DYNAMIC_DRAW,
                                   &data.tile_glbuffers.tbo_tex_color_buffer, GL_R8UI);

  // Tile set
  BurningLogic::TextureSetupHelper(classic_tileset_texture, &data.tile_glbuffers.tbo_tileset_buffer,
                                   data.tile_set, GL_STATIC_DRAW, &data.tile_glbuffers.tbo_tex_tileset_buffer,
                                   GL_R8UI);*/

  // CLUT
  BurningLogic::TextureSetupHelper(updated_clut_texture, &data.tile_glbuffers.tbo_clut_buffer,
                                   kDefaultC64Clut, GL_STATIC_DRAW, &data.tile_glbuffers.tbo_tex_clut_buffer,
                                   GL_RGBA8UI);

  /* // Sprite set
   BurningLogic::TextureSetupHelper(classic_sprite_texture, &data.sprite_glbuffers.tbo_sprite_buffer,
                                    data.processed_sprites, GL_STATIC_DRAW,
                                    &data.sprite_glbuffers.tbo_tex_sprite_buffer, GL_R8UI);

   data.sprite_glbuffers.tbo_clut_buffer = data.tile_glbuffers.tbo_clut_buffer;
   data.sprite_glbuffers.tbo_tex_clut_buffer = data.tile_glbuffers.tbo_tex_clut_buffer;*/

  // Tile shaders
  const std::filesystem::path kTileVertexShaderFilename = shader_path / kUpdatedTileVertexShaderName;
  const std::filesystem::path kTileFragmentShaderFilename = shader_path / kUpdatedTileFragmentShaderName;

  const std::string tile_vertex_shader_source = BurningLogic::LoadShaderSource(kTileVertexShaderFilename);
  const std::string tile_fragment_shader_source = BurningLogic::LoadShaderSource(kTileFragmentShaderFilename);

  data.tile_shader = BurningLogic::BuildShaderProgram(tile_vertex_shader_source, tile_fragment_shader_source);

  /*// Sprite shaders
  const std::filesystem::path kClassicSpriteVertexShaderFilename =
      shader_path / kClassicSpriteVertexShaderName;
  const std::filesystem::path kClassicSpriteFragmentShaderFilename =
      shader_path / kClassicSpriteFragmentShaderName;

  const std::string classic_sprite_vertex_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteVertexShaderFilename);
  const std::string classic_sprite_fragment_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteFragmentShaderFilename);

  data.sprite_shader = BurningLogic::BuildShaderProgram(classic_sprite_vertex_shader_source,
                                                        classic_sprite_fragment_shader_source);*/

  return std::make_unique<UpdatedClass>(Token{}, data);
}

//--------------

bool SetupUpdated(UpdatedData& data, std::filesystem::path resource_path, std::filesystem::path shader_path) {
  const std::string kUpdatedDirName{"updated"};
  const std::string kWallTextureName{"wall_texture.png"};

  auto wall_texture_path_and_name = resource_path / kUpdatedDirName / kWallTextureName;

  bool success = LoadWallTexture(wall_texture_path_and_name, data);

  if (!success) {
    std::cerr << "Failed to load wall texture\n";
    return false;
  }

  /*

  // Tile buffer
  BurningLogic::TextureSetupHelper(classic_tile_texture, &data.tile_glbuffers.tbo_tile_buffer,
                                   data.tile_buffer, GL_DYNAMIC_DRAW,
                                   &data.tile_glbuffers.tbo_tex_tile_buffer, GL_R8UI);

  // Color buffer
  BurningLogic::TextureSetupHelper(classic_color_texture, &data.tile_glbuffers.tbo_color_buffer,
                                   data.color_buffer, GL_DYNAMIC_DRAW,
                                   &data.tile_glbuffers.tbo_tex_color_buffer, GL_R8UI);

  // Tile set
  BurningLogic::TextureSetupHelper(classic_tileset_texture, &data.tile_glbuffers.tbo_tileset_buffer,
                                   data.tile_set, GL_STATIC_DRAW, &data.tile_glbuffers.tbo_tex_tileset_buffer,
                                   GL_R8UI);*/

  // CLUT
  BurningLogic::TextureSetupHelper(updated_clut_texture, &data.tile_glbuffers.tbo_clut_buffer,
                                   kDefaultC64Clut, GL_STATIC_DRAW, &data.tile_glbuffers.tbo_tex_clut_buffer,
                                   GL_RGBA8UI);

  /* // Sprite set
   BurningLogic::TextureSetupHelper(classic_sprite_texture, &data.sprite_glbuffers.tbo_sprite_buffer,
                                    data.processed_sprites, GL_STATIC_DRAW,
                                    &data.sprite_glbuffers.tbo_tex_sprite_buffer, GL_R8UI);

   data.sprite_glbuffers.tbo_clut_buffer = data.tile_glbuffers.tbo_clut_buffer;
   data.sprite_glbuffers.tbo_tex_clut_buffer = data.tile_glbuffers.tbo_tex_clut_buffer;*/

  // Tile shaders
  const std::filesystem::path kTileVertexShaderFilename = shader_path / kUpdatedTileVertexShaderName;
  const std::filesystem::path kTileFragmentShaderFilename = shader_path / kUpdatedTileFragmentShaderName;

  const std::string tile_vertex_shader_source = BurningLogic::LoadShaderSource(kTileVertexShaderFilename);
  const std::string tile_fragment_shader_source = BurningLogic::LoadShaderSource(kTileFragmentShaderFilename);

  data.tile_shader = BurningLogic::BuildShaderProgram(tile_vertex_shader_source, tile_fragment_shader_source);

  /*// Sprite shaders
  const std::filesystem::path kClassicSpriteVertexShaderFilename =
      shader_path / kClassicSpriteVertexShaderName;
  const std::filesystem::path kClassicSpriteFragmentShaderFilename =
      shader_path / kClassicSpriteFragmentShaderName;

  const std::string classic_sprite_vertex_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteVertexShaderFilename);
  const std::string classic_sprite_fragment_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteFragmentShaderFilename);

  data.sprite_shader = BurningLogic::BuildShaderProgram(classic_sprite_vertex_shader_source,
                                                        classic_sprite_fragment_shader_source);*/

  return true;
}

void UpdatedDraw(const UpdatedData& updated_data, const wizard_level::MonsterInfoArray& monster_info,
                 const wizard_level::WizardInfo& wizard_info, const BurningLogic::mat4& view_matrix) {
  alatar_updated::PaintUpdatedTiles(updated_data.tile_shader, view_matrix, updated_data.vertex_manager,
                                    updated_data.tile_glbuffers);

  /*// Draw monster sprites
  for (unsigned int ii = 0; ii < 6; ++ii) {
    if (monster_info[ii].active) {
      alatar_classic::DrawClassicSpriteC64(
          classic_data.sprite_shader, view_matrix, classic_data.sprite_glbuffers, monster_info[ii].sprite_id,
          monster_info[ii].x, monster_info[ii].y,
          {{wizard_level::kColorLightBlue, monster_info[ii].color, wizard_level::kColorWhite}});
    }
  }

  // Draw wizard sprite
  alatar_classic::DrawClassicSpriteC64(
      classic_data.sprite_shader, view_matrix, classic_data.sprite_glbuffers, 0, wizard_info.x, wizard_info.y,
      {{wizard_level::kColorLightBlue, wizard_info.color, wizard_level::kColorWhite}});*/
}

}  // namespace alatar_updated
