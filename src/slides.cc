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

#include "wiz_level.h"

namespace alatar {

void UpdateSlides(const LevelClass& level, WizardLevelBuffer& tiles) {
  auto slide_info = level.GetSlideInfo();
  unsigned char base_tile;

  for (const auto& slide : slide_info) {
    if (slide.active) {
      unsigned int base_offset = slide.start_tile_offset;
      unsigned int stride = slide.stride;

      base_tile = tiles[base_offset];
      tiles[base_offset] = 1;

      unsigned int offset = base_offset + stride;
      int count = 0;

      while ((offset < kLevelTileCount) && (tiles[offset] == base_tile)) {
        ++count;
        tiles[offset] = 1 + count;
        offset += stride;
      }
    }
  }
}

}  // namespace alatar
