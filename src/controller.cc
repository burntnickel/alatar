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

#include <array>
#include <charconv>

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

// TODO: Add code that remebers and reuses controllers if they're still connected
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

  // Have user select which game pad to use. There's got to be a better way than asking each time.  Maybe
  // rememebr and use that game pad if it is found againb otherwise ask?

  // TODO: Move a subset of this to util or similar
  for (unsigned int ii = 0; ii < num_game_pads; ++ii) {
    bool got_input = false;
    std::string line;
    unsigned int value;

    while (true) {
      std::cout << "Please select a game pad for input " << (ii + 1) << ": ";

      if (!std::getline(std::cin, line)) {
        // Handle EOF (e.g., Ctrl+D / Ctrl+Z)
        break;
      }

      auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), value);

      if (ec == std::errc() && ptr == line.data() + line.size()) {
        if ((value > 0) && (value <= game_pads.size())) {
          got_input = true;
          tmp_game_pad_list.push_back(game_pads[value - 1]);
          break;
        }
      }

      std::cout << "Please enter an integer between 1 and " << game_pads.size() << "\n";
    }

    if (!got_input) {
      std::cout << "\nAssigning default controller\n";
      tmp_game_pad_list.push_back(game_pads[0]);
    }
  }

  return tmp_game_pad_list;
}

static bool IsZeroGuid(SDL_GUID guid) {
  bool ret = true;

  for (int ii = 0; ii < 16; ++ii) {
    ret = ret && (guid.data[ii] == 0);
  }

  return ret;
}

GamepadInfo GetGamepadInfo(SDL_Gamepad* game_pad) {
  GamepadInfo game_pad_info;

  const char* name = SDL_GetGamepadName(game_pad);

  if (name == nullptr) {
    game_pad_info.name = "";
  } else {
    game_pad_info.name = name;
  }

  const char* serial = SDL_GetGamepadSerial(game_pad);

  if (serial == nullptr) {
    game_pad_info.serial_number = "";
  } else {
    game_pad_info.serial_number = serial;
  }

  const char* path = SDL_GetGamepadPath(game_pad);

  if (path == nullptr) {
    game_pad_info.path = "";
  } else {
    game_pad_info.path = path;
  }

  SDL_JoystickID joystick = SDL_GetGamepadID(game_pad);

  if (joystick == 0) {
    game_pad_info.guid_opt = {};
  } else {
    SDL_GUID guid = SDL_GetGamepadGUIDForID(joystick);

    if (IsZeroGuid(guid)) {
      game_pad_info.guid_opt = {};
    } else {
      game_pad_info.guid_opt = guid;

      std::array<char, 33> guid_string;

      SDL_GUIDToString(guid, guid_string.data(), guid_string.size());

      game_pad_info.guid_string = std::string(guid_string.data());
    }
  }

  return game_pad_info;
}

void PrintGamepadInfo(const GamepadInfo& info, std::ostream& os) {
  os << "Name          : " << info.name << "\n";
  os << "Serial Number : " << info.serial_number << "\n";
  os << "Path          : " << info.path << "\n";

  if (info.guid_opt) {
    os << "GUID          : " << info.guid_string << "\n";
  }
}

}  // namespace BurningLogic
