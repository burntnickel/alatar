#ifndef H_ALATAR_WIZ_LEVEL
#define H_ALATAR_WIZ_LEVEL

#include <span>

namespace wizard_level {

constexpr int kFileLength = 1138;
constexpr int kTileDataCols = 40;
constexpr int kTileDataRows = 21;

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
  kColorLightBlur = 14,
  kColorLightGrey = 15
};

class LevelClass {
 public:
  LevelClass(std::span<unsigned char, kFileLength> data);

  unsigned char GetTileAt(int row, int col) const;
  unsigned char GetBrickColor(void) const;
  unsigned char GetLadderColor(void) const;
  unsigned char GetRopeColor(void) const;
  unsigned char GetPortalColor(void) const;
  unsigned char GetTileColor(unsigned char c) const;

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

}  // namespace wizard_level

#endif
