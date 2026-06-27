#ifndef H_ALATAR_TILES_CLASSIC
#define H_ALATAR_TILES_CLASSIC

#include <SDL.h>

#include "opengl_helper.h"

namespace tiles_classic {

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

void PaintClassicTiles(BurningLogic::ShaderVars shader_vars, SDL_FRect sdl_rect,
                       ClassicTileGLBuffers gl_buffers);

}  // namespace tiles_classic

#endif
