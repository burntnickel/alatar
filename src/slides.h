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

#ifndef H_ALATAR_SLIDES
#define H_ALATAR_SLIDES

#include <array>
#include <unordered_map>

#include "wiz_level.h"

namespace alatar {

constexpr double kSlideSpeedModifier = 3.0; // TODO: Config file?

struct SlideState {
  bool valid = false;
  bool normal = true;
  unsigned char normal_tile;
  unsigned char slide_tile;
  int tile_count = 0;
  Uint64 old_counter = 0;
};

using SlideStateCache = std::array<SlideState, kNumSlides>;

// Mappings of tile pairs for slides
// 1) Floor (92) -> Space (blank) (32)
// 2) Steep Staris Left (93) -> Slide Left (122)
// 3) Steep Stairs Right (94) -> Slight Right (123)
const std::unordered_map<unsigned char, unsigned char> kSlideMapping{{92, 32}, {93, 122}, {94, 123}};

void InitSlides(const LevelClass& level, const WizardLevelBuffer& tiles, SlideStateCache& slide_states);

void UpdateSlides(const LevelClass& level, WizardLevelBuffer& tiles, SlideStateCache& slide_states,
                  Uint64 counter);

}  // namespace alatar

#endif
