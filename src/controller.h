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

#ifndef H_BURNINGLOGIC_CONTROLLER
#define H_BURNINGLOGIC_CONTROLLER

#include <SDL3/SDL.h>

#include <iostream>
#include <optional>
#include <vector>
#include <string>

namespace BurningLogic {

using GamepadVector = std::vector<SDL_Gamepad*>;

struct GamepadInfo {
  std::string name{};
  std::string serial_number{};
  std::string path{};
  std::optional<SDL_GUID> guid_opt{};
  std::string guid_string{};
};

[[nodiscard]] bool InitControllers(GamepadVector& game_pads);

void CleanupControllers(GamepadVector& game_pads);

GamepadVector SelectActiveGamePads(const GamepadVector& game_pads, unsigned int num_game_pads);

GamepadInfo GetGamepadInfo(SDL_Gamepad* game_pad);

void PrintGamepadInfo(const GamepadInfo& info, std::ostream& os);

}  // namespace BurningLogic

#endif
