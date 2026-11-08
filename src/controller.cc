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

#include "controller.h"

#include <SDL3/SDL.h>

#include <iostream>

namespace BurningLogic {

bool InitControllers(void) {
  if (!SDL_HasGamepad()) {
    std::clog << "Warning: No game controllers detected\n";
    return false;
  }

  int gamepad_count = 0;
  SDL_JoystickID* joystick_ids = SDL_GetGamepads(&gamepad_count);

  if (gamepad_count == 0) {
    std::clog << "Warning: No game controllers detected\n";
    return false;
  }

  if (joystick_ids == nullptr) {
    std::cerr << "Error calling SDL_GetGamepads: " << SDL_GetError() << "\n";
    return false;
  }

  std::cout << "controller.cc: " << gamepad_count << " controller(s) detected\n";

  return true;
}

void CleanupControllers(void) {}

} // namespace BurningLogic


