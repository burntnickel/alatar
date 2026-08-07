#ifndef H_ALATAR_SPRITES_CLASSIC
#define H_ALATAR_SPRITES_CLASSIC

#include <filesystem>
#include <span>

#include <SDL.h>

#include "opengl_helper.h"

namespace alatar_classic {

struct ClassicSpriteGLBuffers {
  GLuint tbo_sprite_buffer;
  GLuint tbo_tex_sprite_buffer;
  //GLenum sprite_texture_unit;
  GLuint tbo_clut_buffer;
  GLuint tbo_tex_clut_buffer;
  //GLenum clut_texture_unit;
};

enum { kSpriteNormal = 0, kSpriteFlipX = 1, kSpriteExpandX = 2, kSpriteExpandY = 4 };

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

void PaintClassicSprite(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                        SDL_FRect sdl_rect, ClassicSpriteGLBuffers gl_buffers, int id,
                        std::array<int, 3> colors, int modifier);

void DrawClassicSprite(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                       ClassicSpriteGLBuffers gl_buffers, int id, int x, int y, std::array<int, 3> colors,
                       int modifier = kSpriteNormal);

// Same as DrawClassicSprite but uses the Commorode 64 coordinates
void DrawClassicSpriteC64(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                          ClassicSpriteGLBuffers gl_buffers, int id, int x, int y, std::array<int, 3> colors,
                          int modifier = kSpriteNormal);

}  // namespace alatar_classic

#endif
