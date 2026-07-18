// Copyright 2025 Jude Giampaolo

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
