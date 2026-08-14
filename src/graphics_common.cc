#include "graphics_common.h"

#include <iostream>

#include "classic.h"

namespace alatar {

std::optional<GraphicsCommonPtr> GraphicsCommonClass::GraphicsCommonClassFactory(
    std::filesystem::path resource_path, std::filesystem::path shader_path, GraphicsMode graphics_mode) {
  switch (graphics_mode) {
    case Classic:
      return alatar_classic::ClassicClass::ClassicClassFactory(resource_path, shader_path);
      break;
    default:
      std::cerr << "Unimplemented graphics mode" << std::endl;
      return {};
  }
}

}  // namespace alatar
