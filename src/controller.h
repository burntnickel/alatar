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

#include <stdexcept>
#include <vector>

namespace BurningLogic {

using GamepadVector = std::vector<SDL_Gamepad*>;

[[nodiscard]] bool InitControllers(GamepadVector& game_pads);
void CleanupControllers(GamepadVector& game_pads);

GamepadVector SelectActiveGamePads(const GamepadVector& game_pads, unsigned int num_game_pads);

}  // namespace BurningLogic

#endif
