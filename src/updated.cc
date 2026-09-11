
#include "updated.h"

// #include <SDL_image.h>

#include <iostream>
#include <set>

#include "c64_clut.h"
#include "sdl_helper.h"

namespace alatar_updated {

/*static bool LoadToSurfaceRGBA(std::filesystem::path path_and_name, SDL_Surface*& target_surface) {
  SDL_Surface* raw_surface = IMG_Load(path_and_name.c_str());
  bool success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling IMG_Load:");

  // Convert to the required RGBA format here
  SDL_Surface* converted_surface;

  if (success) {
    converted_surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA32, 0);
    success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling SDL_ConvertSurfaceFormat:");
  }

  if (raw_surface != NULL) {
    SDL_FreeSurface(raw_surface);
  }

  if (success) {
    target_surface = converted_surface;
  }

  return success;
}

static bool LoadToSurface1Chan(std::filesystem::path path_and_name, SDL_Surface*& target_surface) {
  SDL_Surface* raw_surface = IMG_Load(path_and_name.c_str());
  bool success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling IMG_Load:");

  if (success) {
    // Convert to single 8 bit channel here
    SDL_Surface* tmp_surface = SDL_CreateRGBSurface(0, 1, 1, 8, 0, 0, 0, 0);
    // SDL_PixelFormat* index_8_format = tmp_surface->format;

    std::array<SDL_Color, 256> colors;

    for (unsigned int i = 0; i < 256; ++i) {
      colors[i].r = static_cast<Uint8>(i);
      colors[i].g = static_cast<Uint8>(i);
      colors[i].b = static_cast<Uint8>(i);
      colors[i].a = 255;
    }

    SDL_SetPaletteColors(tmp_surface->format->palette, colors.data(), 0, 256);

    target_surface = SDL_ConvertSurface(raw_surface, tmp_surface->format, 0);

    SDL_FreeSurface(tmp_surface);
    SDL_FreeSurface(raw_surface);
  }

  return success;
}*/

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
  // Delete tile wall shader opengl constructs
  glDeleteBuffers(1, &(data_.tile_wall_shader.vbo));
  glDeleteBuffers(1, &(data_.tile_wall_shader.ibo));
  glDeleteVertexArrays(1, &(data_.tile_wall_shader.vao));
  glDeleteProgram(data_.tile_wall_shader.program);

  // Delete tile misc shader opengl constructs
  glDeleteBuffers(1, &(data_.tile_misc_shader.vbo));
  glDeleteBuffers(1, &(data_.tile_misc_shader.ibo));
  glDeleteVertexArrays(1, &(data_.tile_misc_shader.vao));
  glDeleteProgram(data_.tile_misc_shader.program);

  /*// Delete sprite shader opengl constructs
  glDeleteBuffers(1, &(data_.sprite_shader.vbo));
  glDeleteBuffers(1, &(data_.sprite_shader.ibo));
  glDeleteVertexArrays(1, &(data_.sprite_shader.vao));
  glDeleteProgram(data_.sprite_shader.program);*/

  // Clean up SDL stuff used for textures
  SDL_FreeSurface(data_.wall_texture_surface);
  SDL_FreeSurface(data_.tiles_and_masks_surface);
};

// Private constructor for factory
UpdatedClass::UpdatedClass(Token, const UpdatedData& data) {
  data_ = data;
}

void UpdatedClass::LevelInit([[maybe_unused]] const alatar::LevelClass& level) {
  // Initalize the buffers used to pass level data to the shader
  for (auto& array_entry : data_.tile_buffer) {
    array_entry = 0;
  }

  for (auto& array_entry : data_.color_buffer) {
    array_entry = 0;
  }
}

void UpdatedClass::UpdateWalls(const alatar::LevelClass& level) {
  for (int row = 0; row < alatar::kTileDataRows; ++row) {
    for (int col = 0; col < alatar::kTileDataCols; ++col) {
      unsigned int screen_row = static_cast<unsigned int>(row) + 1;
      unsigned int screen_col = static_cast<unsigned int>(col);
      unsigned int screen_index = alatar::kColTiles * screen_row + screen_col;

      unsigned char tile = level.GetTileAt(row, col);
      alatar::MaskTiles mask_tiles = level.GetMaskTiles(row, col);
      auto mask_index = GetWallTileMaskIndex(mask_tiles);

      // Tile and mask updates
      unsigned char mask;

      switch (tile) {
        case 91:  // Wall
          mask = kWallMaskSmoothTable[mask_index];
          break;
        case 92:  // Floor
          mask = kWallMaskSmoothTable[mask_index];
          break;
        case 93:  // Steep stair left
          mask = kSteepStairLeftSmoothTable[mask_index];
          break;
        case 94:  // Steep stair right
          mask = kSteepStairRightSmoothTable[mask_index];
          break;
        case 95:  // Shallow stair left (left part)
                  // mask = 12;
          mask = kShallowStairLeft1SmoothTable[mask_index];
          break;
        case 96:  // Shallow stair left (right part)
                  // mask = 14;
          mask = kShallowStairLeft2SmoothTable[mask_index];
          break;
        case 97:  // Shallow stairs right (left part)
                  // mask = 16;
          mask = kShallowStairRight2SmoothTable[mask_index];
          break;
        case 98:  // Shallow stairs right (right part)
                  // mask = 18;
          mask = kShallowStairRight1SmoothTable[mask_index];
          break;
        case 122:  // Slide left
                   // mask = 20;
          mask = kSlideLeftSmoothTable[mask_index];
          break;
        case 123:  // SLide right
          // mask = 22;
          mask = kSlideRightSmoothTable[mask_index];
          break;
        default:
          mask = kMaskTileBlank;
      }

      data_.tile_buffer[kWallMaskOffset + screen_index] = mask;

      // Color buffer updates (only do this once here)
      data_.color_buffer[screen_index] = level.GetTileColor(tile);
    }
  }
}

