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

#include "graphics_common.h"

#include <iostream>

#include "classic.h"
#include "updated.h"

namespace alatar {

std::optional<GraphicsCommonPtr> GraphicsCommonClass::GraphicsCommonClassFactory(
    std::filesystem::path resource_path, std::filesystem::path shader_path, GraphicsMode graphics_mode,
    Uint64 sdl_counter) {
  switch (graphics_mode) {
    case Classic:
      return alatar_classic::ClassicClass::ClassicClassFactory(resource_path, shader_path, sdl_counter);
      break;
    case Updated:
      return alatar_updated::UpdatedClass::UpdatedClassFactory(resource_path, shader_path, sdl_counter);
      break;
    default:
      std::cerr << "Unimplemented graphics mode" << std::endl;
      return {};
  }
}

}  // namespace alatar
