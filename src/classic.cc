#include "classic.h"

#include <iostream>

#include "c64_clut.h"

namespace alatar_classic {

bool SetupClassic(ClassicData& data, std::filesystem::path resource_path, std::filesystem::path shader_path) {
  const std::string kClassicDirName{"classic"};
  const std::string kCharSetName{"chrw"};
  const std::string kSpriteSetName{"sprw"};

  auto tile_set_path_and_name = resource_path / kClassicDirName / kCharSetName;
  auto sprite_set_path_and_name = resource_path / kClassicDirName / kSpriteSetName;

  bool success = alatar_classic::LoadCharSet(tile_set_path_and_name, data.tile_set);

  if (!success) {
    std::cerr << "Failed to load classic charater set\n";
    return false;
  }

  success = alatar_classic::LoadSpriteData(sprite_set_path_and_name, data.processed_sprites);

  if (!success) {
    std::cerr << "Failed to load classic sprite set\n";
    return false;
  }

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
                                   GL_R8UI);

  // CLUT
  BurningLogic::TextureSetupHelper(classic_clut_texture, &data.tile_glbuffers.tbo_clut_buffer,
                                   kDefaultC64Clut, GL_STATIC_DRAW, &data.tile_glbuffers.tbo_tex_clut_buffer,
                                   GL_RGBA8UI);

  // Sprite set
  BurningLogic::TextureSetupHelper(classic_sprite_texture, &data.sprite_glbuffers.tbo_sprite_buffer,
                                   data.processed_sprites, GL_STATIC_DRAW,
                                   &data.sprite_glbuffers.tbo_tex_sprite_buffer, GL_R8UI);

  data.sprite_glbuffers.tbo_clut_buffer = data.tile_glbuffers.tbo_clut_buffer;
  data.sprite_glbuffers.tbo_tex_clut_buffer = data.tile_glbuffers.tbo_tex_clut_buffer;

  // Tile shaders
  const std::filesystem::path kClassicTileVertexShaderFilename = shader_path / kClassicTileVertexShaderName;
  const std::filesystem::path kClassicTileFragmentShaderFilename =
      shader_path / kClassicTileFragmentShaderName;

  const std::string classic_tile_vertex_shader_source =
      BurningLogic::LoadShaderSource(kClassicTileVertexShaderFilename);
  const std::string classic_tile_fragment_shader_source =
      BurningLogic::LoadShaderSource(kClassicTileFragmentShaderFilename);

  data.tile_shader = BurningLogic::BuildShaderProgram(classic_tile_vertex_shader_source,
                                                      classic_tile_fragment_shader_source);

  // Sprite shaders
  const std::filesystem::path kClassicSpriteVertexShaderFilename =
      shader_path / kClassicSpriteVertexShaderName;
  const std::filesystem::path kClassicSpriteFragmentShaderFilename =
      shader_path / kClassicSpriteFragmentShaderName;

  const std::string classic_sprite_vertex_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteVertexShaderFilename);
  const std::string classic_sprite_fragment_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteFragmentShaderFilename);

  data.sprite_shader = BurningLogic::BuildShaderProgram(classic_sprite_vertex_shader_source,
                                                        classic_sprite_fragment_shader_source);

  return true;
}

void ClassicLevelInit(ClassicData& data, const wizard_level::LevelClass& level) {
  // Set to 32 (space) as blank character & black (0)
  for (unsigned int ii = 0; ii < (wizard_level::kRowTiles * wizard_level::kColTiles); ++ii) {
    data.tile_buffer[ii] = 32;
    data.color_buffer[ii] = 0;
  }

  for (int row = 0; row < wizard_level::kTileDataRows; ++row) {
    for (int col = 0; col < wizard_level::kTileDataCols; ++col) {
      unsigned int screen_row = static_cast<unsigned int>(row) + 1;
      unsigned int screen_col = static_cast<unsigned int>(col);
      unsigned int screen_index = wizard_level::kColTiles * screen_row + screen_col;

      unsigned char tile = level.GetTileAt(row, col);

      data.tile_buffer[screen_index] = tile;
      data.color_buffer[screen_index] = level.GetTileColor(tile);
    }
  }
}

}  // namespace alatar_classic