void UpdatedClass::UpdateMiscTiles(const alatar::LevelClass& level) {
  const std::set<unsigned char> kSkipTiles{91, 92, 93, 94, 95, 96, 97, 98, 122, 123};

  for (int row = 0; row < alatar::kTileDataRows; ++row) {
    for (int col = 0; col < alatar::kTileDataCols; ++col) {
      unsigned int screen_row = static_cast<unsigned int>(row) + 1;
      unsigned int screen_col = static_cast<unsigned int>(col);
      unsigned int screen_index = alatar::kColTiles * screen_row + screen_col;

      unsigned char tile = level.GetTileAt(row, col);

      // TODO: add context like for the walls to correctly draw just vertical parts

      unsigned char new_tile;

      if (!kSkipTiles.contains(tile)) {
        // Tile and mask updates

        switch (tile) {
          case 101:  // Ladder left
            new_tile = kLadderTileComboLeft;
            break;
          case 102:  // Ladder middle
            new_tile = kLadderTileHorizontal;
            break;
          case 103:  // Ladder right
            new_tile = kLadderTileComboRight;
            break;
          default:
            //new_tile = kMaskTileBlank;
            new_tile = tile;
        }
      } else {
        new_tile = kMaskTileBlank;
      }

      data_.tile_buffer[kTileMiscOffset + screen_index] = new_tile;
    }
  }
}

void UpdatedClass::Update([[maybe_unused]] Uint64 counter, const alatar::LevelClass& level) {
  UpdateWalls(level);
  UpdateMiscTiles(level);

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

  /*// Set updated fire and treasure colors
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
  }*/
}

void UpdatedClass::Draw([[maybe_unused]] const alatar::MonsterClassArray& monster_info,
                        [[maybe_unused]] const alatar::WizardInfo& wizard_info,
                        const BurningLogic::mat4& view_matrix) {
  alatar_updated::PaintUpdatedTilesWalls(data_.tile_wall_shader, view_matrix, data_.vertex_manager,
                                         data_.tile_glbuffers);
  alatar_updated::PaintUpdatedTilesMisc(data_.tile_misc_shader, view_matrix, data_.vertex_manager,
                                        data_.tile_glbuffers);
}

