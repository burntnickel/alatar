#ifndef H_ALATAR_SPRITES_CLASSIC
#define H_ALATAR_SPRITES_CLASSIC

#include <filesystem>
#include <span>

#include <SDL.h>

#include "opengl_helper.h"

namespace alatar_classic {

struct ClassicSpriteGLBuffers {
  GLuint sprite_buffer;
  GLuint sprite_texture;
  GLuint clut_buffer;
  GLuint clut_texture;
};

enum { kSpriteNormal = 0, kSpriteFlipX = 1, kSpriteExpandX = 2, kSpriteExpandY = 4, kSpriteMultiColor = 8 };

// 128 sprites in the data file
constexpr std::size_t kNumSprites = 128;

// 64 bytes per sprite
constexpr std::size_t kSpriteSetSize = kNumSprites * 64;

// Size of pre-processed sprite data
// 2 mirror images x 2 horizontal sizes, x 2 vertical sizes, 64*8 bytes per base sprite, 4x for
// 2x horizonal and 2x verical size, and 2x for multi-color vs hires
constexpr std::size_t kSpriteProcDataSize = kNumSprites * 8 * 64 * 8 * 4 * 2;

bool LoadSpriteData(std::filesystem::path path_and_name,
                    std::span<unsigned char, kSpriteProcDataSize> proc_sprite_data);

void PaintClassicSprite(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                        SDL_FRect sdl_rect, ClassicSpriteGLBuffers gl_buffers, unsigned int id,
                        std::array<unsigned int, 3> colors, int modifier);

void DrawClassicSprite(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                       ClassicSpriteGLBuffers gl_buffers, unsigned int id, unsigned int x, unsigned int y,
                       std::array<unsigned int, 3> colors, int modifier = kSpriteNormal);

// Same as DrawClassicSprite but uses the Commorode 64 coordinates
void DrawClassicSpriteC64(BurningLogic::ShaderVars shader_vars, const BurningLogic::mat4& view_matrix,
                          ClassicSpriteGLBuffers gl_buffers, unsigned int id, unsigned int x, unsigned int y,
                          std::array<unsigned int, 3> colors, int modifier = kSpriteNormal);

}  // namespace alatar_classic

#endif
