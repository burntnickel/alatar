
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

void UpdatedClass::LevelInit([[maybe_unused]] const alatar::LevelClass& level) {}

void UpdatedClass::Update([[maybe_unused]] Uint64 start_counter, [[maybe_unused]] double counter_to_ms_scale,
                          [[maybe_unused]] const alatar::LevelClass& level) {}

void UpdatedClass::Draw([[maybe_unused]] const alatar::MonsterClassArray& monster_info,
                        [[maybe_unused]] const alatar::WizardInfo& wizard_info,
                        const BurningLogic::mat4& view_matrix) {
  alatar_updated::PaintUpdatedTiles(data_.tile_shader, view_matrix, data_.vertex_manager,
                                    data_.tile_glbuffers);
}

std::optional<alatar::GraphicsCommonPtr> UpdatedClass::UpdatedClassFactory(
    std::filesystem::path resource_path, std::filesystem::path shader_path,
    [[maybe_unused]] Uint64 sdl_counter) {
  UpdatedData data;

  const std::string kUpdatedDirName{"updated"};
  const std::string kWallTextureName{"wall_texture.png"};

  auto wall_texture_path_and_name = resource_path / kUpdatedDirName / kWallTextureName;

  bool success = LoadWallTexture(wall_texture_path_and_name, data);

  if (!success) {
    std::cerr << "Failed to load wall texture\n";
    return {};
  }

  // Wall texture
  BurningLogic::PrintGLError("UpdatedClassFactory:Before I Do Anything!");

  GLuint wall_texture;
  glGenTextures(1, &wall_texture);  // TODO: Going to need to add corresponding deletes I guess (also for
                                    // "legacy" cases) Maybe?
  // BurningLogic::PrintGLError("UpdatedClassFactory:glGenTextures");
  std::cout << wall_texture << "\n";

  glGenTextures(1, &wall_texture);
  std::cout << wall_texture << "\n";
  glBindTexture(GL_TEXTURE_2D, wall_texture);
  BurningLogic::PrintGLError("UpdatedClassFactory:glBindTexture");

  glActiveTexture(kUpdatedWallTextureUnit);
  BurningLogic::PrintGLError("UpdatedClassFactory:glActiveTexture");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_WRAP_S");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_WRAP_T");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_MIN_FILTER");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_MAG_FILTER");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.wall_texture_surface->w, data.wall_texture_surface->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, data.wall_texture_surface->pixels);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexImage2D");

  // glGenerateMipmap(GL_TEXTURE_2D);

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
  BurningLogic::TextureSetupHelper(kUpdatedClutTextureUnit, &data.tile_glbuffers.tbo_clut_buffer,
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

}  // namespace alatar_updated