std::optional<alatar::GraphicsCommonPtr> UpdatedClass::UpdatedClassFactory(
    std::filesystem::path resource_path, std::filesystem::path shader_path,
    [[maybe_unused]] Uint64 sdl_counter) {
  UpdatedData data;

  const std::string kUpdatedDirName{"updated"};
  const std::string kWallTextureName{"wall_texture.png"};
  const std::string kTilesAndMasksName{"tiles_and_masks.png"};

  auto wall_texture_path_and_name = resource_path / kUpdatedDirName / kWallTextureName;
  bool success = LoadToSurfaceRGBA(wall_texture_path_and_name, data.wall_texture_surface);

  if (!success) {
    std::cerr << "Failed to load wall texture\n";
    return {};
  }

  auto tiles_and_masks_path_and_name = resource_path / kUpdatedDirName / kTilesAndMasksName;
  // success = LoadToSurface1Chan(tiles_and_masks_path_and_name, data.tiles_and_masks_surface);
  success = LoadToSurfaceRGBA(tiles_and_masks_path_and_name, data.tiles_and_masks_surface);

  if (!success) {
    std::cerr << "Failed to load tile and masks texture\n";
    return {};
  }

  // ------------------------------------------------------------------------------------------------
  // Wall texture
  // ------------------------------------------------------------------------------------------------
  glActiveTexture(kUpdatedWallTextureUnit);
  BurningLogic::PrintGLError("UpdatedClassFactory:glActiveTexture");

  glGenTextures(1, &data.tile_glbuffers.wall_texture);  // TODO: Going to need to add corresponding deletes I
                                                        // guess (also for "legacy" cases) Maybe?
  BurningLogic::PrintGLError("UpdatedClassFactory:glGenTextures");

  glBindTexture(GL_TEXTURE_2D, data.tile_glbuffers.wall_texture);
  BurningLogic::PrintGLError("UpdatedClassFactory:glBindTexture");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_WRAP_S");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_WRAP_T");

  // Mipmap filter mode means I need to have mipmaps generated
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_MIN_FILTER");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_MAG_FILTER");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.wall_texture_surface->w, data.wall_texture_surface->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, data.wall_texture_surface->pixels);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexImage2D");

  glGenerateMipmap(GL_TEXTURE_2D);
  BurningLogic::PrintGLError("UpdatedClassFactory:glGenerateMipmap");

  // ------------------------------------------------------------------------------------------------
  // Tiles and Masks texture
  // ------------------------------------------------------------------------------------------------
  glActiveTexture(kUpdatedTileMaskTextureUnit);
  BurningLogic::PrintGLError("UpdatedClassFactory:glActiveTexture");

  // TODO: Going to need to add corresponding deletes
  // I guess (also for "legacy" cases) Maybe?
  glGenTextures(1, &data.tile_glbuffers.tile_mask_texture);
  BurningLogic::PrintGLError("UpdatedClassFactory:glGenTextures");

  glBindTexture(GL_TEXTURE_2D, data.tile_glbuffers.tile_mask_texture);
  BurningLogic::PrintGLError("UpdatedClassFactory:glBindTexture");

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_WRAP_S");

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_WRAP_T");

  // Mipmap filter mode means I need to have mipmaps generated
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_MIN_FILTER");

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  BurningLogic::PrintGLError("UpdatedClassFactory:glTexParameteri GL_TEXTURE_MAG_FILTER");

  {
    GLsizei width = data.tiles_and_masks_surface->w;
    GLsizei height = width;
    GLsizei depth = data.tiles_and_masks_surface->h / height;

    /*  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE,
                   data.tiles_and_masks_surface->pixels);*/
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 data.tiles_and_masks_surface->pixels);
    BurningLogic::PrintGLError("UpdatedClassFactory:glTexImage3D");
  }

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  BurningLogic::PrintGLError("UpdatedClassFactory:glGenerateMipmap");

  /*
  // Tile buffer
  BurningLogic::TextureSetupHelper(classic_tile_texture, &data.tile_glbuffers.tbo_tile_buffer,
                                   data.tile_buffer, GL_DYNAMIC_DRAW,
                                   &data.tile_glbuffers.tbo_tex_tile_buffer, GL_R8UI);*/

  // Color buffer
  BurningLogic::TextureSetupHelper(kUpdatedColorTextureUnit, &data.tile_glbuffers.color_buffer,
                                   data.color_buffer, GL_DYNAMIC_DRAW, &data.tile_glbuffers.color_texture,
                                   GL_R8UI);

  // Tile data
  BurningLogic::TextureSetupHelper(kUpdatedTileDataTextureUnit, &data.tile_glbuffers.tile_buffer,
                                   data.tile_buffer, GL_DYNAMIC_DRAW, &data.tile_glbuffers.tile_texture,
                                   GL_R8UI);

  // CLUT
  BurningLogic::TextureSetupHelper(kUpdatedClutTextureUnit, &data.tile_glbuffers.clut_buffer, kDefaultC64Clut,
                                   GL_STATIC_DRAW, &data.tile_glbuffers.clut_texture, GL_RGBA8UI);

  /* // Sprite set
   BurningLogic::TextureSetupHelper(classic_sprite_texture, &data.sprite_glbuffers.tbo_sprite_buffer,
                                    data.processed_sprites, GL_STATIC_DRAW,
                                    &data.sprite_glbuffers.tbo_tex_sprite_buffer, GL_R8UI);

   data.sprite_glbuffers.tbo_clut_buffer = data.tile_glbuffers.tbo_clut_buffer;
   data.sprite_glbuffers.tbo_tex_clut_buffer = data.tile_glbuffers.tbo_tex_clut_buffer;*/

  // Tile shaders
  const std::filesystem::path kTileVertexShaderFilename = shader_path / kUpdatedTileVertexShaderName;
  const std::filesystem::path kTileWallFragmentShaderFilename =
      shader_path / kUpdatedTileWallFragmentShaderName;
  const std::filesystem::path kTileMiscFragmentShaderFilename =
      shader_path / kUpdatedTileMiscFragmentShaderName;

  const std::string tile_vertex_shader_source = BurningLogic::LoadShaderSource(kTileVertexShaderFilename);
  const std::string tile_wall_fragment_shader_source =
      BurningLogic::LoadShaderSource(kTileWallFragmentShaderFilename);
  const std::string tile_misc_fragment_shader_source =
      BurningLogic::LoadShaderSource(kTileMiscFragmentShaderFilename);

  data.tile_wall_shader =
      BurningLogic::BuildShaderProgram(tile_vertex_shader_source, tile_wall_fragment_shader_source);
  data.tile_misc_shader =
      BurningLogic::BuildShaderProgram(tile_vertex_shader_source, tile_misc_fragment_shader_source);

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
