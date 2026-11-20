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

#include <iostream>

namespace BurningLogic {

bool InitControllers(GamepadVector& game_pads) {
  // Clear the vector in case init is called multiple times in smae program.  It should, of course, always be
  // paired with the cleanup routine
  game_pads.clear();

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

  for (int ii = 0; ii < gamepad_count; ++ii) {
    SDL_Gamepad* game_pad = SDL_OpenGamepad(joystick_ids[ii]);

    if (game_pad == nullptr) {
      std::cerr << "Error calling SDL_OpenGamepad: " << SDL_GetError() << "\n";
    } else {
      game_pads.push_back(game_pad);
      std::cout << "Gamepad(" << game_pads.size() << "): " << SDL_GetGamepadName(game_pad) << "\n";
    }
  }

  return true;
}

void CleanupControllers(GamepadVector& game_pads) {
  for (const auto& game_pad : game_pads) {
    SDL_CloseGamepad(game_pad);
  }

  game_pads.clear();
}

GamepadVector SelectActiveGamePads(const GamepadVector& game_pads, unsigned int num_game_pads) {
  GamepadVector tmp_game_pad_list{};

  if (num_game_pads > 1) {
    throw std::out_of_range("Currently only selection of a single controller is supported");
  }

  if (num_game_pads == 0) {
    tmp_game_pad_list.clear();
    return tmp_game_pad_list;
  }

  if (num_game_pads >= game_pads.size()) {
    std::cout << "Auto assigning " << game_pads.size() << " game pad(s), " << num_game_pads << " requested\n";

    for (unsigned int ii = 0; ii < game_pads.size(); ++ii) {
      tmp_game_pad_list.push_back(game_pads[ii]);
    }

    return tmp_game_pad_list;
  }

  return tmp_game_pad_list;
}

}  // namespace BurningLogic
