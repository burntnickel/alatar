// Copyright 2026 Jude Giampaolo
//
// This file is part of Alatar.
//
// Alatar is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Alatar is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Alatar.
// If not, see <https://www.gnu.org/licenses/>.

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "constants.h"
#include "controller.h"
#include "getrespath.h"
#include "globals.h"
#include "graphics_common.h"
#include "iswayland.h"
#include "load_data.h"
#include "monsters.h"
#include "opengl_helper.h"
#include "pathdef.h"
#include "sdl_helper.h"
#include "updated.h"
#include "wiz_level.h"

const std::string kConfigIniFilename{"config.ini"};

SDL_Window* g_window = NULL;
SDL_GLContext g_sdl_glcontext;
constexpr SDL_InitFlags kSDLInitFlags =
    SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;

BurningLogic::GamepadVector gGamepadVector;  // TODO: Make this not global
SDL_Gamepad* gActiveGamePad;

static bool Initialize(const std::filesystem::path res_path) {
  // Initalize SDL
  bool success = ErrorEvalPrintSDL(SDL_Init(kSDLInitFlags), "SDL could not be initialized:");

  if (success) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    // TODO: Remove SDL_WINDOW_HIGH_PIXEL_DENSITY?
    g_window = SDL_CreateWindow("SDL Window", alatar::kScreenWidth, alatar::kScreenHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    success = ErrorEvalPrintSDL(g_window != NULL, "Window could not be created:");
  }

  if (success) {
    // Create context
    g_sdl_glcontext = SDL_GL_CreateContext(g_window);

    success = ErrorEvalPrintSDL(g_sdl_glcontext != NULL, "OpenGL context could not be created:");
  }

  if (success) {
    // Initialize GLEW
    glewExperimental = GL_TRUE;  // Is this a variable defined in the header?
    GLenum glew_error = glewInit();

    if (glew_error != GLEW_OK) {
      // Need this to work around an issue with GLEW and Wayland
      if ((glew_error == GLEW_ERROR_NO_GLX_DISPLAY) && BurningLogic::IsWayland()) {
        std::cout << "Ignoring GLEW_ERROR_NO_GLX_DISPLAY as using Wayland\n";
      } else {
        std::cerr << glew_error << "\n";
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(glew_error) << "(" << glew_error
                  << ")\n";
        success = false;
      }
    }
  }

  if (success) {
    // Use Vsync
    if (!SDL_GL_SetSwapInterval(1)) {
      std::cerr << "Warning: Unable to set VSync: " << SDL_GetError() << "\n";
    }
  }

  // Set blend mode
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  // Load config.ini
  if (success) {
    std::filesystem::path path{res_path / alatar::kConfigDirName / kConfigIniFilename};
    alatar::gIniFile = BurningLogic::LoadIniFile(path);
  }

  // Initalize controllers / gamepads / etc.
  if (success) {
    bool game_pad_inited = BurningLogic::InitControllers(gGamepadVector);

    BurningLogic::GamepadVector selected_gamepad;

    if (game_pad_inited) {
      selected_gamepad = BurningLogic::SelectActiveGamePads(gGamepadVector, 1);
    }

    if (selected_gamepad.size() == 0) {
      std::clog << "Warning: continuing without an active gamepad\n";
    } else {
      gActiveGamePad = selected_gamepad[0];
      std::cout << "Controller info:\n";
      BurningLogic::GamepadInfo gamepad_info = BurningLogic::GetGamepadInfo(gActiveGamePad);
      BurningLogic::PrintGamepadInfo(gamepad_info, std::cout);
    }
  }

  // Initialize counter scaling
  alatar::gCounterToMsScale = 1000.0 / static_cast<double>(SDL_GetPerformanceFrequency());

  return success;
}

static void Cleanup(void) {
  BurningLogic::CleanupControllers(gGamepadVector);

  bool success =
      ErrorEvalPrintSDL(SDL_GL_DestroyContext(g_sdl_glcontext), "OpenGL context could not be created:");

  if (success) {
    SDL_DestroyWindow(g_window);
    SDL_QuitSubSystem(kSDLInitFlags);
    SDL_Quit();
  }
}

static bool SetupCommon(std::span<unsigned char, alatar::kFileLength> level_data, alatar::LevelClass& level,
                        const std::filesystem::path& level_filename) {
  bool success = alatar::LoadData(level_filename, level_data);

  if (!success) {
    std::cerr << "Failed to load level file\n";
    return false;
  }

  level = alatar::LevelClass(level_data);

  return true;
}

