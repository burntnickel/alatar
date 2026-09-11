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

#ifndef H_BURNINGLOGIC_SDL_HELPER
#define H_BURNINGLOGIC_SDL_HELPER

#include <filesystem>
#include <string_view>

#include <SDL_image.h>

bool ErrorEvalPrintSDL(bool condition, std::string_view message);

bool LoadToSurfaceRGBA(std::filesystem::path path_and_name, SDL_Surface*& target_surface);
bool LoadToSurface1Chan(std::filesystem::path path_and_name, SDL_Surface*& target_surface);

#endif
