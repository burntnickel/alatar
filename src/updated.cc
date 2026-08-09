
#include "updated.h"

#include "c64_clut.h"

namespace alatar_updated {

bool SetupUpdated(UpdatedData& data, std::filesystem::path resource_path, std::filesystem::path shader_path) {
  /*const std::string kClassicDirName{"classic"};
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
