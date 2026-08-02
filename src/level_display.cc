#include <GL/glew.h>
#include <SDL.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <string_view>

#include "c64_clut.h"
#include "classic.h"
#include "getrespath.h"
#include "load_data.h"
#include "opengl_helper.h"
#include "sprites_classic.h"
#include "tiles_classic.h"
#include "value_cycle.h"
#include "wiz_level.h"

// Default to 60 frames per second
constexpr double kDefaultFrameRateHz = 60;
constexpr double kMsPerFrame = 1000.0 / kDefaultFrameRateHz;

const std::string kClassicTileVertexShaderName{"classic_vertex.glsl"};
const std::string kClassicTileFragmentShaderName{"classic_tile_fragment.glsl"};
const std::string kClassicSpriteVertexShaderName{"classic_vertex.glsl"};
const std::string kClassicSpriteFragmentShaderName{"classic_sprite_fragment.glsl"};

SDL_Window* g_window = NULL;
SDL_GLContext g_sdl_glcontext;

alatar_classic::ClassicTileGLBuffers g_tile_glbuffers;
alatar_classic::ClassicSpriteGLBuffers g_sprite_glbuffers;

// The factor of 0.75 is to match the Commodore 64 pixel aspect ratio (NTSC at least)
constexpr float kPixelAspectRatio = 0.75;
constexpr uint_least32_t kScreenWidth = 8 * wizard_level::kColTiles * 2.5 * kPixelAspectRatio;
constexpr uint_least32_t kScreenHeight = 8 * wizard_level::kRowTiles * 2.5;
constexpr float kDesiredScreenAspect = static_cast<float>(kScreenWidth) / static_cast<float>(kScreenHeight);

BurningLogic::mat4 g_view_matrix{};

static bool ErrorEvalPrintSDL(bool condition, std::string_view message) {
  if (condition) {
    std::cerr << message << " " << SDL_GetError() << "\n";
  }

  return !condition;
}

static bool Initialize(void) {
  // Initalize SDL
  bool success = ErrorEvalPrintSDL(SDL_Init(SDL_INIT_VIDEO) < 0, "SDL could not be initialized:");

  if (success) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    g_window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth,
                                kScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    success = ErrorEvalPrintSDL(g_window == NULL, "Window could not be created:");
  }

  if (success) {
    // Create context
    g_sdl_glcontext = SDL_GL_CreateContext(g_window);

    success = ErrorEvalPrintSDL(g_sdl_glcontext == NULL, "OpenGL context could not be created:");
  }

  if (success) {
    // Initialize GLEW
    glewExperimental = GL_TRUE;  // Is this a variable defined in the header?
    GLenum glew_error = glewInit();

    if (glew_error != GLEW_OK) {
      std::cerr << "Error initializing GLEW: " << glewGetErrorString(glew_error) << "\n";
      success = false;
    }
  }

  if (success) {
    // Use Vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
      std::cerr << "Warning: Unable to set VSync: " << SDL_GetError() << "\n";
    }
  }

  // Set blend mode
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  return success;
}

