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

#ifndef H_ALATAR_GENERAL_CONSTANTS
#define H_ALATAR_GENERAL_CONSTANTS

#include "wiz_level.h"

namespace alatar {

// Default to 60 frames per second
constexpr double kDefaultFrameRateHz = 60;
constexpr double kMsPerFrame = 1000.0 / kDefaultFrameRateHz;

// The factor of 0.75 is to match the Commodore 64 pixel aspect ratio (NTSC at least)
constexpr float kPixelAspectRatio = 0.75;
constexpr uint_least32_t kScreenWidth = 8 * alatar::kColTiles * 2.5 * kPixelAspectRatio;
constexpr uint_least32_t kScreenHeight = 8 * alatar::kRowTiles * 2.5;
constexpr float kDesiredScreenAspect = static_cast<float>(kScreenWidth) / static_cast<float>(kScreenHeight);

}  // namespace alatar

#endif
