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

#ifndef H_ALATAR_INPUT
#define H_ALATAR_INPUT

#include <SDL3/SDL.h>

#include <iostream>

namespace alatar {

struct ButtonStates {
  // These flags are for if the button is currently pressed
  bool up = false;
  bool down = false;
  bool left = false;
  bool right = false;
  bool jump = false;
  bool spell = false;
  // These flags are only set when a button is first pressed or released, they indicate state transitions
  bool up_pressed;
  bool down_pressed;
  bool left_pressed;
  bool right_pressed;
  bool jump_pressed;
  bool spell_pressed;
  bool up_released;
  bool down_released;
  bool left_released;
  bool right_released;
  bool jump_released;
  bool spell_released;
};

// Pass in the previous state and the structure will be updated
void GetButtonStates(SDL_Gamepad* gamepad, ButtonStates& states);

// This is really just for debug etc.
void PrintStatusChanges(const ButtonStates states, std::ostream& os);

}  // namespace alatar

#endif
