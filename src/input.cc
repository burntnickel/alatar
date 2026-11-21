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

#include "input.h"

namespace alatar {

// TODO: This would all need to be customizable in a real game

void GetButtonStates(SDL_Gamepad* gamepad, ButtonStates& states) {
  ButtonStates old_states = states;

  // TODO: Add support for analog sticks as well
  states.up = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
  states.down = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
  states.left = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
  states.right = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
  states.jump = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
  states.spell = SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST);

  states.up_pressed = !old_states.up && states.up;
  states.down_pressed = !old_states.down && states.down;
  states.left_pressed = !old_states.left && states.left;
  states.right_pressed = !old_states.right && states.right;
  states.jump_pressed = !old_states.jump && states.jump;
  states.spell_pressed = !old_states.spell && states.spell;

  states.up_released = old_states.up && !states.up;
  states.down_released = old_states.down && !states.down;
  states.left_released = old_states.left && !states.left;
  states.right_released = old_states.right && !states.right;
  states.jump_released = old_states.jump && !states.jump;
  states.spell_released = old_states.spell && !states.spell;
}

void PrintStatusChanges(const ButtonStates states, std::ostream& os) {
  if (states.up_pressed) {
    os << "Up Pressed\n";
  }

  if (states.down_pressed) {
    os << "Down Pressed\n";
  }

  if (states.left_pressed) {
    os << "Left Pressed\n";
  }

  if (states.right_pressed) {
    os << "Right Pressed\n";
  }

  if (states.jump_pressed) {
    os << "Jump Pressed\n";
  }

  if (states.spell_pressed) {
    os << "Spell Pressed\n";
  }

  if (states.up_released) {
    os << "Up Released\n";
  }

  if (states.down_released) {
    os << "Down Released\n";
  }

  if (states.left_released) {
    os << "Left Released\n";
  }

  if (states.right_released) {
    os << "Right Released\n";
  }

  if (states.jump_released) {
    os << "Jump Released\n";
  }

  if (states.spell_released) {
    os << "Spell Released\n";
  }
}

}  // namespace alatar
