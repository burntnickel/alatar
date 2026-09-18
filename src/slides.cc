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

#include "slides.h"

#include "globals.h"
#include "wiz_level.h"

namespace alatar {

void InitSlides(const LevelClass& level, const WizardLevelBuffer& tiles, SlideStateCache& slide_states) {
  auto slide_info = level.GetSlideInfo();

  for (unsigned int ii = 0; ii < kNumSlides; ++ii) {
    auto slide = slide_info[ii];

    if (slide.active) {
      unsigned int base_offset = slide.start_tile_offset;
      unsigned int stride = slide.stride;

      unsigned char base_tile = tiles[base_offset];

      if (kSlideMapping.contains(base_tile)) {
        unsigned int offset = base_offset + stride;
        int count = 1;

        while ((offset < kLevelTileCount) && (tiles[offset] == base_tile)) {
          ++count;
          offset += stride;
        }

        slide_states[ii].valid = true;
        slide_states[ii].normal = true;
        slide_states[ii].normal_tile = base_tile;
        slide_states[ii].slide_tile = kSlideMapping.find(base_tile)->second;
        slide_states[ii].tile_count = count;
      } else {
        slide_states[ii].valid = false;
      }
    }
  }
}

void UpdateSlides(const LevelClass& level, WizardLevelBuffer& tiles, SlideStateCache& slide_states,
                  Uint64 counter) {
  auto slide_info = level.GetSlideInfo();

  for (unsigned int ii = 0; ii < kNumSlides; ++ii) {
    if (slide_info[ii].active && slide_states[ii].valid) {
      if (slide_states[ii].old_counter == 0) {
        slide_states[ii].old_counter = counter;
      }

      double elapsed_ms =
          static_cast<double>(counter - slide_states[ii].old_counter) * alatar::gCounterToMsScale;
      double elapsed_s = elapsed_ms / 1000.0;

      unsigned int base_offset = slide_info[ii].start_tile_offset;
      unsigned int stride = slide_info[ii].stride;

      if (slide_states[ii].normal) {
        if ((elapsed_s * kSpeedFactor * kSlideSpeedModifier) >
            static_cast<double>(slide_info[ii].normal_time)) {
          slide_states[ii].old_counter = counter;
          slide_states[ii].normal = false;

          unsigned char base_tile = tiles[base_offset];

          if (kSlideMapping.contains(base_tile)) {
            auto slide_tile = kSlideMapping.find(base_tile)->second;

            tiles[base_offset] = slide_tile;

            unsigned int offset = base_offset + stride;
            int count = 1;

            while ((offset < kLevelTileCount) && (tiles[offset] == base_tile)) {
              tiles[offset] = slide_tile;
              ++count;
              offset += stride;
            }

            slide_states[ii].valid = true;
            slide_states[ii].normal_tile = base_tile;
            slide_states[ii].slide_tile = slide_tile;
            slide_states[ii].tile_count = count;
          } else {
            slide_states[ii].valid = false;
          }
        }
      } else {
        if ((elapsed_s * kSpeedFactor * kSlideSpeedModifier) >
            (static_cast<double>(slide_info[ii].slide_time) / kSpeedFactor)) {
          slide_states[ii].old_counter = counter;
          slide_states[ii].normal = true;

          unsigned int offset = 0;

          for (int jj = 0; jj < slide_states[ii].tile_count; ++jj) {
            tiles[base_offset + offset] = slide_states[ii].normal_tile;
            offset += stride;
          }
        }
      }
    }
  }

  /* auto slide_info = level.GetSlideInfo();

   for (unsigned int ii = 0; ii < kNumSlides; ++ii) {
     auto slide = slide_info[ii];

     if (slide.active) {
       unsigned int base_offset = slide.start_tile_offset;
       unsigned int stride = slide.stride;

       unsigned char base_tile = tiles[base_offset];

       // If the slide is set for unsupported tile type do nothing
       if (kSlideMapping.contains(base_tile)) {
       }


     }
   }*/
}

}  // namespace alatar