static void ParseCommandLine(int argc, char* argv[], std::filesystem::path& filename,
                             alatar::GraphicsMode& graphics_mode) {
  if ((argc < 2) || (argc > 3)) {
    std::cerr << "Usage: level_display [-classic | -updated] <filename>" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ((argc == 2) & (argv[1][0] != '-')) {
    filename = std::filesystem::path(argv[1]);
    graphics_mode = alatar::Classic;
    return;
  }

  std::string arg1(argv[1]);

  if (arg1 == "-classic") {
    graphics_mode = alatar::Classic;
  } else if (arg1 == "-updated") {
    graphics_mode = alatar::Updated;
  } else {
    std::cerr << "Usage: level_display [-classic | -updated] <filename>" << std::endl;
    exit(EXIT_FAILURE);
  }

  filename = std::filesystem::path(argv[2]);
}

int main(int argc, char* argv[]) {
  bool success;
  std::array<unsigned char, alatar::kFileLength> level_data{};
  alatar::LevelClass level;

  const std::filesystem::path kResourcePath = BurningLogic::GetResPath();

  std::filesystem::path filename;
  alatar::GraphicsMode graphics_mode;

  ParseCommandLine(argc, argv, filename, graphics_mode);

  success = Initialize(kResourcePath);

  if (!success) {
    std::cerr << "Failed to initalize, exiting\n";
    return EXIT_FAILURE;
  }

  success = SetupCommon(level_data, level, filename);

  if (!success) {
    std::cerr << "Failed to complete SetupCommon, exiting\n";
    return EXIT_FAILURE;
  }

  alatar::GraphicsCommonPtr graphics_routine_ptr;
  Uint64 sdl_counter = SDL_GetPerformanceCounter();

  auto graphics_routine_opt =
      alatar::GraphicsCommonClass::GraphicsCommonClassFactory(kResourcePath, graphics_mode, sdl_counter);

  if (!graphics_routine_opt) {
    std::cerr << "Failed to initalize graphics, exiting\n";
    return EXIT_FAILURE;
  }

  graphics_routine_ptr = std::move(graphics_routine_opt.value());

  graphics_routine_ptr->LevelInit(level);

  alatar::WizardInfo wizard_info = level.GetWizardInfo();
  alatar::MonsterClassArray monster_info = level.GetMonsterInfo();

  bool gDone = false;
  SDL_Event sdl_event;

  BurningLogic::mat4 view_matrix{};
  BurningLogic::Identity4(view_matrix);

  while (!gDone) {
    Uint64 start_counter = SDL_GetPerformanceCounter();

    while (SDL_PollEvent(&sdl_event) != 0) {
      switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
          gDone = true;
          break;
        case SDL_EVENT_WINDOW_RESIZED: {
          auto window_width = sdl_event.window.data1;
          auto window_height = sdl_event.window.data2;
          float window_aspect = static_cast<float>(window_width) / static_cast<float>(window_height);

          float x_scale;
          float y_scale;

          if (window_aspect > alatar::kDesiredScreenAspect) {
            x_scale = alatar::kDesiredScreenAspect / window_aspect;
            y_scale = 1.0;
          } else {
            x_scale = 1.0;
            y_scale = window_aspect / alatar::kDesiredScreenAspect;
          }

          BurningLogic::ScaleMatHelper2D(view_matrix, x_scale, y_scale);
          glViewport(0, 0, window_width, window_height);
          break;
        }
        case SDL_EVENT_GAMEPAD_ADDED:
          // TODO: We really don't handle this yet (and don't need to until the real game, maybe not even
          // then)
          std::cout << "SDL_EVENT_GAMEPAD_ADDED: New gamepad detected\n";  // TODO: Do something with all of
                                                                           // the cout etc.
          break;
        case SDL_EVENT_GAMEPAD_REMOVED:
          // TODO: In a real game we'd probably pause, here we just note the event and exit
          std::cout << "SDL_EVENT_GAMEPAD_REMOVED\n";

          if (sdl_event.gdevice.which == SDL_GetGamepadID(gActiveGamePad)) {
            std::cerr << "Active gamepad removed, exiting\n";
            gDone = true;
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
    graphics_routine_ptr->Update(start_counter, level);

    alatar::UpdateMonsters(monster_info, start_counter);

    // Draw
    graphics_routine_ptr->Draw(monster_info, wizard_info, view_matrix);

    // Present
    SDL_GL_SwapWindow(g_window);

    Uint64 end_counter = SDL_GetPerformanceCounter();
    double elapsed_ms = static_cast<double>(end_counter - start_counter) * alatar::gCounterToMsScale;
    auto delay_time_ms = std::floor(alatar::kMsPerFrame - elapsed_ms);

    if (delay_time_ms > 0.0) {
      SDL_Delay(static_cast<Uint32>(delay_time_ms));
    }
  }

  Cleanup();

  return EXIT_SUCCESS;
}
