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

#ifndef H_ALATAR_GRAPHICS_COMMON
#define H_ALATAR_GRAPHICS_COMMON

#include <SDL.h>

#include <filesystem>
#include <memory>
#include <optional>

#include "monsters.h"
#include "opengl_helper.h"
#include "wiz_level.h"

namespace alatar {

enum GraphicsMode { Classic, Updated, NoColoring };

class GraphicsCommonClass;  // Forward declaration to support the following type definition
using GraphicsCommonPtr = std::unique_ptr<GraphicsCommonClass>;

// This class is pure virtual
class GraphicsCommonClass {
 public:
  GraphicsCommonClass(void) {};
  GraphicsCommonClass(const GraphicsCommonClass& copyFrom) = delete;
  GraphicsCommonClass& operator=(const GraphicsCommonClass& copyFrom) = delete;
  GraphicsCommonClass(GraphicsCommonClass&&) = delete;
  GraphicsCommonClass& operator=(GraphicsCommonClass&&) = delete;

  virtual ~GraphicsCommonClass(void) {};

 public:
  virtual void LevelInit(const LevelClass& level) = 0;

  virtual void Update(Uint64 counter, const LevelClass& level) = 0;

  virtual void Draw(const alatar::MonsterClassArray& monster_info, const WizardInfo& wizard_info,
                    const BurningLogic::mat4& view_matrix) = 0;

  // Factory function to make sure return objects are always properly constructed and to return the specified
  // sub-class. Ideally should use std:expected but that would require c++23.
  static std::optional<GraphicsCommonPtr> GraphicsCommonClassFactory(std::filesystem::path resource_path,
                                                                     std::filesystem::path shader_path,
                                                                     GraphicsMode graphics_mode,
                                                                     Uint64 sdl_counter);
};

}  // namespace alatar

#endif
