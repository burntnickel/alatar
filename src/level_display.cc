#include <GL/glew.h>
#include <SDL.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <string_view>

#include "classic.h"
#include "getrespath.h"
#include "load_data.h"
#include "opengl_helper.h"
#include "sprites_classic.h"
#include "tiles_classic.h"
#include "updated.h"
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
                        wizard_level::LevelClass& level, const std::filesystem::path& level_filename) {
  bool success = alatar::LoadData(level_filename, level_data);

  if (!success) {
    std::cerr << "Failed to load level file\n";
    return false;
  }

  level = wizard_level::LevelClass(level_data);

  return true;
}

enum GraphicsMode { Classic, Updated, NoColoring };

static void ParseCommandLine(int argc, char* argv[], std::filesystem::path& filename,
                             GraphicsMode& graphics_mode) {
  if ((argc < 2) || (argc > 3)) {
    std::cerr << "Usage: level_display [-classic | -updated] <filename>" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ((argc == 2) & (argv[1][0] != '-')) {
    filename = std::filesystem::path(argv[1]);
    graphics_mode = Classic;
    return;
  }

  std::string arg1(argv[1]);

  if (arg1 == "-classic") {
    graphics_mode = Classic;
  } else if (arg1 == "-updated") {
    graphics_mode = Updated;
  } else {
    std::cerr << "Usage: level_display [-classic | -updated] <filename>" << std::endl;
    exit(EXIT_FAILURE);
  }

  filename = std::filesystem::path(argv[2]);
}

int main(int argc, char* argv[]) {
  bool success;
  alatar_classic::ClassicData classic_data;
  alatar_updated::UpdatedData updated_data;
  std::array<unsigned char, wizard_level::kFileLength> level_data{};
  wizard_level::LevelClass level;

  const std::filesystem::path kResourcePath = BurningLogic::GetResPath();
  const std::string kShadersDirName{"shaders"};
  const std::filesystem::path kShaderPath = kResourcePath / kShadersDirName;

  std::filesystem::path filename;
  GraphicsMode graphics_mode;

  ParseCommandLine(argc, argv, filename, graphics_mode);

  success = Initialize();

  if (!success) {
    std::cerr << "Failed to initalize, exiting\n";
    return EXIT_FAILURE;
  }

  success = SetupCommon(level_data, level, filename);

  if (!success) {
    std::cerr << "Failed to complete SetupCommon, exiting\n";
    return EXIT_FAILURE;
  }

  if (graphics_mode == Classic) {
    success = alatar_classic::SetupClassic(classic_data, kResourcePath, kShaderPath);

    if (!success) {
      std::cerr << "Failed to complete SetupClassic, exiting\n";
      return EXIT_FAILURE;
    }

    ClassicLevelInit(classic_data, level);
  } else if (graphics_mode == Updated) {
    success = alatar_updated::SetupUpdated(updated_data, kResourcePath, kShaderPath);

    if (!success) {
      std::cerr << "Failed to complete SetupUpdated, exiting\n";
      return EXIT_FAILURE;
    }
  } else {
    std::cerr << "Unimplemented graphics mode" << std::endl;
    return EXIT_FAILURE;
  }

  wizard_level::WizardInfo wizard_info = level.GetWizardInfo();
  wizard_level::MonsterInfoArray monster_info = level.GetMonsterInfo();

  bool gDone = false;
  SDL_Event sdl_event;

  double counter_to_ms_scale = 1000.0 / static_cast<double>(SDL_GetPerformanceFrequency());

  if (graphics_mode == Classic) {
    Uint64 sdl_counter = SDL_GetPerformanceCounter();

    classic_data.treasure_color_cycle_counter = sdl_counter;
    classic_data.fire_color_cycle_counter = sdl_counter;
    classic_data.fire_animation_counter = sdl_counter;
  }

  BurningLogic::mat4 view_matrix{};
  BurningLogic::Identity4(view_matrix);

  while (!gDone) {
    Uint64 start_counter = SDL_GetPerformanceCounter();

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

            BurningLogic::ScaleMatHelper2D(view_matrix, x_scale, y_scale);
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

    // Update
    if (graphics_mode == Classic) {
      ClassicUpdate(classic_data, start_counter, counter_to_ms_scale, level);
    }

    // Draw
    if (graphics_mode == Classic) {
      ClassicDraw(classic_data, monster_info, wizard_info, view_matrix);
    } else if (graphics_mode == Updated) {
      UpdatedDraw(updated_data, monster_info, wizard_info, view_matrix);
    } else {
      std::cerr << "Unimplemented graphics mode" << std::endl;
      return EXIT_FAILURE;
    }

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
