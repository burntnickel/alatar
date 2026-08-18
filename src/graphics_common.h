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

 public:
  double counter_to_ms_scale_;

  // Factory function to make sure return objects are always properly constructed and to return the specified
  // sub-class. Ideally should use std:expected but that would require c++23.
  static std::optional<GraphicsCommonPtr> GraphicsCommonClassFactory(std::filesystem::path resource_path,
                                                                     std::filesystem::path shader_path,
                                                                     GraphicsMode graphics_mode,
                                                                     Uint64 sdl_counter);
};

}  // namespace alatar

#endif
