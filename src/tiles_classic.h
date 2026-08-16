#ifndef H_ALATAR_TILES_CLASSIC
#define H_ALATAR_TILES_CLASSIC

#include <SDL.h>

#include <filesystem>
#include <span>

#include "opengl_helper.h"
#include "wiz_level.h"

namespace alatar_classic {

struct ClassicTileGLBuffers {
  GLuint tile_buffer;
  GLuint tile_texture;
  GLuint color_buffer;
  GLuint color_texture;
  GLuint tileset_buffer;
  GLuint tileset_texture;
  GLuint clut_buffer;
  GLuint clut_texture;
};

bool LoadCharSet(std::filesystem::path path_and_name,
                 std::span<unsigned char, alatar::kTileBufferSize> char_set);

void PaintClassicTiles(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                       SDL_FRect sdl_rect, ClassicTileGLBuffers gl_buffers);

}  // namespace alatar_classic

#endif
