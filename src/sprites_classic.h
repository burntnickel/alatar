#ifndef H_ALATAR_SPRITES_CLASSIC
#define H_ALATAR_SPRITES_CLASSIC

#include <filesystem>
#include <span>

#include <SDL.h>

#include "opengl_helper.h"

namespace sprites_classic {

struct ClassicSpriteGLBuffers {
  GLuint tbo_sprite_buffer;
  GLuint tbo_tex_sprite_buffer;
  GLenum sprite_texture_unit;
  GLuint tbo_clut_buffer;
  GLuint tbo_tex_clut_buffer;
  GLenum clut_texture_unit;
};

// 128 sprites in the data file
constexpr std::size_t kNumSprites = 128;

// 64 bytes per sprite
constexpr std::size_t kSpriteSetSize = kNumSprites * 64;

// Size of pre-processed sprite data
// 2 mirror images x 2 horizontal sizes, x 2 vertical sizes, 64*8 bytes per base sprite, 4x for
// 2x horizonal and 2x verical size
constexpr std::size_t kSpriteProcDataSize = kNumSprites * 8 * 64 * 8 * 4;

bool LoadSpriteData(std::filesystem::path path_and_name,
                    std::span<unsigned char, kSpriteProcDataSize> proc_sprite_data);

void PaintClassicSprites(BurningLogic::ShaderVars shader_vars, SDL_FRect sdl_rect,
                         ClassicSpriteGLBuffers gl_buffers);

}  // namespace sprites_classic

#endif