int main(int argc, char* argv[]) {
  bool success;
  std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> tile_buffer{};
  std::array<unsigned char, wizard_level::kRowTiles * wizard_level::kColTiles> color_buffer{};
  std::array<unsigned char, wizard_level::kTileBufferSize> tile_set{};
  std::array<unsigned char, wizard_level::kFileLength> level_data{};
  std::array<unsigned char, alatar_classic::kSpriteProcDataSize> processed_sprites{};

  const std::filesystem::path kResourcePath = BurningLogic::GetResPath();

  const std::string kShadersDirName{"shaders"};
  const std::string kCharSetsDirName{"classic"};
  const std::string kCharSetName{"chrw"};
  const std::string kSpriteSetName{"sprw"};

  alatar::ValueCycle<unsigned char> treasure_color_cycle{alatar_classic::kTreasureCycleColors};
  alatar::ValueCycle<unsigned char> fire_color_cycle{alatar_classic::kFireCycleColors};

  if (argc != 2) {
    std::cerr << "Exactly one arguement must be supplied, the level file name" << std::endl;
    return EXIT_FAILURE;
  }

  success = Initialize();

  if (!success) {
    std::cerr << "Failed to initalize, exiting\n";
    return EXIT_FAILURE;
  }

  // Set to 32 (space) as blank character & black
  for (unsigned int ii = 0; ii < (wizard_level::kRowTiles * wizard_level::kColTiles); ++ii) {
    tile_buffer[ii] = 32;
    color_buffer[ii] = 0;
  }

  success = alatar::LoadData(argv[1], level_data);

  if (!success) {
    std::cerr << "Failed to load level file, exiting\n";
    return EXIT_FAILURE;
  }

  wizard_level::LevelClass level(level_data);

  for (int row = 0; row < wizard_level::kTileDataRows; ++row) {
    for (int col = 0; col < wizard_level::kTileDataCols; ++col) {
      unsigned int screen_row = static_cast<unsigned int>(row) + 1;
      unsigned int screen_col = static_cast<unsigned int>(col);
      unsigned int screen_index = wizard_level::kColTiles * screen_row + screen_col;

      unsigned char tile = level.GetTileAt(row, col);

      tile_buffer[screen_index] = tile;
      color_buffer[screen_index] = level.GetTileColor(tile);
    }
  }

  auto tile_set_path_and_name = kResourcePath / kCharSetsDirName / kCharSetName;
  success = alatar_classic::LoadCharSet(tile_set_path_and_name, tile_set);

  if (!success) {
    std::cerr << "Failed to load charater set, exiting\n";
    return EXIT_FAILURE;
  }

  auto sprite_set_path_and_name = kResourcePath / kCharSetsDirName / kSpriteSetName;
  success = alatar_classic::LoadSpriteData(sprite_set_path_and_name, processed_sprites);

  if (!success) {
    std::cerr << "Failed to load sprite set, exiting\n";
    return EXIT_FAILURE;
  }

  // TODO: Move this stuff out of main
  // Set up the various buffers for the display data
  GLuint tbo_tile_buffer;
  GLuint tbo_tex_tile_buffer;
  GLuint tbo_color_buffer;
  GLuint tbo_tex_color_buffer;
  GLuint tbo_tileset_buffer;
  GLuint tbo_tex_tileset_buffer;
  GLuint tbo_clut_buffer;
  GLuint tbo_tex_clut_buffer;
  GLuint tbo_sprite_buffer;
  GLuint tbo_tex_sprite_buffer;

  // Tile buffer
  BurningLogic::TextureSetupHelper(GL_TEXTURE0, &tbo_tile_buffer, tile_buffer, GL_DYNAMIC_DRAW,
                                   &tbo_tex_tile_buffer, GL_R8UI);

  // Color buffer
  BurningLogic::TextureSetupHelper(GL_TEXTURE1, &tbo_color_buffer, color_buffer, GL_DYNAMIC_DRAW,
                                   &tbo_tex_color_buffer, GL_R8UI);

  // Tile set
  BurningLogic::TextureSetupHelper(GL_TEXTURE2, &tbo_tileset_buffer, tile_set, GL_STATIC_DRAW,
                                   &tbo_tex_tileset_buffer, GL_R8UI);

  // CLUT
  BurningLogic::TextureSetupHelper(GL_TEXTURE3, &tbo_clut_buffer, kDefaultC64Clut, GL_STATIC_DRAW,
                                   &tbo_tex_clut_buffer, GL_RGBA8UI);

  // Sprite set
  BurningLogic::TextureSetupHelper(GL_TEXTURE4, &tbo_sprite_buffer, processed_sprites, GL_STATIC_DRAW,
                                   &tbo_tex_sprite_buffer, GL_R8UI);

  // TODO: Add texture units here as well
  // GL buffers for tiles
  g_tile_glbuffers.tbo_tile_buffer = tbo_tile_buffer;
  g_tile_glbuffers.tbo_tex_tile_buffer = tbo_tex_tile_buffer;
  g_tile_glbuffers.tbo_color_buffer = tbo_color_buffer;
  g_tile_glbuffers.tbo_tex_color_buffer = tbo_tex_color_buffer;
  g_tile_glbuffers.tbo_tileset_buffer = tbo_tileset_buffer;
  g_tile_glbuffers.tbo_tex_tileset_buffer = tbo_tex_tileset_buffer;
  g_tile_glbuffers.tbo_clut_buffer = tbo_clut_buffer;
  g_tile_glbuffers.tbo_tex_clut_buffer = tbo_tex_clut_buffer;

  // GL buffers for sprites
  g_sprite_glbuffers.tbo_sprite_buffer = tbo_sprite_buffer;
  g_sprite_glbuffers.tbo_tex_sprite_buffer = tbo_tex_sprite_buffer;
  g_sprite_glbuffers.sprite_texture_unit = GL_TEXTURE4;
  g_sprite_glbuffers.tbo_clut_buffer = tbo_clut_buffer;
  g_sprite_glbuffers.tbo_tex_clut_buffer = tbo_tex_clut_buffer;
  g_sprite_glbuffers.clut_texture_unit = GL_TEXTURE3;

  // Set up shaders
  const std::filesystem::path kShaderPath = kResourcePath / kShadersDirName;
  const std::filesystem::path kClassicTileVertexShaderFilename = kShaderPath / kClassicTileVertexShaderName;
  const std::filesystem::path kClassicTileFragmentShaderFilename =
      kShaderPath / kClassicTileFragmentShaderName;
  const std::filesystem::path kClassicSpriteVertexShaderFilename =
      kShaderPath / kClassicSpriteVertexShaderName;
  const std::filesystem::path kClassicSpriteFragmentShaderFilename =
      kShaderPath / kClassicSpriteFragmentShaderName;

  // Tile shaders
  const std::string classic_tile_vertex_shader_source =
      BurningLogic::LoadShaderSource(kClassicTileVertexShaderFilename);
  const std::string classic_tile_fragment_shader_source =
      BurningLogic::LoadShaderSource(kClassicTileFragmentShaderFilename);

  // Sprite shaders
  const std::string classic_sprite_vertex_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteVertexShaderFilename);
  const std::string classic_sprite_fragment_shader_source =
      BurningLogic::LoadShaderSource(kClassicSpriteFragmentShaderFilename);

  auto classic_tile_shader = BurningLogic::BuildShaderProgram(classic_tile_vertex_shader_source,
                                                                   classic_tile_fragment_shader_source);
  auto classic_sprite_shader = BurningLogic::BuildShaderProgram(classic_sprite_vertex_shader_source,
                                                                     classic_sprite_fragment_shader_source);

  wizard_level::WizardInfo wizard_info = level.GetWizardInfo();
  wizard_level::MonsterInfoArray monster_info = level.GetMonsterInfo();

  bool gDone = false;
  SDL_Event sdl_event;

  Uint64 treasure_color_cycle_counter = SDL_GetPerformanceCounter();
  Uint64 fire_color_cycle_counter = SDL_GetPerformanceCounter();
  Uint64 fire_animation_counter = SDL_GetPerformanceCounter();

  BurningLogic::Identity4(g_view_matrix);

  while (!gDone) {
    Uint64 start_counter = SDL_GetPerformanceCounter();

    double counter_to_ms_scale = 1000.0 / static_cast<double>(SDL_GetPerformanceFrequency());

    while (SDL_PollEvent(&sdl_event) != 0) {
      switch (sdl_event.type) {
        case SDL_QUIT:
          gDone = true;
          break;
        case SDL_WINDOWEVENT:
          if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
            auto window_width = sdl_event.window.data1;
            auto window_height = sdl_event.window.data2;
            float window_aspect = static_cast<float>(window_width) / static_cast<float>(window_height);

            float x_scale;
            float y_scale;

            if (window_aspect > kDesiredScreenAspect) {
              x_scale = kDesiredScreenAspect / window_aspect;
              y_scale = 1.0;
            } else {
              x_scale = 1.0;
              y_scale = window_aspect / kDesiredScreenAspect;
            }

            BurningLogic::ScaleMatHelper2D(g_view_matrix, x_scale, y_scale);
            glViewport(0, 0, window_width, window_height);
          }
          break;
        default:
          break;
      }
    }

    // Clear the screen
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update graphics (these probably don't upate at 60 Hz, need to get te correct number)
    // I think color changes faster then the fire animation
    if ((static_cast<double>(start_counter - treasure_color_cycle_counter) * counter_to_ms_scale) >
        alatar_classic::kTreasureColorCycleFrameTimeMs) {
      ++treasure_color_cycle;
      treasure_color_cycle_counter = start_counter;
    }

    unsigned char treasure_color_idx = treasure_color_cycle.GetValue();

    if ((static_cast<double>(start_counter - fire_color_cycle_counter) * counter_to_ms_scale) >
        alatar_classic::kFireColorCycleFrameTimeMs) {
      ++fire_color_cycle;
      fire_color_cycle_counter = start_counter;
    }

    unsigned char fire_color_idx = fire_color_cycle.GetValue();

    for (int row = 0; row < wizard_level::kTileDataRows; ++row) {
      for (int col = 0; col < wizard_level::kTileDataCols; ++col) {
        unsigned int screen_row = static_cast<unsigned int>(row) + 1;
        unsigned int screen_col = static_cast<unsigned int>(col);
        unsigned int screen_index = wizard_level::kColTiles * screen_row + screen_col;

        unsigned char tile = level.GetTileAt(row, col);

        if (wizard_level::GetTileGroup(tile) == wizard_level::kTreasure) {
          color_buffer[screen_index] = treasure_color_idx;
        }

        if (wizard_level::GetTileGroup(tile) == wizard_level::kFire) {
          color_buffer[screen_index] = fire_color_idx;
        }
      }
    }

    if ((static_cast<double>(start_counter - fire_animation_counter) * counter_to_ms_scale) >
        alatar_classic::kFireAnimationFrameTimeMs) {
      fire_animation_counter = start_counter;

      for (int row = 0; row < wizard_level::kTileDataRows; ++row) {
        for (int col = 0; col < wizard_level::kTileDataCols; ++col) {
          unsigned int screen_row = static_cast<unsigned int>(row) + 1;
          unsigned int screen_col = static_cast<unsigned int>(col);
          unsigned int screen_index = wizard_level::kColTiles * screen_row + screen_col;

          unsigned char tile = level.GetTileAt(row, col);
          if (wizard_level::GetTileGroup(tile) == wizard_level::kFire) {
            fire_animation_counter = start_counter;
            auto tmp_tile = tile_buffer[screen_index];
            tmp_tile = tmp_tile + 1;

            if (tmp_tile > alatar_classic::kFireTileLast) {
              tmp_tile = alatar_classic::kFireTileFirst;
            }

            tile_buffer[screen_index] = tmp_tile;
          }
        }
      }
    }

    glBindBuffer(GL_TEXTURE_BUFFER, tbo_color_buffer);
    BurningLogic::PrintGLError("main:glBindBuffer");

    glBufferSubData(GL_TEXTURE_BUFFER, 0, static_cast<GLsizeiptr>(color_buffer.size()), color_buffer.data());
    BurningLogic::PrintGLError("main:glBufferSubData");

    glBindBuffer(GL_TEXTURE_BUFFER, tbo_tile_buffer);
    BurningLogic::PrintGLError("main:glBindBuffer");

    glBufferSubData(GL_TEXTURE_BUFFER, 0, static_cast<GLsizeiptr>(tile_buffer.size()), tile_buffer.data());
    BurningLogic::PrintGLError("main:glBufferSubData");

    // End update

    // Draw
    alatar_classic::PaintClassicTiles(classic_tile_shader, g_view_matrix, SDL_FRect({-1.0, -1.0, 2.0, 2.0}),
                                      g_tile_glbuffers);

    for (unsigned int ii = 0; ii < 6; ++ii) {
      if (monster_info[ii].active) {
        alatar_classic::DrawClassicSpriteC64(
            classic_sprite_shader, g_sprite_glbuffers, monster_info[ii].sprite_id, monster_info[ii].x,
            monster_info[ii].y,
            {{wizard_level::kColorLightBlue, monster_info[ii].color, wizard_level::kColorWhite}});
      }
    }

    alatar_classic::DrawClassicSpriteC64(
        classic_sprite_shader, g_sprite_glbuffers, 0, wizard_info.x, wizard_info.y,
        {{wizard_level::kColorLightBlue, wizard_info.color, wizard_level::kColorWhite}});

    // Present
    SDL_GL_SwapWindow(g_window);

    Uint64 end_counter = SDL_GetPerformanceCounter();

    double elapsed_ms = static_cast<double>(end_counter - start_counter) * counter_to_ms_scale;

    auto delay_time_ms = std::floor(kMsPerFrame - elapsed_ms);

    if (delay_time_ms > 0.0) {
      SDL_Delay(static_cast<Uint32>(delay_time_ms));
    }
  }

  return EXIT_SUCCESS;
}
