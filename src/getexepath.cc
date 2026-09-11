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


#include "getexepath.h"

// The below only works for Linux and Windows and is not currently needed for MacOS

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
#include <stdexcept>
#include <string>
#endif

namespace BurningLogic {

std::filesystem::path GetExePath(void) {
#ifdef __linux__
  return std::filesystem::canonical("/proc/self/exe").remove_filename();
#elif defined _WIN32
  constexpr int buffer_len = 4096;
  CHAR buffer[buffer_len];
  DWORD path_length = GetModuleFileNameA(NULL, buffer, buffer_len);
  // then need to strip off the file name as above

  if (path_length == 0) {
    throw std::runtime_error("buffer_len[" + std::to_string(buffer_len) + "] insufficient to hold EXE path");
  }

  return std::filesystem::canonical(buffer).remove_filename();
#endif
}

}  // namespace BurningLogic
