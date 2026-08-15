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

enum { kSpellNumber = 30, kSpellCount = 31, kWizardXLow = 32, kWizardY = 33 };

enum { kBonusSpeed = 82, kSpellColorCode = 83, kSpriteXHighBits = 84 };

enum {
  kSlide0StartTileLowByte = 85,
  kSlide1StartTileLowByte = 86,
  kSlide2StartTileLowByte = 87,
  kSlide0StartTileHighByte = 88,
  kSlide1StartTileHighByte = 89,
  kSlide2StartTileHighByte = 90,
  kSlide0EndTile = 91,
  kSlide1EndTile = 92,
  kSlide2EndTile = 93,
  kSlide0Time = 94,
  kSlide1Time = 95,
  kSlide2Time = 96
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
  int x;
  int y;
  int color;
};

class LevelClass {
 public:
  explicit LevelClass(void) = default;
  explicit LevelClass(std::span<unsigned char, kFileLength> data);

  unsigned char GetTileAt(int row, int col) const;
  unsigned char GetBrickColor(void) const;
  unsigned char GetLadderColor(void) const;
  unsigned char GetRopeColor(void) const;
  unsigned char GetPortalColor(void) const;
  unsigned char GetTileColor(unsigned char c) const;
  WizardInfo GetWizardInfo(void) const;
  alatar::MonsterClassArray GetMonsterInfo(void) const;

 private:
  void SetTileAt(int row, int col, unsigned char c);

 private:
  std::array<unsigned char, kFileLength> raw_data_{};
  unsigned char brick_color_ = 0;
  unsigned char ladder_color_ = 0;
  unsigned char rope_color_ = 0;
  unsigned char portal_color_ = 0;
};

std::string GetC64Color(unsigned char c);
std::string GetSpellName(unsigned char c);
std::string GetMonsterName(unsigned char c);
TileGroup GetTileGroup(unsigned char c);

}  // namespace alatar

#endif
