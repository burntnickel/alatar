#ifndef H_ALATAR_TILES_CLASSIC
#define H_ALATAR_TILES_CLASSIC

#include <SDL.h>

#include <filesystem>
#include <span>

#include "opengl_helper.h"
#include "wiz_level.h"

namespace alatar_classic {

struct ClassicTileGLBuffers {
  GLuint tbo_tile_buffer;
  GLuint tbo_tex_tile_buffer;
  GLuint tbo_color_buffer;
  GLuint tbo_tex_color_buffer;
  GLuint tbo_tileset_buffer;
  GLuint tbo_tex_tileset_buffer;
  GLuint tbo_clut_buffer;
  GLuint tbo_tex_clut_buffer;
};

bool LoadCharSet(std::filesystem::path path_and_name,
                 std::span<unsigned char, alatar::kTileBufferSize> char_set);

void PaintClassicTiles(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                       SDL_FRect sdl_rect, ClassicTileGLBuffers gl_buffers);

}  // namespace alatar_classic

#endif
