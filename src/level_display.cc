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

SDL_Window* g_window = NULL;
SDL_GLContext g_sdl_glcontext;

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

static bool SetupCommon(std::span<unsigned char, wizard_level::kFileLength> level_data,
                        wizard_level::LevelClass& level, const char* level_filename) {
  bool success = alatar::LoadData(level_filename, level_data);

  if (!success) {
    std::cerr << "Failed to load level file\n";
    return false;
  }

  level = wizard_level::LevelClass(level_data);

  return true;
}

int main(int argc, char* argv[]) {
  bool success;
  alatar_classic::ClassicData classic_data;
  std::array<unsigned char, wizard_level::kFileLength> level_data{};
  wizard_level::LevelClass level;

  const std::filesystem::path kResourcePath = BurningLogic::GetResPath();
  const std::string kShadersDirName{"shaders"};

  const std::filesystem::path kShaderPath = kResourcePath / kShadersDirName;

  if (argc != 2) {
    std::cerr << "Exactly one arguement must be supplied, the level file name" << std::endl;
    return EXIT_FAILURE;
  }

  success = Initialize();

  if (!success) {
    std::cerr << "Failed to initalize, exiting\n";
    return EXIT_FAILURE;
  }

  success = SetupCommon(level_data, level, argv[1]);

  if (!success) {
    std::cerr << "Failed to complete SetupCommon, exiting\n";
    return EXIT_FAILURE;
  }

  success = SetupClassic(classic_data, level, kResourcePath, kShaderPath);

  if (!success) {
    std::cerr << "Failed to complete SetupClassic, exiting\n";
    return EXIT_FAILURE;
  }

  wizard_level::WizardInfo wizard_info = level.GetWizardInfo();
  wizard_level::MonsterInfoArray monster_info = level.GetMonsterInfo();

  bool gDone = false;
  SDL_Event sdl_event;

  // Classic specific
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

    // Start update

    // Classic specific
    // Update graphics (these probably don't upate at 60 Hz, need to get te correct number)
    // I think color changes faster then the fire animation
    if ((static_cast<double>(start_counter - treasure_color_cycle_counter) * counter_to_ms_scale) >
        alatar_classic::kTreasureColorCycleFrameTimeMs) {
      ++classic_data.treasure_color_cycle;
      treasure_color_cycle_counter = start_counter;
    }

    // Classic specific
    unsigned char treasure_color_idx = classic_data.treasure_color_cycle.GetValue();

    // Classic specific
    if ((static_cast<double>(start_counter - fire_color_cycle_counter) * counter_to_ms_scale) >
        alatar_classic::kFireColorCycleFrameTimeMs) {
      ++classic_data.fire_color_cycle;
      fire_color_cycle_counter = start_counter;
    }

    // Classic specific
    unsigned char fire_color_idx = classic_data.fire_color_cycle.GetValue();

    // Classic specific
    // Set updated fire and treasure colors
    for (int row = 0; row < wizard_level::kTileDataRows; ++row) {
      for (int col = 0; col < wizard_level::kTileDataCols; ++col) {
        unsigned int screen_row = static_cast<unsigned int>(row) + 1;
        unsigned int screen_col = static_cast<unsigned int>(col);
        unsigned int screen_index = wizard_level::kColTiles * screen_row + screen_col;

        unsigned char tile = level.GetTileAt(row, col);

        if (wizard_level::GetTileGroup(tile) == wizard_level::kTreasure) {
          classic_data.color_buffer[screen_index] = treasure_color_idx;
        }

        if (wizard_level::GetTileGroup(tile) == wizard_level::kFire) {
          classic_data.color_buffer[screen_index] = fire_color_idx;
        }
      }
    }

    // Classic specific
    // Cycle fire tile charaters for animation
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
            auto tmp_tile = classic_data.tile_buffer[screen_index];
            tmp_tile = tmp_tile + 1;

            if (tmp_tile > alatar_classic::kFireTileLast) {
              tmp_tile = alatar_classic::kFireTileFirst;
            }

            classic_data.tile_buffer[screen_index] = tmp_tile;
          }
        }
      }
    }

    // Classic specific
    glBindBuffer(GL_TEXTURE_BUFFER, classic_data.tile_glbuffers.tbo_color_buffer);
    BurningLogic::PrintGLError("main:glBindBuffer");

    // Classic specific
    glBufferSubData(GL_TEXTURE_BUFFER, 0, static_cast<GLsizeiptr>(classic_data.color_buffer.size()),
                    classic_data.color_buffer.data());
    BurningLogic::PrintGLError("main:glBufferSubData");

    // Classic specific
    glBindBuffer(GL_TEXTURE_BUFFER, classic_data.tile_glbuffers.tbo_tile_buffer);
    BurningLogic::PrintGLError("main:glBindBuffer");

    // Classic specific
    glBufferSubData(GL_TEXTURE_BUFFER, 0, static_cast<GLsizeiptr>(classic_data.tile_buffer.size()),
                    classic_data.tile_buffer.data());
    BurningLogic::PrintGLError("main:glBufferSubData");

    // End update

    // Draw
    // Classic specific
    alatar_classic::PaintClassicTiles(classic_data.tile_shader, g_view_matrix,
                                      SDL_FRect({-1.0, -1.0, 2.0, 2.0}), classic_data.tile_glbuffers);
    // Classic specific
    // Draw monster sprites
    for (unsigned int ii = 0; ii < 6; ++ii) {
      if (monster_info[ii].active) {
        alatar_classic::DrawClassicSpriteC64(
            classic_data.sprite_shader, g_view_matrix, classic_data.sprite_glbuffers,
            monster_info[ii].sprite_id, monster_info[ii].x, monster_info[ii].y,
            {{wizard_level::kColorLightBlue, monster_info[ii].color, wizard_level::kColorWhite}});
      }
    }

    // Classic specific
    // Draw wizard sprite
    alatar_classic::DrawClassicSpriteC64(
        classic_data.sprite_shader, g_view_matrix, classic_data.sprite_glbuffers, 0, wizard_info.x,
        wizard_info.y, {{wizard_level::kColorLightBlue, wizard_info.color, wizard_level::kColorWhite}});

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
