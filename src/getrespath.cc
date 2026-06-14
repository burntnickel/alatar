// Copyright 2025 Jude Giampaolo
//
// This file is part of Alien Geometries.
//
// Alien Geometries is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Alien Geometries is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Alien
// Geometries. If not, see <https://www.gnu.org/licenses/>. 

#include "getrespath.h"

#if (defined(__linux__)) || (defined(_WIN32))
#include "getexepath.h"

#elif defined(__APPLE__)
#include <SDL2/SDL.h>
#endif

namespace BurningLogic {

std::filesystem::path GetResPath(void) {
#if (defined(__linux__)) || (defined(_WIN32))
  const std::string kResourceDirName = "resources";

  return GetExePath() / kResourceDirName;
#elif defined(__APPLE__)
  char* base_path = SDL_GetBasePath();
  
  if (base_path == NULL) {
    throw std::runtime_error("Error calling SDL_GetBasePath(): " + std::string(SDL_GetError()));
  }
  
  std::filesystem::path path = base_path;
  SDL_free(base_path);
  
  return std::filesystem::canonical(path);
#endif
}

}
