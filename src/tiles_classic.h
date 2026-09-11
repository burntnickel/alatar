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
