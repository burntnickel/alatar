#include "sdl_helper.h"

#include <SDL.h>

#include <iostream>

bool ErrorEvalPrintSDL(bool condition, std::string_view message) {
  if (condition) {
    std::cerr << message << " " << SDL_GetError() << "\n";
  }

  return !condition;
}
