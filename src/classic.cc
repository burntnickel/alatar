#include "classic.h"

#include <iostream>
#include <memory>
#include <utility>

#include "c64_clut.h"
#include "globals.h"

namespace alatar_classic {

//--------------------------------------------------------------------
// ClassicClass implementation
//--------------------------------------------------------------------

// Move constructor
ClassicClass::ClassicClass(ClassicClass&& other) {
  data_ = other.data_;
}

// Move assignment operator
ClassicClass& ClassicClass::operator=(ClassicClass&& other) {
  if (this != &other) {
    data_ = other.data_;
  }

  return *this;
}

// Destructor
ClassicClass::~ClassicClass(void) {
  // Delete tile shader opengl constructs
  glDeleteBuffers(1, &(data_.tile_shader.vbo));
  glDeleteBuffers(1, &(data_.tile_shader.ibo));
  glDeleteVertexArrays(1, &(data_.tile_shader.vao));
  glDeleteProgram(data_.tile_shader.program);

  // Delete sprite shader opengl constructs
  glDeleteBuffers(1, &(data_.sprite_shader.vbo));
  glDeleteBuffers(1, &(data_.sprite_shader.ibo));
  glDeleteVertexArrays(1, &(data_.sprite_shader.vao));
  glDeleteProgram(data_.sprite_shader.program);
};

// Private constructor for factory
ClassicClass::ClassicClass(Token, const ClassicData& data) {
  data_ = data;
}

void ClassicClass::LevelInit(const alatar::LevelClass& level) {
  // Set to 32 (space) as blank character & black (0)
  for (unsigned int ii = 0; ii < (alatar::kRowTiles * alatar::kColTiles); ++ii) {
    data_.tile_buffer[ii] = 32;
    data_.color_buffer[ii] = 0;
  }

  for (int row = 0; row < alatar::kTileDataRows; ++row) {
    for (int col = 0; col < alatar::kTileDataCols; ++col) {
      unsigned int screen_row = static_cast<unsigned int>(row) + 1;
      unsigned int screen_col = static_cast<unsigned int>(col);
      unsigned int screen_index = alatar::kColTiles * screen_row + screen_col;

      unsigned char tile = level.GetTileAt(row, col);

      data_.tile_buffer[screen_index] = tile;
      data_.color_buffer[screen_index] = level.GetTileColor(tile);
    }
  }
}

void ClassicClass::Update(Uint64 counter, const alatar::LevelClass& level) {
  // Update graphics (these probably don't upate at 60 Hz, need to get the correct number)
  // I think color changes faster then the fire animation
  if ((static_cast<double>(counter - data_.treasure_color_cycle_counter) * alatar::gCounterToMsScale) >
      kTreasureColorCycleFrameTimeMs) {
    ++data_.treasure_color_cycle;
    data_.treasure_color_cycle_counter = counter;
  }

  unsigned char treasure_color_idx = data_.treasure_color_cycle.GetValue();

  if ((static_cast<double>(counter - data_.fire_color_cycle_counter) * alatar::gCounterToMsScale) >
      kFireColorCycleFrameTimeMs) {
    ++data_.fire_color_cycle;
    data_.fire_color_cycle_counter = counter;
  }

  unsigned char fire_color_idx = data_.fire_color_cycle.GetValue();

  // Set updated fire and treasure colors
  for (int row = 0; row < alatar::kTileDataRows; ++row) {
    for (int col = 0; col < alatar::kTileDataCols; ++col) {
      unsigned int screen_row = static_cast<unsigned int>(row) + 1;
      unsigned int screen_col = static_cast<unsigned int>(col);
      unsigned int screen_index = alatar::kColTiles * screen_row + screen_col;

      unsigned char tile = level.GetTileAt(row, col);

      if (alatar::GetTileGroup(tile) == alatar::kTreasure) {
        data_.color_buffer[screen_index] = treasure_color_idx;
      }

      if (alatar::GetTileGroup(tile) == alatar::kFire) {
        data_.color_buffer[screen_index] = fire_color_idx;
      }
    }
  }

  // Cycle fire tile charaters for animation
  if ((static_cast<double>(counter - data_.fire_animation_counter) * alatar::gCounterToMsScale) >
      kFireAnimationFrameTimeMs) {
    data_.fire_animation_counter = counter;

    for (int row = 0; row < alatar::kTileDataRows; ++row) {
      for (int col = 0; col < alatar::kTileDataCols; ++col) {
        unsigned int screen_row = static_cast<unsigned int>(row) + 1;
        unsigned int screen_col = static_cast<unsigned int>(col);
        unsigned int screen_index = alatar::kColTiles * screen_row + screen_col;

        unsigned char tile = level.GetTileAt(row, col);
        if (alatar::GetTileGroup(tile) == alatar::kFire) {
          data_.fire_animation_counter = counter;
          auto tmp_tile = data_.tile_buffer[screen_index];
          tmp_tile = tmp_tile + 1;

          if (tmp_tile > kFireTileLast) {
            tmp_tile = kFireTileFirst;
          }

          data_.tile_buffer[screen_index] = tmp_tile;
        }
      }
    }
  }

  glBindBuffer(GL_TEXTURE_BUFFER, data_.tile_glbuffers.color_buffer);
  BurningLogic::PrintGLError("main:glBindBuffer");

  glBufferSubData(GL_TEXTURE_BUFFER, 0, static_cast<GLsizeiptr>(data_.color_buffer.size()),
                  data_.color_buffer.data());
  BurningLogic::PrintGLError("main:glBufferSubData");

  glBindBuffer(GL_TEXTURE_BUFFER, data_.tile_glbuffers.tile_buffer);
  BurningLogic::PrintGLError("main:glBindBuffer");

  glBufferSubData(GL_TEXTURE_BUFFER, 0, static_cast<GLsizeiptr>(data_.tile_buffer.size()),
                  data_.tile_buffer.data());
  BurningLogic::PrintGLError("main:glBufferSubData");
}

void ClassicClass::Draw(const alatar::MonsterClassArray& monster_info, const alatar::WizardInfo& wizard_info,
                        const BurningLogic::mat4& view_matrix) {
  // Draw monster sprites (ones with lower priority)
  for (unsigned int ii = 0; ii < 6; ++ii) {
    unsigned int idx = 5 - ii;

    if (monster_info[idx]->IsActive() && !monster_info[idx]->GetPriority()) {
      DrawClassicSpriteC64(data_.sprite_shader, view_matrix, data_.sprite_glbuffers,
                           monster_info[idx]->sprite_id_, monster_info[idx]->x_, monster_info[idx]->y_,
                           {{alatar::kColorLightBlue, monster_info[idx]->GetColor(), alatar::kColorWhite}});
    }
  }

  PaintClassicTiles(data_.tile_shader, view_matrix, SDL_FRect({-1.0, -1.0, 2.0, 2.0}), data_.tile_glbuffers);

  // Draw monster sprites (ones with higher priority)
  for (unsigned int ii = 0; ii < 6; ++ii) {
    unsigned int idx = 5 - ii;

    if (monster_info[idx]->IsActive() && monster_info[idx]->GetPriority()) {
      DrawClassicSpriteC64(data_.sprite_shader, view_matrix, data_.sprite_glbuffers,
                           monster_info[idx]->sprite_id_, monster_info[idx]->x_, monster_info[idx]->y_,
                           {{alatar::kColorLightBlue, monster_info[idx]->GetColor(), alatar::kColorWhite}});
    }
  }

  // Draw wizard sprite
  DrawClassicSpriteC64(data_.sprite_shader, view_matrix, data_.sprite_glbuffers, 0, wizard_info.x,
                       wizard_info.y, {{alatar::kColorLightBlue, wizard_info.color, alatar::kColorWhite}});
}

std::optional<alatar::GraphicsCommonPtr> ClassicClass::ClassicClassFactory(
    std::filesystem::path resource_path, std::filesystem::path shader_path, Uint64 sdl_counter) {
  ClassicData data;

  const std::string kClassicDirName{"classic"};
  const std::string kCharSetName{"chrw"};
  const std::string kSpriteSetName{"sprw"};

  const auto tile_set_path_and_name = resource_path / kClassicDirName / kCharSetName;
  const auto sprite_set_path_and_name = resource_path / kClassicDirName / kSpriteSetName;

  bool success = LoadCharSet(tile_set_path_and_name, data.tile_set);

  if (!success) {
    std::cerr << "Failed to load classic charater set\n";
    return {};
  }

  success = LoadSpriteData(sprite_set_path_and_name, data.processed_sprites);

  if (!success) {
    std::cerr << "Failed to load classic sprite set\n";
    return {};
  }

  // Tile buffer
  BurningLogic::TextureSetupHelper(kClassicTileTextureUnit, &data.tile_glbuffers.tile_buffer,
                                   data.tile_buffer, GL_DYNAMIC_DRAW, &data.tile_glbuffers.tile_texture,
                                   GL_R8UI);

  // Color buffer
  BurningLogic::TextureSetupHelper(kClassicColorTextureUnit, &data.tile_glbuffers.color_buffer,
                                   data.color_buffer, GL_DYNAMIC_DRAW, &data.tile_glbuffers.color_texture,
                                   GL_R8UI);

  // Tile set
  BurningLogic::TextureSetupHelper(kClassicTilesetTextureUnit, &data.tile_glbuffers.tileset_buffer,
                                   data.tile_set, GL_STATIC_DRAW, &data.tile_glbuffers.tileset_texture,
                                   GL_R8UI);

  // CLUT
  BurningLogic::TextureSetupHelper(kClassicClutTextureUnit, &data.tile_glbuffers.clut_buffer, kDefaultC64Clut,
                                   GL_STATIC_DRAW, &data.tile_glbuffers.clut_texture, GL_RGBA8UI);

  // Sprite set
  BurningLogic::TextureSetupHelper(kClassicSpriteTextureUnit, &data.sprite_glbuffers.sprite_buffer,
                                   data.processed_sprites, GL_STATIC_DRAW,
                                   &data.sprite_glbuffers.sprite_texture, GL_R8UI);

  data.sprite_glbuffers.clut_buffer = data.tile_glbuffers.clut_buffer;
  data.sprite_glbuffers.clut_texture = data.tile_glbuffers.clut_texture;

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

  // Set up inital animation counters
  data.treasure_color_cycle_counter = sdl_counter;
  data.fire_color_cycle_counter = sdl_counter;
  data.fire_animation_counter = sdl_counter;

  return std::make_unique<ClassicClass>(Token{}, data);
}

}  // namespace alatar_classic
