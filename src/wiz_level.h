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

#ifndef H_ALATAR_WIZ_LEVEL
#define H_ALATAR_WIZ_LEVEL

#include <array>
#include <cstdlib>
#include <span>
#include <string>

#include "monsters.h"

namespace alatar {

constexpr int kFileLength = 1138;
constexpr int kTileDataCols = 40;
constexpr int kTileDataRows = 21;

constexpr unsigned int kRowTiles = 25;
constexpr unsigned int kColTiles = 40;
constexpr unsigned int kLevelTileCount = kRowTiles * kColTiles;

// 256 characters at 8 bytes each
constexpr std::size_t kNumCharInSet = 256;
constexpr std::size_t kCharSetSize = kNumCharInSet * 8;

// Character set represented with a byte per pixel
constexpr std::size_t kTileBufferSize = kCharSetSize * 8;

enum {
  kMonster0XLow = 2,
  kMonster1XLow = 3,
  kMonster2XLow = 4,
  kMonster3XLow = 5,
  kMonster4XLow = 6,
  kMonster5XLow = 7,
  kMonster0Y = 8,
  kMonster1Y = 9,
  kMonster2Y = 10,
  kMonster3Y = 11,
  kMonster4Y = 12,
  kMonster5Y = 13,
  kBrickColorCode = 14,
  kLadderColorCode = 15,
  kRopeColorCode = 16,
  kPortalColorCode = 17,
  kMonster0ColorCode = 18,
  kMonster1ColorCode = 19,
  kMonster2ColorCode = 20,
  kMonster3ColorCode = 21,
  kMonster4ColorCode = 22,
  kMonster5ColorCode = 23
};

enum {
  kElevator0DxDy = 24,
  kElevator1DxDy = 25,
  kElevator2DxDy = 26,
  kElevator3DxDy = 27,
  kElevator4DxDy = 28,
  kElevator5DxDy = 29
};

enum { kSpellNumber = 30, kSpellCount = 31, kWizardXLow = 32, kWizardY = 33 };

enum { kBonusSpeed = 82, kSpellColorCode = 83, kSpriteXHighBits = 84 };

enum {
  kSlide0StartTileLowByte = 85,
  kSlide1StartTileLowByte = 86,
  kSlide2StartTileLowByte = 87,
  kSlide0StartTileHighByte = 88,
  kSlide1StartTileHighByte = 89,
  kSlide2StartTileHighByte = 90,
  kSlide0Stride = 91,
  kSlide1Stride = 92,
  kSlide2Stride = 93,
  kSlide0Time = 94,
  kSlide1Time = 95,
  kSlide2Time = 96,
  kElevatorDuration = 97
};

constexpr unsigned int kSlideTileBase = 0xc400;

enum {
  kMonster0SpriteID = 98,
  kMonster1SpriteID = 99,
  kMonster2SpriteID = 100,
  kMonster3SpriteID = 101,
  kMonster4SpriteID = 102,
  kMonster5SpriteID = 103
};

enum {
  kMonster0AnimationLength = 106,
  kMonster1AnimationLength = 107,
  kMonster2AnimationLength = 108,
  kMonster3AnimationLength = 109,
  kMonster4AnimationLength = 110,
  kMonster5AnimationLength = 111
};

enum {
  kSafeColorCode = 113,
  kMonster0MonsterID = 114,
  kMonster1MonsterID = 115,
  kMonster2MonsterID = 116,
  kMonster3MonsterID = 117,
  kMonster4MonsterID = 118,
  kMonster5MonsterID = 119
};

enum { kTileDataStart = 298, kTileDataEnd = 1137 };

enum { kLevelNameStart = 1106, kLevelNameEnd = 1129 };

enum TileGroup {
  kUnknown,
  kKnownUnknown,
  kText,
  kKey,
  kTreasure,
  kKeyhole,
  kBrick,
  kRope,
  kLadder,
  kPortal,
  kTeleport,
  kDeath,
  kFire
};

enum C64Colors {
  kColorBlack = 0,
  kColorWhite = 1,
  kColorRed = 2,
  kColorCyan = 3,
  kColorPurple = 4,
  kColorGreen = 5,
  kColorBlue = 6,
  kColorYellow = 7,
  kColorOrange = 8,
  kColorBrown = 9,
  kColorPink = 10,
  kColorDarkGrey = 11,
  kColorGrey = 12,
  kColorLightGreen = 13,
  kColorLightBlue = 14,
  kColorLightGrey = 15
};

// High bit of x-position masks
enum XMask {
  kXMaskMonster0 = 1,
  kXMaskMonster1 = 2,
  kXMaskMonster2 = 4,
  kXMaskMonster3 = 8,
  kXMaskMonster4 = 16,
  kXMaskMonster5 = 32,
  kXMaskWizard = 64 + 128  // Not sure about this one
};

struct WizardInfo {
  unsigned int x;
  unsigned int y;
  unsigned int color;
};

struct SlideInfo {
  bool active;
  unsigned int start_tile_offset;
  unsigned int stride;
  unsigned int slide_time;
  unsigned int normal_time;
};

constexpr int kNumSlides = 3;
using SlideInfoArray = std::array<SlideInfo, kNumSlides>;

using MaskTiles = std::array<unsigned char, 6>;

using WizardLevelBuffer = std::array<unsigned char, kLevelTileCount>;

class LevelClass {
 public:
  explicit LevelClass(void) = default;
  explicit LevelClass(std::span<unsigned char, kFileLength> data);

  unsigned char GetTileAt(int row, int col) const;
  unsigned char GetTileAtOffset(unsigned int offset) const;
  unsigned char GetBrickColor(void) const;
  unsigned char GetLadderColor(void) const;
  unsigned char GetRopeColor(void) const;
  unsigned char GetPortalColor(void) const;
  unsigned char GetTileColor(unsigned char c) const;
  WizardInfo GetWizardInfo(void) const;
  alatar::MonsterClassArray GetMonsterInfo(void) const;
  //MaskTiles GetMaskTiles(int row, int col) const;
  SlideInfoArray GetSlideInfo(void) const;

 private:
  void SetTileAt(int row, int col, unsigned char c);
  void SetTileOffset(unsigned int offset, unsigned char c);

 private:
  std::array<unsigned char, kFileLength> raw_data_{};
  unsigned char brick_color_ = 0;
  unsigned char ladder_color_ = 0;
  unsigned char rope_color_ = 0;
  unsigned char portal_color_ = 0;
  SlideInfoArray slide_info_array_;
};

std::string GetC64Color(unsigned char c);
std::string GetSpellName(unsigned char c);
std::string GetMonsterName(unsigned char c);
TileGroup GetTileGroup(unsigned char c);

MaskTiles GetMaskTiles(const WizardLevelBuffer& buffer, int row, int col);

}  // namespace alatar

#endif
