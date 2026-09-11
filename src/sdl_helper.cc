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

#include "sdl_helper.h"

#include <SDL.h>

#include <array>
#include <iostream>

bool ErrorEvalPrintSDL(bool condition, std::string_view message) {
  if (condition) {
    std::cerr << message << " " << SDL_GetError() << "\n";
  }

  return !condition;
}

bool LoadToSurfaceRGBA(std::filesystem::path path_and_name, SDL_Surface*& target_surface) {
  SDL_Surface* raw_surface = IMG_Load(path_and_name.c_str());
  bool success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling IMG_Load:");

  // Convert to the required RGBA format here
  SDL_Surface* converted_surface;

  if (success) {
    converted_surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA32, 0);
    success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling SDL_ConvertSurfaceFormat:");
  }

  if (raw_surface != NULL) {
    SDL_FreeSurface(raw_surface);
  }

  if (success) {
    target_surface = converted_surface;
  }

  return success;
}

bool LoadToSurface1Chan(std::filesystem::path path_and_name, SDL_Surface*& target_surface) {
  SDL_Surface* raw_surface = IMG_Load(path_and_name.c_str());
  bool success = ErrorEvalPrintSDL(raw_surface == NULL, "Error calling IMG_Load:");

  if (success) {
    // Convert to single 8 bit channel here
    SDL_Surface* tmp_surface = SDL_CreateRGBSurface(0, 1, 1, 8, 0, 0, 0, 0);
    // SDL_PixelFormat* index_8_format = tmp_surface->format;

    std::array<SDL_Color, 256> colors;

    for (unsigned int i = 0; i < 256; ++i) {
      colors[i].r = static_cast<Uint8>(i);
      colors[i].g = static_cast<Uint8>(i);
      colors[i].b = static_cast<Uint8>(i);
      colors[i].a = 255;
    }

    SDL_SetPaletteColors(tmp_surface->format->palette, colors.data(), 0, 256);

    target_surface = SDL_ConvertSurface(raw_surface, tmp_surface->format, 0);

    SDL_FreeSurface(tmp_surface);
    SDL_FreeSurface(raw_surface);
  }

  return success;
}

